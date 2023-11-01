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
            agtrSize = par("agtrSize");
            agtrCount = par("maxAgtrNum");
            bufferSize = agtrCount * agtrSize;
            if (bufferSize > 0) {
                useAgtrIndex = true;
            }
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

Aggregator* Routing::tryGetAgtr(const AggPacket* apk)
{
    if (useAgtrIndex) {
        auto agtrIndex = apk->getAggregatorIndex();
        if ( aggregators.find(agtrIndex) == aggregators.end() ) {
            if (apk->getPacketType() == MACK) { // ! only allocate for a agg data packet
                return nullptr;
            }
            aggregators[agtrIndex] = new Aggregator(apk);
            // if (groupMetricTable.find(jobId) == groupMetricTable.end()) {
            //     // the first time we see this group
            //     groupMetricTable[jobId] = new jobMetric(this, jobId);
            //     groupMetricTable[jobId]->createBufferSignalForGroup(jobId);
            // }
            usedBuffer += agtrSize;
            aggregators[agtrIndex]->usedBuffer = agtrSize;
            emit(bufferInUseSignal, usedBuffer);
            return aggregators.at(agtrIndex);
        }
        else if ( aggregators.at(agtrIndex)->checkAdmission(apk) ) {
            return aggregators.at(agtrIndex);
        }
        else {
            return nullptr;
        }
    }
    else {
        int jobId = apk->getJobId();
        auto seq = apk->getAggSeqNumber();
        auto psAddr = (apk->getPacketType()==MACK) ? apk->getSrcAddr() : apk->getDestAddr();
        AgtrID key{psAddr, seq, jobId};
        if ( agtrIndexes.find(key) == agtrIndexes.end() ) {
            if (apk->getPacketType() == MACK)
                return nullptr;
            std::size_t agtrIndex = 0;
            if (!aggregators.empty())
                agtrIndex = aggregators.rbegin()->first + 1;
            agtrIndexes[key] = agtrIndex;
            aggregators[agtrIndex] = new Aggregator(apk);
            usedBuffer += agtrSize;
            aggregators[agtrIndex]->usedBuffer = agtrSize;
            emit(bufferInUseSignal, usedBuffer);
            return aggregators.at(agtrIndex);
        }
        else {

            auto agtrIndex = agtrIndexes.at(key);
            if ( !aggregators.at(agtrIndex)->checkAdmission(apk) ) {
                throw cRuntimeError("(get) In useAgtrIndex disabled mode, there shouldn't be collision.");
            }
            return aggregators.at(agtrIndex);
        }
    }
}

void Routing::tryReleaseAgtr(const AggPacket* apk)
{
    if (!apk->getResend() && !(apk->getPacketType() == MACK)) {
        throw cRuntimeError("release agtr should only triggered by resend and mack packets.");
    }
    if (useAgtrIndex) {
        auto agtrIndex = apk->getAggregatorIndex();
        if (aggregators.find(agtrIndex) != aggregators.end()) {
            auto agtr = aggregators.at(agtrIndex);

            if ( agtr->checkAdmission(apk) ) {
                // ! can be false if resend multiple times
                // ! if aggregator[agtrIndex] belongs to me, which means the aggregator is stuck
                // ! even if it's not stuck here(full aggregation here), you don't know if it's stuck downstream
                usedBuffer -= agtrSize;
                delete aggregators.at(agtrIndex);
                aggregators.erase(agtrIndex);
                emit(bufferInUseSignal, usedBuffer);
            }
        }
    }
    else {
        int jobId = apk->getJobId();
        auto seq = apk->getAggSeqNumber();
        auto psAddr = (apk->getPacketType()==MACK) ? apk->getSrcAddr() : apk->getDestAddr();
        AgtrID key{psAddr, seq, jobId};
        ASSERT(agtrIndexes.find(key)!=agtrIndexes.end());
        auto agtrIndex = agtrIndexes.at(key);
        if (aggregators.find(agtrIndex) != aggregators.end()) {
            auto agtr = aggregators.at(agtrIndex);
            if ( !agtr->checkAdmission(apk) ) {
                throw cRuntimeError("(release) In useAgtrIndex disabled mode, there shouldn't be collision.");
            }
            usedBuffer -= agtrSize;
            agtrIndexes.erase(key);
            delete aggregators.at(agtrIndex);
            aggregators.erase(agtrIndex);
            emit(bufferInUseSignal, usedBuffer);
        }
    }

}

