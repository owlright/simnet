#include "Routing.h"
Define_Module(Routing);

simsignal_t Routing::bufferInUseSignal = registerSignal("bufferInUse");

void Routing::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        myAddress = par("address");
        ecmpFlow = par("ecmpFlow").boolValue();
        isSwitch = (getParentModule()->getProperties()->get("switch") != nullptr);
        myAddress = getParentModule()->par("address");
        routeManager = findModuleFromTopLevel<GlobalRouteManager>("routeManager", this);
        if (!routeManager) // ! this module is necessary
            throw cRuntimeError("no routeManager!");

        if (isSwitch) {
            bufferSize = par("bufferSize");
            numAggregators = getParentModule()->par("numAggregators");
            agtrSize = getParentModule()->par("agtrSize");
            collectionPeriod = par("collectPeriod").doubleValueInUnit("s");
            dataCollectTimer = new cMessage("dataCollector");
        }

    }
}

Routing::~Routing()
{
    if (isSwitch) {
        // cancelAndDelete(aggTimeOut);
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
        throw cRuntimeError("%" PRId64" Routing::getReversePortIndexes: agtrIndex: %zu, inGate: %d not found!",
                                    myAddress, mKey.agtrIndex, mKey.inGateIndex);
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
    auto currSegment = destAddr;
    auto numSegments = pk->getSegmentsLeft();
    int outGateIndex = -1;

    if (numSegments <= 1 && pk->getPacketType() != MACK) { // DATA, ACK, MACK
        auto srcAddr = pk->getSrcAddr();
        outGateIndex = getRouteGateIndex(srcAddr, destAddr);
    }

    if ( numSegments > 1 ) {
        ASSERT(pk->getPacketType() == AGG); // only INC uses segments now
        auto nextSegment = destAddr;
        int segmentIndex = numSegments - 1;
        currSegment = pk->getSegments(segmentIndex);
        ASSERT(segmentIndex != 0);
        nextSegment = pk->getSegments(segmentIndex - 1);
        // ! aggPacket's srcAddr may change when resend or collision happen
        // ! we must make sure in any case the ecmp give the same outGate index
        auto srcAddr = myAddress + destAddr;
        // ! get the output gate index
        if (currSegment == myAddress) {
            outGateIndex = getRouteGateIndex(srcAddr, nextSegment); // ! next segment
        }
        else {
            outGateIndex = getRouteGateIndex(srcAddr, currSegment); // ! current segment
        }
    }


    if (pk->getPacketType() == AGG) {
        auto apk = check_and_cast<AggPacket*>(pk);
        auto jobId = apk->getJobId();
        auto seq = apk->getAggSeqNumber();
        auto agtrIndex = apk->getAggregatorIndex();
        ASSERT(outGateIndex != -1);
        MulticastID mKey = {agtrIndex, outGateIndex};
        recordIncomingPorts(mKey, pk->getArrivalGate()->getIndex());
        auto it = aggregators.find(agtrIndex);
        if (pk->getResend()) {
            if (it != aggregators.end()) {
                auto agtr = it->second;
                if (agtr->checkAdmission(apk)) {
                    aggregators.erase(agtrIndex);
                }
            }
            incomingPortIndexes.erase(mKey);
        }
        else if (currSegment == myAddress) {
            if (it == aggregators.end() || it->second->checkAdmission(apk)) {
                if (aggregators.find(agtrIndex) == aggregators.end())
                    aggregators[agtrIndex] = new Aggregator();
                if (groupMetricTable.find(jobId) == groupMetricTable.end()) {
                    // the first time we see this group
                    groupMetricTable[jobId] = new jobMetric(this, jobId);
                    groupMetricTable[jobId]->createBufferSignalForGroup(jobId);
                }
                ASSERT(aggregators.find(agtrIndex) != aggregators.end());
                auto agtr = aggregators.at(agtrIndex);
                pk = agtr->doAggregation(apk);
                if (pk != nullptr) {
                    ASSERT(pk == apk);
                    aggregators.erase(agtrIndex);
                    ASSERT(groupMetricTable.find(apk->getJobId()) != groupMetricTable.end());
                    groupMetricTable.at(apk->getJobId())->releaseUsedBuffer(agtrSize);
                }
                else {
                    return;
                }
            }
            else {
                EV_DEBUG << "collision happen on destAddr " << destAddr << " seq " << seq << endl;
                apk->setCollision(true);
                apk->setResend(true);
            }
        }
        if (currSegment == myAddress && pk != nullptr)
            apk->setSegmentsLeft(pk->getSegmentsLeft() - 1);
    }
    else if (pk->getPacketType() == MACK) { // TODO very strange, groupAddr is totally useless here
        auto apk = check_and_cast<AggPacket*>(pk);
        auto agtrIndex = apk->getAggregatorIndex();
        MulticastID mKey = {agtrIndex, apk->getArrivalGate()->getIndex()};
        if (incomingPortIndexes.find(mKey) != incomingPortIndexes.end()) {
            auto outGateIndexes = incomingPortIndexes.at(mKey);
            incomingPortIndexes.erase(mKey);
            broadcast(pk, outGateIndexes);
        }
        else {
            delete pk;
            EV_WARN << "The multicast entry doesn't exist, it must be deleted by a resend packet." << endl;
            if (!getEnvir()->isExpressMode()) {
                getParentModule()->bubble("miss multicast entry!");
            }
        }

        return;
    }

    ASSERT(outGateIndex != -1);
    send(pk, "out", outGateIndex);
}

int Routing::getForwardGateIndex(const Packet* pk, IntAddress nextAddr)
{
    // route this packet which may be:
    // 1. unicast packet
    // 2. agg packet but not ask for aggregation(segment!= myAddress) or it's a resend
    // 3. finished aggregated packet
    // 4. agg packet failed to be aggregated(hash collision), which is also a resend packet
    int outGateIndex = -1;
    auto srcAddr = pk->getSrcAddr();
    auto destAddr = pk->getDestAddr();
    if (nextAddr != -1 && pk->getPacketType() == AGG) {
        // ! aggPacket's srcAddr may change when resend or collision happen
        // ! we must make sure in any case the ecmp give the same outGate index
        srcAddr = myAddress + destAddr;
        // ! route to next router, otherwise ecmp may break this
        destAddr = nextAddr;
    }
    // 1. unicast packet
    // 2. agg packet but not ask for aggregation(segment!= myAddress) or it's a resend
    outGateIndex = getRouteGateIndex(srcAddr, destAddr);
    EV << "Forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;
    return outGateIndex;
}

Packet* Routing::aggregate(AggPacket *apk)
{
    auto agtrIndex = apk->getAggregatorIndex();
    if (aggregators.at(agtrIndex) == nullptr) { // lazy initialization to save memory
        ASSERT(groupMetricTable.find(apk->getJobId()) != groupMetricTable.end());
        if (!apk->getResend())
            aggregators[agtrIndex] = new Aggregator();
        else  // ! a resend packet mustn't get an idle aggregator
            return apk;
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
    // if (isSwitch) {
    //     char buf[30];
    //     sprintf(buf, "switch-%" PRId64 "-compEff", myAddress);
    //     // I dont want the time's unit too big, otherwise the efficiency will be too big
    //     recordScalar(buf, getComputationCount() / double(getUsedTime().inUnit(SIMTIME_US))); // TODO will resource * usedTime better?
    // }
    for (auto& [index, p] : aggregators) {
        if (p!=nullptr) {
            EV_WARN << "there is unreleased aggregator on router " << myAddress
            << " belongs to job " << p->getJobId() << " seq " << p->getSeqNumber() << endl;
        }
    }
}

void Routing::handleParameterChange(const char *parameterName)
{
    if (strcmp(parameterName, "address") == 0) {
        myAddress = par("address");
    }
}
