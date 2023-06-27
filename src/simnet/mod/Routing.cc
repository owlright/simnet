#include "Routing.h"
Define_Module(Routing);

void Routing::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        ecmpFlow = par("ecmpFlow").boolValue();
        dropSignal = registerSignal("drop");
        outputIfSignal = registerSignal("outputIf");
        isSwitch = (getParentModule()->getProperties()->get("switch") != nullptr);

        routeManager = findModuleFromTopLevel<GlobalRouteManager>("routeManager", this);
        if (!routeManager) // ! this module is necessary
            throw cRuntimeError("no routeManager!");

        if (isSwitch) {
            bufferSize = par("bufferSize");
            numAggregators = getParentModule()->par("numAggregators");
            agtrSize = getParentModule()->par("agtrSize");
            position = getParentModule()->par("position");
            aggregators.resize(numAggregators, nullptr);
            collectionPeriod = par("collectPeriod").doubleValueInUnit("s");
//            aggPolicy = par("aggPolicy").stdstringValue();
//            isTimerPolicy = (aggPolicy == "Timer");
            aggTimeOut = new cMessage("aggTimeOut");
            dataCollectTimer = new cMessage("dataCollector");
        }

    }
    else if (stage == INITSTAGE_ACCEPT) {
        myAddress = getParentModule()->par("address"); // HACK
    }
    if (stage == INITSTAGE_LAST) {
        if (isSwitch) {
            EV_TRACE << "router " << myAddress << "'s position is " << position << endl;
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

std::vector<int> Routing::getReversePortIndexes(const MulticastID& mKey) const
{
    if (incomingPortIndexes.find(mKey) == incomingPortIndexes.end())
        throw cRuntimeError("Routing::getReversePortIndexes: group: %" PRId64 ":%" PRId64 " seq %" PRId64 "not found!",
                                    mKey.addr, mKey.port, mKey.seq);
    return incomingPortIndexes.at(mKey);
}

int Routing::getComputationCount() const
{
    int c = 0;
    for (const auto& kv: groupMetricTable) {
        c += kv.second->getComputationCount();
    }
    return c;
}

simtime_t Routing::getUsedTime() const
{
    simtime_t t = 0;
    for (const auto& kv: groupMetricTable) {
        t += kv.second->getUsedTime();
    }
    return t;
}

simsignal_t Routing::createBufferSignalForGroup(IntAddress group)
{
    char signalName[32];
    sprintf(signalName, "group %" PRId64 "usedBuffer", group);
    simsignal_t signal = registerSignal(signalName);

    char statisticName[32];
    sprintf(statisticName, "group %" PRId64 "-usedBuffer", group);
    cProperty *statisticTemplate =
        getProperties()->get("statisticTemplate", "groupUsedBuffer");
    getEnvir()->addResultRecorders(this, signal, statisticName, statisticTemplate);
    return signal;
}

void Routing::forwardIncoming(Packet *pk)
{
    auto destAddr = pk->getDestAddr();
    auto srcAddr = pk->getSrcAddr();
    auto seq = pk->getSeqNumber();
    auto destSeqKey = std::make_pair(destAddr, seq);

    auto entryIndex = pk->getLastEntry();
    // ! entryIndex is unsigned, do not check it == -1
    auto segment = pk->getSegmentsArraySize() > 0 ? pk->getSegments(entryIndex) : -1;
    if (segment == myAddress) {
        auto& fun = pk->getFuns(entryIndex);
        if (fun == "aggregation") {
            auto apk = check_and_cast<AggPacket*>(pk);
            auto jobId = apk->getJobId();
            auto PSport = apk->getDestPort();
            MulticastID mKey = {destAddr, PSport, seq};
            recordIncomingPorts(mKey, pk->getArrivalGate()->getIndex());
            if (groupMetricTable.find(jobId) == groupMetricTable.end()) {
                // the first time we see this group
                groupMetricTable[jobId] = new jobMetric(this, jobId);
                groupMetricTable[jobId]->createBufferSignalForGroup(jobId);
            }

            pk = aggregate(apk);
            if (pk == nullptr) {// ! Aggregation is not finished;
                return;
            } else { // TODO: do we release resource at aggpacket leave or ACK arrive?
                ASSERT(pk == apk);
                // ! pop the segments
                apk->popSegment();
                apk->popFun();
                apk->popArg();
                apk->setLastEntry(apk->getLastEntry() - 1);
                auto agtrIndex = apk->getAggregatorIndex();
                auto job = apk->getJobId();
                auto seq = apk->getSeqNumber();
                auto agtr = aggregators.at(agtrIndex);
                // ! 1. agtr can be nullptr when router is not responsible for this group
                // ! 2. collision may happen so the agtr doesn't belong to the packet
                // ! 3. it's a resend packet arrives at switch==1 and there's no according agtr for it
                if (agtr != nullptr
                        && agtr->getJobId() == job
                        && agtr->getSeqNumber() == seq) {
                    delete aggregators[agtrIndex];
                    aggregators[agtrIndex] = nullptr;
                    groupMetricTable.at(apk->getJobId())->releaseUsedBuffer(agtrSize);
                }
            }
        }
    }
    if (pk->getPacketType() == MACK) { // TODO very strange, groupAddr is totally useless here
        MulticastID srcSeqKey = {srcAddr, pk->getLocalPort(), seq};
        // ! if group does not deal with this group, then its group table is empty
        // ! but it still need to send ACK reversely back to incoming ports
        auto outGateIndexes = getReversePortIndexes(srcSeqKey);
        incomingPortIndexes.erase(srcSeqKey); // ! avoid comsuming too much memory
        // groupMetricTable.at(apk->getJobId())->releaseUsedBuffer(agtrSize);
        broadcast(pk, outGateIndexes);
        return;
    }

    // route this packet which may be:
    // 1. unicast packet
    // 2. finished aggregated packet
    // 3. group packet not responsible for
    // 4. group packet failed to be aggregated(hash collision, resend)
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

Packet* Routing::aggregate(AggPacket *apk)
{
    auto agtrIndex = apk->getAggregatorIndex();
    if (aggregators.at(agtrIndex) == nullptr) { // lazy initialization to save memory
        ASSERT(groupMetricTable.find(apk->getJobId()) != groupMetricTable.end());

        switch(apk->getAggPolicy())
        {
            case ATP:
                if (position == 1) {
                    groupMetricTable.at(apk->getJobId())->addUsedBuffer(agtrSize);
                    aggregators[agtrIndex] = new ATPEntry(apk);
                }
                else // ! ATP only do aggregation at edge switches
                    return apk;
                break;
            case MTATP:
                aggregators[agtrIndex] = new MTATPEntry();
                break;
            case SRAGG:
                if (!apk->getResend())
                    aggregators[agtrIndex] = new Aggregator();
                else  // ! a resend packet mustn't get an idle aggregator
                    return apk;
                break;
            default:
                throw cRuntimeError("unknown agg policy");
        }
    }
    auto entry = aggregators.at(agtrIndex);
    if (entry->checkAdmission(apk))
    {
        return entry->doAggregation(apk);
    }
    else {
        // this means collision happened;
        // set these fields before forwarding
        apk->setCollision(true);
        apk->setResend(true); // TODO I don't know why ATP set this, maybe prevent switch 1 do aggregation, but why not just check collision?
        return apk;
    }
}

void Routing::recordIncomingPorts(MulticastID& addrSeqKey, int port)
{
    // TODO maybe use unordered_set?
    bool found = false;
    for (auto& p: incomingPortIndexes[addrSeqKey])
    {
        if (port == p)
        {
            found = true;
            break;
        }
    }
    if (!found)
        incomingPortIndexes[addrSeqKey].push_back(port);
}

void Routing::handleMessage(cMessage *msg)
{
    if (msg == aggTimeOut)
    {
        int count = 0;
        for (auto& groupSeqTimeout : seqDeadline) {
            MulticastID mKey = groupSeqTimeout.first;
            auto group = mKey.addr;
            auto seq = mKey.port;
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
    if (msg == dataCollectTimer)
    {
        for (auto const& p : groupMetricTable)
        {
            auto entry = p.second;
            entry->emitAllSignals();
        }
        if (!groupMetricTable.empty())
            scheduleAfter(collectionPeriod, dataCollectTimer);
        return;
    }
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
    else if (pk->getPacketType() == MACK && !isSwitch)
    {   // TODO FIXME should register multicast member at this interface
        EV_TRACE << "received a multicast packet: "<< pk->getName()  << ", deliver it to upperLayer." << endl;
        send(pk, "localOut");
        return;
    }
    else if (isSwitch)
    {
        forwardIncoming(pk);
    }
    else
    {
        throw cRuntimeError("%" PRId64 " is not a router", myAddress);
    }

}

void Routing::finish()
{
    if (isSwitch) {
        char buf[30];
        sprintf(buf, "switch-%" PRId64 "-compEff", myAddress);
        // I dont want the time's unit too big, otherwise the efficiency will be too big
        recordScalar(buf, getComputationCount() / double(getUsedTime().inUnit(SIMTIME_US))); // TODO will resource * usedTime better?
    }
    for (auto& p : aggregators) {
        if (p!=nullptr) {
            EV_WARN << "there is unreleased aggregator on router " << myAddress
            << " belongs to job " << p->getJobId() << " seq " << p->getSeqNumber() << endl;
        }
    }
}
