#include "Routing.h"
Define_Module(Routing);

void Routing::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        myAddress = getParentModule()->par("address"); // HACK
        ecmpFlow = par("ecmpFlow").boolValue();
        dropSignal = registerSignal("drop");
        outputIfSignal = registerSignal("outputIf");
        outputPacketSignal = registerSignal("outputPacket");
        isSwitch = (getParentModule()->getProperties()->get("switch") != nullptr);
        // WATCH_MAP(rtable); // ! this causes error if value is vector
        routeManager = getModuleFromPar<GlobalRouteManager>(par("routeManager"), this);
        if (strcmp(par("groupManager").stringValue(), "") != 0)
            groupManager = getModuleFromPar<GlobalGroupManager>(par("groupManager"), this);

        if (isSwitch) {
            bufferSize = par("bufferSize");
        }

    }
    if (stage == INITSTAGE_ASSIGN) {
        if (!isSwitch && groupManager != nullptr)
            myGroupAddress = groupManager->getGroupAddress(myAddress);
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
        if (isGroupAddr(destAddr)) {
            address = groupManager->getGroupRootAddress(destAddr);
        }
        auto outGateIndexes = routeManager->getRoutes(myAddress, address); // ! pass switchAddress not srcAddress
        rtable[destAddr] = outGateIndexes;
        return getRouteGateIndex(srcAddr, destAddr); // ! recursion find outgate index
    }
}

void Routing::broadcast(Packet *pk, const std::unordered_set<int>& outGateIndexes) {
    for (auto& gateIndex : outGateIndexes ) {
        auto packet = pk->dup();
        EV_INFO << "Forwarding broadcast packet " << pk->getName() << " on gate index " << gateIndex << endl;
        send(packet, "out", gateIndex);
    }
    delete pk;
}

std::unordered_set<int> Routing::getReversePortIndexes(Packet *pk) const
{
    auto group = pk->getDestAddr();
    auto seq = pk->getSeqNumber();
    if (groupTable.find(group)==groupTable.end())
        throw cRuntimeError("GroupPacketHandler::getReversePortIndexes: not find the group");
    return groupTable.at(group)->getIncomingPortIndexes(seq);
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

void Routing::handleMessage(cMessage *msg)
{
    Packet *pk = check_and_cast<Packet *>(msg);
    auto srcAddr = pk->getSrcAddr();
    auto destAddr = pk->getDestAddr();
    // ! If the packet destAddr is me(unicast) or my address is the group root
    // ! send the packet to upperLayer
    if ((destAddr == myAddress || destAddr == myGroupAddress)
            && strcmp(pk->getArrivalGate()->getName(),"in")==0)
    { // packet must come from below layer
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
        if (pk->getKind() == DATA) {
            if (markNotAgg.find(std::make_pair(group, seq)) == markNotAgg.end())
            {
                if (groupTable.find(group) != groupTable.end()) // already have an entry
                {
                    pk = groupTable.at(group)->agg(pk);
                }
                else
                {
                    // * the first time we see the group, generate an entry for it
                    if (usedBuffer + pk->getByteLength() <= bufferSize)
                    {
                        auto indegree = groupManager->getFanIndegree(group, 0, myAddress); // TODO: the treeIndex is fixed to 0
                         // TODO now every group use whole memory
                        groupTable[group] = new AggGroupEntry(bufferSize, indegree);
                        usedBuffer += pk->getByteLength();
                        // but a group may have its own restriction
                        pk = groupTable.at(group)->agg(pk); // ! this line must be put at last as pk is changed
                    }
                    else
                    {
                        // ! not enough buffer to hold it , it must be sent out immediately,
                        // ! the following packets of the same <groupAddr, seq> cannot be aggregated either
                        markNotAgg.insert(std::make_pair(group, seq));
                        // ! do nothing to pk, pk will be handled like normal packet below
                    }
                }
                if (pk == nullptr)
                    return;
            }
        }

        if (pk->getKind() == ACK )
        {   // key not exist is ok
            markNotAgg.erase(std::make_pair(group, seq));
            if (groupTable.find(group) != groupTable.end())
            {
                auto outGateIndexes = getReversePortIndexes(pk);
                groupTable[group]->release(pk);
                // TODO release occupied buffer
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
