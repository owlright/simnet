#include "Routing.h"
Define_Module(Routing);

void Routing::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        myAddress = getParentModule()->par("address"); // HACK
        ecmpFlow = par("ecmpFlow").boolValue();
        dropSignal = registerSignal("drop");
        outputIfSignal = registerSignal("outputIf");
        isSwitch = (getParentModule()->getProperties()->get("switch") != nullptr);
        // WATCH_MAP(rtable); // ! this causes error if value is vector
        routeManager = findModuleFromTopLevel<GlobalRouteManager>("routeManager", this);
        if (!routeManager) // ! this module is necessary
            throw cRuntimeError("no routeManager!");

//        if (strcmp(par("groupManager").stringValue(), "") != 0)
        // groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);

        if (isSwitch) {
            bufferSize = par("bufferSize");
            numAggregators = par("numAggregators");
            aggregators.resize(numAggregators, nullptr);
            collectionPeriod = par("collectPeriod").doubleValueInUnit("s");
            aggPolicy = par("aggPolicy").stdstringValue();
            isTimerPolicy = (aggPolicy == "Timer");
            aggTimeOut = new cMessage("aggTimeOut");
            // dataCollectTimer = new cMessage("dataCollector");
        }

    }
    if (stage == INITSTAGE_ASSIGN) {
        // if (!isSwitch && groupManager != nullptr)
            // myGroupAddress = groupManager->getGroupAddress(myAddress);
    }
    if (stage == INITSTAGE_LAST) {
        if (isSwitch) {
            // scheduleAfter(collectionPeriod, dataCollectTimer);
        }
    }
}

Routing::~Routing()
{
    if (isSwitch) {
        cancelAndDelete(aggTimeOut);
        cancelAndDelete(dataCollectTimer);
    }

}

int Routing::getRouteGateIndex(int srcAddr, int destAddr)
{
    // srcAddr is only used for ecmp the flow
    RoutingTable::iterator it = rtable.find(destAddr);
    if (it != rtable.end()) {
        auto outGateIndexes = (*it).second;
        if (ecmpFlow) {
            auto N = srcAddr + destAddr + myAddress; // HACK a too simple hash function
            return outGateIndexes.at(N % outGateIndexes.size());
        } else {
            return outGateIndexes.at(0);
        }
    }
    else {
        int address = destAddr;
        auto outGateIndexes = routeManager->getRoutes(myAddress, address); // ! pass switchAddress not srcAddress
        rtable[destAddr] = outGateIndexes;
        return getRouteGateIndex(srcAddr, destAddr); // ! recursion find outgate index
    }
}

void Routing::broadcast(Packet *pk, const std::vector<int>& outGateIndexes) {
    for (auto& gateIndex : outGateIndexes ) {
        auto packet = pk->dup();
        EV_INFO << "Forwarding broadcast packet " << pk->getName() << " on gate index " << gateIndex << endl;
        send(packet, "out", gateIndex);
    }
    delete pk;
}

std::vector<int> Routing::getReversePortIndexes(const AddrSeqType& groupSeqKey) const
{

    if (incomingPortIndexes.find(groupSeqKey) == incomingPortIndexes.end())
        throw cRuntimeError("Routing::getReversePortIndexes: group %lld seq %lld not found!",
                                    groupSeqKey.first, groupSeqKey.second);
    return incomingPortIndexes.at(groupSeqKey);
}

int Routing::getComputationCount() const
{
    int c = 0;
    for (const auto& kv: groupTable) {
        c += kv.second->getComputationCount();
    }
    return c;
}

simtime_t Routing::getUsedTime() const
{
    simtime_t t = 0;
    for (const auto& kv: groupTable) {
        t += kv.second->getUsedTime();
    }
    return t;
}

simsignal_t Routing::createBufferSignalForGroup(IntAddress group)
{
    char signalName[32];
    sprintf(signalName, "group%lld-usedBuffer", group);
    simsignal_t signal = registerSignal(signalName);

    char statisticName[32];
    sprintf(statisticName, "group%lld-usedBuffer", group);
    cProperty *statisticTemplate =
        getProperties()->get("statisticTemplate", "groupUsedBuffer");
    getEnvir()->addResultRecorders(this, signal, statisticName, statisticTemplate);
    return signal;
}

