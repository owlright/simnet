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
        groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);

        if (isSwitch) {
            bufferSize = par("bufferSize");
            aggPolicy = par("aggPolicy").stdstringValue();
            isTimerPolicy = (aggPolicy == "Timer");
            aggTimeOut = new cMessage("aggTimeOut");
        }

    }
    if (stage == INITSTAGE_ASSIGN) {
        if (!isSwitch && groupManager != nullptr)
            myGroupAddress = groupManager->getGroupAddress(myAddress);
    }
}

Routing::~Routing()
{
    if (isSwitch)
        cancelAndDelete(aggTimeOut);
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
        if (isGroupAddr(destAddr)) {
            address = groupManager->getGroupRootAddress(destAddr);
        }
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

std::vector<int> Routing::getReversePortIndexes(const GroupSeqType& groupSeqKey) const
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

void Routing::handleMessage(cMessage *msg)
{
    if (msg == aggTimeOut)
    {
        for (auto& groupSeqTimeout : seqDeadline) {
            GroupSeqType groupSeq = groupSeqTimeout.first;
            auto group = groupSeq.first;
            auto seq = groupSeq.second;
            auto timeout = SimTime(groupSeqTimeout.second, SIMTIME_NS);
            if (timeout <= simTime()) {
                EV_DEBUG <<"group " << group << " seq " << seq << " reaches its deadline." << endl;
                // make a fake packet
                auto pk = new Packet("dummy");
                pk->setKind(DATA);
                pk->setDestAddr(group);
                pk->setSeqNumber(seq);
                pk->setAggCounter(0);
                scheduleAt(simTime(), pk);
            }
        }
        return;
    }
    Packet *pk = check_and_cast<Packet *>(msg);
    auto srcAddr = pk->getSrcAddr();
    auto destAddr = pk->getDestAddr();
    // ! If the packet destAddr is me(unicast) or my address is the group root
    // ! send the packet to upperLayer
    if ((destAddr == myAddress || destAddr == myGroupAddress)
            && strcmp(pk->getArrivalGate()->getName(),"in")==0)
    {
        // the group root is me
        // packet must come from lower layer
        EV << "local delivery of packet " << pk->getName() << endl;
        send(pk, "localOut");
        return;
    }

    // ! If I'm a host, the packet comes from upperLayer
    if (!isSwitch) {
    // HACK: if this node is host, only gate out[0] can be sent to
    // TODO: save the time to ask route manager, but if this is always right?
        EV << "Forwarding packet " << pk->getName() << " on gate index " << 0 << endl;
        send(pk, "out", 0);
        return;
    }

    // ! Only switch deal with group address
    if (isSwitch && isGroupAddr(destAddr) ) {
        auto group = pk->getDestAddr();
        auto seq = pk->getSeqNumber();
        auto groupSeqKey = std::make_pair(group, seq);

        auto indegree = groupManager->getFanIndegree(group, 0, myAddress); // TODO: the treeIndex is fixed to 0
        if (indegree == -1) {
            // ! this switch doesn't deal with this group
            markNotAgg.insert(groupSeqKey);
        }

        if (pk->getKind() == DATA) {
            // ! but it still need to record incoming ports because the ACK packet
            // ! has to be sent reversely.
            if (!pk->isSelfMessage()) {// ! in case this is a dummy packet
                auto newPort = pk->getArrivalGate()->getIndex();
                bool found = false;
                for (auto& port: incomingPortIndexes[groupSeqKey])
                {
                    if (newPort == port)
                        found = true;
                }
                if (!found)
                    incomingPortIndexes[groupSeqKey].push_back(newPort);
            }
            if (markNotAgg.find(groupSeqKey) == markNotAgg.end())
            {
                if (groupTable.find(group) != groupTable.end()) // already have an entry
                {
                    if (groupTable.at(group)->getLeftBuffer() > 0) { // ! a group may have its own restriction
                        if (isTimerPolicy) {
                            // ! in case the last packet of aggregation not leaving
                            if (!pk->isSelfMessage()) { // ! in case this is a dummy packet
                                auto timeout = SimTime(pk->getTimer(), SIMTIME_NS);
                                auto deadline = simTime() + timeout;
                                seqDeadline[groupSeqKey] = deadline.inUnit(SIMTIME_NS);
                                if (!aggTimeOut->isScheduled() || deadline < aggTimeOut->getArrivalTime() ) {
                                    scheduleAfter(timeout, aggTimeOut);
                                }
                            }
                        }
                        pk = groupTable.at(group)->agg(pk);
                    }
                    emit(groupTable.at(group)->usedBufferSignal, groupTable.at(group)->getUsedBuffer());
                }
                else
                {
                    // * the first time we see the group, generate an entry for it
                    if (usedBuffer + pk->getByteLength() <= bufferSize)
                    {
//                        auto indegree = groupManager->getFanIndegree(group, 0, myAddress); // TODO: the treeIndex is fixed to 0
//                        if (indegree == -1) { // this switch doesn't deal with this group
//                            markNotAgg.insert(groupSeqKey);
//
//                        }
                         // TODO now every group use whole memory for now
                        groupTable[group] = new AggGroupEntry(bufferSize, indegree);
                        groupTable.at(group)->usedBufferSignal = createBufferSignalForGroup(group);
                        groupTable.at(group)->setAggPolicy(aggPolicy);
                        if (isTimerPolicy) {
                            // ! in case the last packet of aggregation not leaving
                            auto timeout = SimTime(pk->getTimer(), SIMTIME_NS);
                            auto deadline = simTime() + timeout;
                            seqDeadline[groupSeqKey] = deadline.inUnit(SIMTIME_NS);
                            if (!aggTimeOut->isScheduled() || deadline < aggTimeOut->getArrivalTime() ) {
                                scheduleAfter(timeout, aggTimeOut);
                            }
                            // ASSERT(seqDeadline.find(groupSeqKey) != seqDeadline.end());
                            // store the timeout value for each <group, addr> pair
                            seqDeadline[groupSeqKey] = simTime().inUnit(SIMTIME_NS) + pk->getTimer();
                        }
                        if (groupTable.at(group)->getLeftBuffer() > pk->getByteLength())
                        {// ! this check maybe not necessary, unless you set bufferSize < pk->getByteLength()
                            usedBuffer += pk->getByteLength();
                            pk = groupTable.at(group)->agg(pk); // ! this line must be put at last as pk is changed
                        }
                        else
                            markNotAgg.insert(groupSeqKey);
                    }
                    else
                    {
                        // ! not enough buffer to hold it , it must be sent out immediately,
                        // ! the following packets of the same <groupAddr, seq> cannot be aggregated either
                        markNotAgg.insert(groupSeqKey);
                        // ! do nothing to pk, pk will be handled like normal packet below
                    }
                }

                if (pk != nullptr) {
                    if (isTimerPolicy) {
                        auto releasedBuffer = groupTable[group]->release(pk);
                        usedBuffer -= releasedBuffer;
                        seqDeadline.erase(groupSeqKey);
                        EV_DEBUG <<"group " << group << " seq " << seq << " release buffer " << releasedBuffer << " bytes" << endl;
                    }
                }
                else
                    return;
            }
        }
        else if (pk->getKind() == ACK )
        {
            // markNotAgg.erase(groupSeqKey); // key not exist is ok
            if (groupTable.find(group) != groupTable.end())
            {
                auto outGateIndexes = getReversePortIndexes(groupSeqKey);
                incomingPortIndexes.erase(groupSeqKey);
                if (!isTimerPolicy) {
                    auto releasedBuffer = groupTable[group]->release(pk);
                    usedBuffer -= releasedBuffer;
                    seqDeadline.erase(std::make_pair(group, seq));
                    EV_DEBUG <<"group " << group << "seq " << seq << " release buffer " << releasedBuffer << " bytes" << endl;
                }
                broadcast(pk, outGateIndexes);
                return;
            }

            if (incomingPortIndexes.find(groupSeqKey) != incomingPortIndexes.end())
            {
                // ! this will happen when this switch doesn't deal with this group
                auto outGateIndexes = getReversePortIndexes(groupSeqKey);
                // ASSERT(outGateIndexes.size() == 1);
                incomingPortIndexes.erase(groupSeqKey);
                broadcast(pk, outGateIndexes);
                return;
            }
        }
    }

    // ! destAddr may be unicast addr or group data packet
    int outGateIndex = getRouteGateIndex(srcAddr, destAddr);
    if (outGateIndex == -1) { // ! if not found, routeManager will throw an error, the code is useless
        EV << "address " << destAddr << " unreachable, discarding packet " << pk->getName() << endl;
        emit(dropSignal, (intval_t)pk->getByteLength());
        delete pk;
        return;
    }
    EV << "Forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;
    send(pk, "out", outGateIndex);
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
