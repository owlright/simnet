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
        groupManager = getModuleFromPar<GlobalGroupManager>(par("groupManager"), this);
        ASSERT(routeManager != nullptr);
        ASSERT(groupManager != nullptr);

        if (isSwitch) {
            aggPacketHandler.bufferSize = par("bufferSize");
            aggPacketHandler.groupManager = groupManager;
            aggPacketHandler.switchAddress = myAddress;
        }

    }
    if (stage == INITSTAGE_ASSIGN) {
        if (!isSwitch)
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
    // save the time to ask route manager
        EV << "Forwarding packet " << pk->getName() << " on gate index " << 0 << endl;
        send(pk, "out", 0);
        return;
    }

    // ! If I'm a switch, deal with group address
    if (isSwitch && isGroupAddr(destAddr) ) {
        if (pk->getKind() == DATA) {
            auto aggpk = aggPacketHandler.agg(pk); // group addr and packet seq will be handled here
            if (aggpk == nullptr) { // packet aggregation is finished
                return;
            }
            pk = aggpk;
        }

        if (pk->getKind() == ACK) {
            auto outGateIndexes = aggPacketHandler.getReversePortIndexes(pk);
            aggPacketHandler.releaseGroupOnSeq(pk->getDestAddr(), pk->getSeqNumber()); // release store memory
            broadcast(pk, outGateIndexes);
            return;
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