void Routing::forwardIncoming(Packet *pk)
{
    auto destAddr = pk->getDestAddr();
    if (pk->getPacketType() == AGG)
    {
        auto apk = check_and_cast<AggPacket*>(pk);
        auto dest = apk->getDestAddr();
        auto seq = apk->getSeqNumber();
        auto destSeqKey = std::make_pair(dest, seq);
        if (!apk->isAck())
        {
            auto agtrIndex = apk->getAggregatorIndex();
            if (aggregators.at(agtrIndex) == nullptr) { // lazy initialization to save memory
                switch(apk->getAggPolicy())
                {
                    case ATP:
                        aggregators[agtrIndex] = new ATPEntry();
                        break;
                    case MTATP:
                        aggregators[agtrIndex] = new MTATPEntry();
                        break;
                    default:
                        throw cRuntimeError("unknown agg policy");
                }

            }
            auto entry = aggregators[agtrIndex];
            if (entry->checkAdmission(apk)) {
                pk = entry->doAggregation(apk);
            } else {
                // this means collision
                // set these fields before forwarding
                apk->setCollision(true);
                apk->setResend(true); // TODO I don't know why ATP set this
                if (apk->getAggPolicy() == ATP)
                    check_and_cast<ATPPacket*>(apk)->setSwitchIdentifier(1);
            }
        }
        else
        {
            // ! if group does not deal with this group, then its group table is empty
            // ! but it still need to send ACK reversely back to incoming ports
            auto outGateIndexes = getReversePortIndexes(destSeqKey);
            incomingPortIndexes.erase(destSeqKey); // ! avoid comsuming too much memory
            broadcast(pk, outGateIndexes);
            return;
        }

        if (pk == nullptr) // ! Aggregation is not finished;
            return;
    }

    // route this packet which may be:
    // 1. unicast packet
    // 2. finished aggregated packet
    // 2. group packet not responsible for
    // 3. group packet failed to be aggregated
    auto srcAddr = pk->getSrcAddr();
    int outGateIndex = getRouteGateIndex(srcAddr, destAddr);
    if (outGateIndex == -1) { // ! TODO if not found, routeManager will throw an error, the code is useless
        EV << "address " << destAddr << " unreachable, discarding packet " << pk->getName() << endl;
        emit(dropSignal, (intval_t)pk->getByteLength());
        delete pk;
        return;
    }
    EV << "Forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;
    send(pk, "out", outGateIndex);
}

Packet* Routing::doAggregation(Packet *pk)
{
    auto group = pk->getDestAddr();
    auto seq = pk->getSeqNumber();
    auto groupSeqKey = std::make_pair(group, seq);
    ASSERT(groupTable.find(group) != groupTable.end());
    ASSERT(groupTable.at(group)->isSeqPresent(seq));
    if (pk->getKind() == REMIND) // a dummy packet to remind aggregation is over
    {
        ASSERT(pk->isSelfMessage());
        ASSERT(groupTable.find(group) != groupTable.end());
        pk = groupTable.at(group)->agg(pk);
        ASSERT(pk!=nullptr);
    }
    else if (pk->getKind() == DATA) {
        // ! before aggregation do things here
        if (isTimerPolicy) {
            // ! in case the last packet of aggregation not leaving
            auto timeout = SimTime(check_and_cast<MTATPPacket*>(pk)->getTimer(), SIMTIME_NS);
            auto deadline = simTime() + timeout;
            seqDeadline[groupSeqKey] = deadline.inUnit(SIMTIME_NS);
            if (!aggTimeOut->isScheduled() || deadline < aggTimeOut->getArrivalTime() ) {
                rescheduleAfter(timeout, aggTimeOut);
            }
            seqDeadline[groupSeqKey] = simTime().inUnit(SIMTIME_NS) + check_and_cast<MTATPPacket*>(pk)->getTimer();
        }
        // ! aggregation is always the last thing to do as it change the pk pointer
        pk = groupTable.at(group)->agg(pk);
    }

    // aggregation is finished
    if (pk != nullptr) {
        if (isTimerPolicy) {
            auto releasedBuffer = groupTable[group]->release(pk);
            usedBuffer -= releasedBuffer;
            seqDeadline.erase(groupSeqKey);
            EV_DEBUG <<"group " << group << " seq " << seq << " release buffer " << releasedBuffer << " bytes" << endl;
        }
    }
    return pk;
}

