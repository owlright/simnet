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

    if (numSegments <= 1 && pk->getPacketType() != MACK) { // DATA, ACK
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
        // auto jobId = apk->getJobId();
        auto seq = apk->getAggSeqNumber();
        auto agtrIndex = apk->getAggregatorIndex();
        // ASSERT(outGateIndex != -1);
        // MulticastID mKey = {agtrIndex, outGateIndex};
        if (currSegment != myAddress && !pk->getResend()) { // ! store a special unicast entry
            auto key = AddrGate(destAddr, seq, outGateIndex);
            // ASSERT(groupUnicastTable.find(key) == groupUnicastTable.end());
            groupUnicastTable[key].insert(pk->getArrivalGate()->getIndex());
        }
        if (currSegment == myAddress) { // ! I'm responsible for aggregation
            if (!pk->getResend()) {
                if (aggregators.find(agtrIndex) == aggregators.end() || aggregators.at(agtrIndex)->checkAdmission(apk))
                {
                    if (aggregators.find(agtrIndex) == aggregators.end()) {
                        aggregators[agtrIndex] = new Aggregator(apk);
                        // if (groupMetricTable.find(jobId) == groupMetricTable.end()) {
                        //     // the first time we see this group
                        //     groupMetricTable[jobId] = new jobMetric(this, jobId);
                        //     groupMetricTable[jobId]->createBufferSignalForGroup(jobId);
                        // }
                        if (currSegment == myAddress) {
                            usedBuffer += pk->getByteLength();
                            emit(bufferInUseSignal, usedBuffer);
                            aggregators[agtrIndex]->usedBuffer = pk->getByteLength();
                        }
                    }
                    ASSERT(aggregators.find(agtrIndex) != aggregators.end());
                    auto agtr = aggregators.at(agtrIndex);
                    agtr->recordIncomingPorts(apk, outGateIndex);

                    pk = agtr->doAggregation(apk);
                    if (pk != nullptr) {
                        ASSERT(pk == apk);
                        agtr->fullAggregation = true; //useless flag, just for debugging
                        // aggregators.erase(agtrIndex);
                        // ASSERT(groupMetricTable.find(apk->getJobId()) != groupMetricTable.end());
                        // groupMetricTable.at(apk->getJobId())->releaseUsedBuffer(agtrSize);
                    }
                    else {
                        return;
                    }
                } else {
                    EV_DEBUG << "collision happen on destAddr " << destAddr << " seq " << seq << endl;
                    apk->setCollision(true);
                    apk->setResend(true);
                }
            }
            else {
                if (aggregators.find(agtrIndex) != aggregators.end()) {
                    auto agtr = aggregators.at(agtrIndex);

                    if ( agtr->checkAdmission(apk) ) {
                        // ! can be false if resend multiple times
                        // ! if aggregator[agtrIndex] belongs to me, which means the aggregator is stuck
                        // ! even if it's not stuck here(full aggregation here), you don't know if it's stuck downstream
                        aggregators.erase(agtrIndex);
                        usedBuffer -= apk->getByteLength();
                        emit(bufferInUseSignal, usedBuffer);
                    }
                }
            }
        }

        if (currSegment == myAddress && pk != nullptr) {
            // ! Even if it's a resend packet, do not forget to do this
            apk->setSegmentsLeft(pk->getSegmentsLeft() - 1);
        }
    }
    else if (pk->getPacketType() == MACK) { // TODO very strange, groupAddr is totally useless here
        auto apk = check_and_cast<AggPacket*>(pk);
        auto agtrIndex = apk->getAggregatorIndex();
        auto srcAddr = apk->getSrcAddr();
        auto seq = apk->getAggSeqNumber();
        auto key = AddrGate(srcAddr, seq, apk->getArrivalGate()->getIndex());
        bool foundEntry = false;
        if (aggregators.find(agtrIndex) != aggregators.end()) {
            auto agtr = aggregators.at(agtrIndex);
            if (agtr->checkAdmission(apk)) {
                ASSERT(groupUnicastTable.find(key) == groupUnicastTable.end());
                auto outGateIndexes = agtr->getOutGateIndexes(apk->getArrivalGate()->getIndex());
                // agtr->multicastCount++;
                broadcast(pk, outGateIndexes);
                // if (agtr->forAggregation) {
                    usedBuffer -= agtr->usedBuffer;
                    emit(bufferInUseSignal, usedBuffer);
                // }
                // if (agtr->multicastCount == agtr->getMulticastEntryNumber()) {
                    aggregators.erase(agtrIndex);
                // }
                foundEntry = true;
            }
        }
        if (!foundEntry) {
            if (groupUnicastTable.find(key) == groupUnicastTable.end()) {
                std::cout << simTime() << " " << pk->getName() << endl;
                EV_WARN << "multicast entry deleted too early." << endl;
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

    ASSERT(outGateIndex != -1);
    send(pk, "out", outGateIndex);
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