int Routing::getRouteGateIndex(int srcAddr, int destAddr) {
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

void Routing::broadcast(Packet* pk, const std::unordered_set<int>& outGateIndexes)
{
    for (auto& gateIndex : outGateIndexes ) {
        auto packet = pk->dup();
        EV_INFO << "Forwarding broadcast packet " << pk->getName() << " on gate index " << gateIndex << endl;
        send(packet, "out", gateIndex);
    }
    delete pk;
}

// std::vector<int> Routing::getReversePortIndexes(const MulticastID& mKey) const
// {
//     if (incomingPortIndexes.find(mKey) == incomingPortIndexes.end())
//         throw cRuntimeError("%" PRId64" Routing::getReversePortIndexes: agtrIndex: %zu, inGate: %d not found!",
//                                     myAddress, mKey.agtrIndex, mKey.inGateIndex);
//     return incomingPortIndexes.at(mKey);
// }

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
    if (pk->getPacketType() == MACK) {
        // * Step 1. broadcast this packet
        auto apk = check_and_cast<AggPacket*>(pk);

        // auto agtrIndex = apk->getAggregatorIndex();
        auto srcAddr = apk->getSrcAddr();
        auto seq = apk->getAggSeqNumber();
        auto key = AddrGate(srcAddr, seq, apk->getArrivalGate()->getIndex());
        bool foundEntry = false;

        auto agtr = tryGetAgtr(apk);
        if (agtr != nullptr) {
            ASSERT(groupUnicastTable.find(key) == groupUnicastTable.end());
            auto outGateIndexes = agtr->getOutGateIndexes();
            tryReleaseAgtr(apk);
            broadcast(apk, outGateIndexes);
            foundEntry = true;
        }

        if (!foundEntry) {
            if (groupUnicastTable.find(key) == groupUnicastTable.end()) {
                EV_ERROR << RED << simTime() << " " << pk->getName() << ENDC;
                EV_ERROR << RED <<"multicast entry deleted too early." << ENDC;
                delete pk;
                throw cRuntimeError("The multicast entry doesn't exist, it must be deleted by a resend packet. Check allowed resend interval.");
            } else {
                foundEntry = true;
                broadcast(pk, groupUnicastTable.at(key));
                groupUnicastTable.erase(key);
            }
        }
        ASSERT(foundEntry);


        // MulticastID mKey = {agtrIndex, apk->getArrivalGate()->getIndex())};
        // if (incomingPortIndexes.find(mKey) != incomingPortIndexes.end()) {
        //     auto outGateIndexes = incomingPortIndexes.at(mKey);
        //     incomingPortIndexes.erase(mKey);
        //     broadcast(pk, outGateIndexes);
        // }
        // else {
        //     delete pk;
        //     EV_WARN << "The multicast entry doesn't exist, it must be deleted by a resend packet." << endl;
        //     if (!getEnvir()->isExpressMode()) {
        //         getParentModule()->bubble("miss multicast entry!");
        //     }
        // }

        return;
    }
    // * Step 2. Unicast
    if (numSegments <= 1) {
        // unicast DATA, ACK etc. packets
        auto srcAddr = pk->getSrcAddr();
        outGateIndex = getRouteGateIndex(srcAddr, destAddr);
    }
    else {
        // * Step 3. AggPacket
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
        // auto jobId = apk->getJobId();
        auto seq = apk->getAggSeqNumber();

        // ASSERT(outGateIndex != -1);
        // MulticastID mKey = {agtrIndex, outGateIndex};
        // ! must store a special unicast entry for reverse ack, unless this is a resent packet, because PS will send each worker an ack instead of MACK
        if (currSegment != myAddress && !apk->getResend()) {
            auto key = AddrGate(destAddr, seq, outGateIndex);
            // ASSERT(groupUnicastTable.find(key) == groupUnicastTable.end());
            groupUnicastTable[key].insert(apk->getArrivalGate()->getIndex());
        }

        if (currSegment == myAddress) { // ! I'm responsible for aggregation
            processAggPacket(apk);
            if (apk == nullptr)
                return;
        }

        if (currSegment == myAddress && apk != nullptr) {
            // ! Even if it's a resend packet, do not forget to do this
            apk->setSegmentsLeft(apk->getSegmentsLeft() - 1);
        }
    }

    ASSERT(outGateIndex != -1);
    send(pk, "out", outGateIndex);
}

void Routing::processAggPacket(AggPacket*& apk)
{
    if (!apk->getResend()) {
        auto agtr = tryGetAgtr(apk);
        if (agtr != nullptr)
        {
            agtr->recordIncomingPorts(apk);
            apk = agtr->doAggregation(apk);
            if (apk != nullptr) {
                // aggregators.erase(agtrIndex);
                // ASSERT(groupMetricTable.find(apk->getJobId()) != groupMetricTable.end());
                // groupMetricTable.at(apk->getJobId())->releaseUsedBuffer(agtrSize);
            }

        } else {
            EV_DEBUG << "collision happen on destAddr " << apk->getDestAddr() << " seq " << apk->getAggSeqNumber() << endl;
            apk->setCollision(true);
            apk->setResend(true);
        }
    }
    else {
        if (!useAgtrIndex) {
            std::cout << RED << apk << ENDC;
            throw cRuntimeError("there shouldn't be any resend packets when useAgtrIndex disabled.");
        }

        tryReleaseAgtr(apk);
    }
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
            << " belongs to job " << p->getJobId() << " round " << p->getRound() << " seq " << p->getSeqNumber() << endl;
        }
    }
}

void Routing::handleParameterChange(const char *parameterName)
{
    if (strcmp(parameterName, "address") == 0) {
        myAddress = par("address");
    }
}