bool Routing::addGroupEntry(IntAddress group, B bufferCanUsed, B firstDataSize, int indegree)
{
    ASSERT(groupTable.find(group) == groupTable.end());
    // setup for the group
    groupTable[group] = new AggGroupEntry(bufferSize, indegree);
    groupTable.at(group)->usedBufferSignal = createBufferSignalForGroup(group);
    // if (!dataCollectTimer->isScheduled())
    //     scheduleAfter(collectionPeriod, dataCollectTimer);
    // TODO all group use the same aggPolicy for now
    groupTable.at(group)->setAggPolicy(aggPolicy);
    if (bufferSize - usedBuffer >= firstDataSize)
    {
        // ! this check maybe unnecessary, unless you set group canUsedBufferSize < pk->getByteLength() which makes no sense
        if (groupTable.at(group)->getLeftBuffer() >= firstDataSize)
        {
            usedBuffer += firstDataSize;
            return true;
        }
    }
    return false;
}

bool Routing::tryAddSeqEntry(const Packet* pk)
{
    auto group = pk->getDestAddr();
    auto seq = pk->getSeqNumber();
    // ! make sure the seq not exist
    ASSERT(groupTable.find(group) != groupTable.end()
            && !groupTable.at(group)->isSeqPresent(seq));
    return groupTable.at(group)->addSeqEntry(pk);
}

void Routing::recordIncomingPorts(AddrSeqType& groupSeqKey, int port)
{
    // TODO maybe use unordered_set?
    bool found = false;
    for (auto& p: incomingPortIndexes[groupSeqKey])
    {
        if (port == p)
        {
            found = true;
            break;
        }
    }
    if (!found)
        incomingPortIndexes[groupSeqKey].push_back(port);
}

void Routing::handleMessage(cMessage *msg)
{
    if (msg == aggTimeOut)
    {
        int count = 0;
        for (auto& groupSeqTimeout : seqDeadline) {
            AddrSeqType groupSeq = groupSeqTimeout.first;
            auto group = groupSeq.first;
            auto seq = groupSeq.second;
            auto timeout = SimTime(groupSeqTimeout.second, SIMTIME_NS);
            if (timeout <= simTime()) {
                count += 1;
                EV_DEBUG <<"group " << group << " seq " << seq << " reaches its deadline." << endl;
                // make a fake packet
                auto pk = new Packet("dummy");
                pk->setKind(REMIND);
                pk->setDestAddr(group);
                pk->setSeqNumber(seq);
                // pk->setAggCounter(0);
                scheduleAt(simTime(), pk);
            }
        }
        if (count < seqDeadline.size())
        {
            // ! Relying solely on setting a timeout when data packets arrive is not enough.
            // ! Consider this scenario: when the last few seq packets of the aggregated stream arrive,
            // ! the timeout has been set to a relatively small value by the previous seq packets.
            // ! Therefore, these last few seq packets will not update the timeout to a larger value.
            // ! As a result, when the timeout expires and no more packets arrive, the timeout will never receive a new value.
            auto timeout = SimTime(1, SIMTIME_US);
            scheduleAfter(timeout, aggTimeOut);
        }
        return;
    }
    // if (msg == dataCollectTimer)
    // {
    //     for (auto const& p : groupTable)
    //     {
    //         auto entry = p.second;
    //         auto used = entry->getUsedBuffer();
    //         emit(entry->usedBufferSignal, used);
    //     }
    //     if (!groupTable.empty())
    //         scheduleAfter(collectionPeriod, dataCollectTimer);
    //     return;
    // }
    Packet *pk = check_and_cast<Packet *>(msg);
    auto destAddr = pk->getDestAddr();
    if (pk->getArrivalGate() == gate("localIn"))
    {
        // ! only host has localIn, so save the time to ask route manager
        // TODO: but if this is always right? such as sending to another app
        EV << "Send out packet " << pk->getName() << " on gate index " << 0 << endl;
        send(pk, "out", 0);
        return;
    }

    if (destAddr == myAddress)
    {
        // destination is me
        EV_TRACE << "deliver packet to upperLayer" << pk->getName() << endl;
        send(pk, "localOut");
        return;
    }
    else if (isSwitch)
    {
        forwardIncoming(pk);
    }
    else
    {
        throw cRuntimeError("%lld is not a router", myAddress);
    }

}

void Routing::refreshDisplay() const
{
    if (!getEnvir()->isExpressMode()) {
        char buf[20];
        sprintf(buf, "%" PRId64, myAddress);
        getParentModule()->getDisplayString().setTagArg("t", 0, buf);
    }
}

void Routing::finish()
{
    if (isSwitch) {
        char buf[30];
        sprintf(buf, "switch-%lld-compEff", myAddress);
        // I dont want the time's unit too big, otherwise the efficiency will be too big
        recordScalar(buf, getComputationCount() / double(getUsedTime().inUnit(SIMTIME_US))); // TODO will resource * usedTime better?
    }

}
