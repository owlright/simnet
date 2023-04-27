//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <map>
#include <omnetpp.h>
#include "../mod/Packet_m.h"
#include "../common/Defs.h"
#include "../common/Print.h"
#include "../common/ModuleAccess.h"
#include "simnet/mod/GlobalRouteMangager.h"
#include "simnet/mod/agroup/GroupPacketHandler.h"
// #include "../mod/AggrGroupInfo.h"
using namespace omnetpp;

/**
 * Demonstrates static routing, utilizing the cTopology class.
 */
class Routing : public cSimpleModule
{
private:
    IntAddress myAddress;
    bool ecmpFlow = false;
    typedef std::map<int, std::vector<int>> RoutingTable;  // destaddr -> gateindex
    RoutingTable rtable;
    typedef std::map<int, std::vector<int> > AggrRoutingTable;
    AggrRoutingTable aggrChildren;
    // std::map<int, AggrGroupInfo*> aggrGroupTable;
    GlobalRouteManager* routeManager{nullptr};
    GroupPacketHandler* gpkHandler{nullptr};
    simsignal_t dropSignal;
    simsignal_t outputIfSignal;
    simsignal_t outputPacketSignal;
    // std::map<int, Packet*> aggrPacket;
    // std::map<int, int> aggrCounter;
    // std::map<int, int> aggrNumber;

private:
    // bool isAggrGroupAdded(int address) const;
    // bool isAggrGroup(int address) const;
    // AggrGroupInfo* getAggrGroup(int address) const;
    // AggrGroupInfo* getOrAddGroup(int address);
    int getRouteGateIndex(int srcAddr, int destAddr);
    bool isGroupAddr(IntAddress addr) const {
        return (GROUPADDR_START <= addr && addr < GROUPADDR_END);
    };
    bool isUnicastAddr(IntAddress addr) const {
        return !isGroupAddr(addr);
    };
    void broadcast(Packet* pk, const std::vector<int>& outGateIndexes);

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void refreshDisplay() const override;
};

Define_Module(Routing);

void Routing::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        myAddress = getParentModule()->par("address"); // HACK
        ecmpFlow = par("ecmpFlow").boolValue();
        dropSignal = registerSignal("drop");
        outputIfSignal = registerSignal("outputIf");
        outputPacketSignal = registerSignal("outputPacket");
        // WATCH_MAP(rtable); // ! this causes error if value is vector
        routeManager = getModuleFromPar<GlobalRouteManager>(par("globalRouteManager"), this);
        ASSERT(routeManager != nullptr);
        auto handler = getSubmodule("pkh");
        if (handler!=nullptr) // handler is allowed to be nullptr
            gpkHandler = check_and_cast<GroupPacketHandler*>(handler);
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
        auto outGateIndexes = routeManager->getRoutes(myAddress, destAddr);
        rtable[destAddr] = outGateIndexes;
        return getRouteGateIndex(srcAddr, destAddr); // ! recursion find outgate index
    }
}

void Routing::broadcast(Packet *pk, const std::vector<int>& outGateIndexes) {
    for (auto& gateIndex : outGateIndexes ) {
        auto packet = pk->dup();
        char pkname[40];
        sprintf(pkname, "ACK-%" PRId64 "-to-group-%" PRId64 "-seq%" PRId64,
                packet->getSrcAddr(), packet->getDestAddr(), packet->getSeqNumber());
        packet->setName(pkname);
        EV_INFO << "Forwarding broadcast packet " << pk->getName() << " on gate index " << gateIndex << endl;
        send(packet, "out", gateIndex);
    }
    delete pk;
}
// bool Routing::isAggrGroupAdded(int address) const
// {
//     return aggrGroupTable.find(address)!=aggrGroupTable.end();
// }

// bool Routing::isAggrGroup(int address) const
// {
//     return controller->isAggrGroupOnRouter(address, myAddress);
// }

// AggrGroupInfo* Routing::getOrAddGroup(int address) {
//     if (!isAggrGroupAdded(address)) {// ! the first packet of the first round
//         auto numberOfChildren = controller->getGroupAggrNum(address, myAddress);
//         auto bufferSize = controller->getGroupAggrBuffer(address, myAddress);
//         aggrGroupTable[address] = new AggrGroupInfo(address, numberOfChildren, bufferSize);
//         EV << "Aggregating group " << address << " Buffer size: " << bufferSize << " with " << numberOfChildren << " children." << endl;

//     }

//    return getAggrGroup(address);
// }

// AggrGroupInfo* Routing::getAggrGroup(int address) const {
//     return aggrGroupTable.at(address);
// }

void Routing::handleMessage(cMessage *msg)
{
    Packet *pk = check_and_cast<Packet *>(msg);
    auto srcAddr = pk->getSrcAddr();
    auto destAddr = pk->getDestAddr();
    if (isGroupAddr(destAddr)) {
        if (gpkHandler == nullptr) {
            throw cRuntimeError("Routing::handleMessage: you must have a group packet handler to deal with group packet!");
        }

        if (pk->getKind() == DATA) {
            auto aggpk = gpkHandler->agg(pk); // group addr and packet seq will be handled here
            if (aggpk != nullptr) { // packet aggregation is finished

            }
            return;
        }

        if (pk->getKind() == ACK) {
            auto outGateIndexes = gpkHandler->getReversePortIndexes(pk);
            broadcast(pk, outGateIndexes);
            gpkHandler->releaseGroupOnSeq(pk->getDestAddr(), pk->getSeqNumber()); // release store memory
            return;
        }
    }
    /*
    if (getParentModule()->getProperties()->get("switch") != nullptr) { // I'm the router
        if (isAggrGroup(groupAddr)) {
            // ! Deal with aggr packet here
             auto aggrGroup = getOrAddGroup(groupAddr);
             if (pk->getKind()==PacketType::DATA) {
                 auto seq = pk->getSeq();
                 if (!aggrGroup->isChildrenFull()) { // TODO make the code better
                     // record the children of current aggregation node
                     aggrGroup->insertChildNode(pk->getSrcAddr());
                 }
                 if (aggrGroup->isRecordedAggr(seq) || // recorded already
                         (aggrGroup->isGroupHasBuffer() && !aggrGroup->isRecordedNotAggr(seq))) // the first time we see the seq and there is space to store
                 {
                     auto aggpacket = aggrGroup->aggrPacket(seq, pk);
                     // * when a round finish, then we have the aggr packet otherwise just update info
                     if (aggpacket != nullptr) { // ! all packets are aggregated
                         int outGateIndex = getRouteGateIndex(aggpacket->getSrcAddr(), destAddr);
                         aggpacket->setSrcAddr(myAddress);
                         if (aggrGroup->getChildrenNum() > aggpacket->getAggNum()) {
                             aggpacket->setAggNum(aggrGroup->getChildrenNum());
                         }
                         if (aggrGroup->getBufferSize() < aggpacket->getAggWin()) {
                             aggpacket->setAggWin(aggrGroup->getBufferSize());
                         }
         //                aggpacket->setHopCount(aggpacket->getHopCount()+1); // TODO how to count hop
                         emit(outputIfSignal, outGateIndex);
                         emit(outputPacketSignal, aggpacket);
                         send(aggpacket, "out", outGateIndex);
                     }
                 }
//                 else  if (aggrGroup->isGroupHasBuffer() && !aggrGroup->isRecordedNotAggr(seq) ) {
//
//                     aggrGroup->aggrPacket(seq, pk); //just update info
//                 }
                 else if (aggrGroup->isRecordedNotAggr(seq) || !aggrGroup->isGroupHasBuffer()){
                     // nospace or the seq is set to noaggr, just send it out
                     aggrGroup->recordNotAggr(seq);
                     int outGateIndex = getRouteGateIndex(pk->getSrcAddr(), destAddr);
                     emit(outputIfSignal, outGateIndex);
                     emit(outputPacketSignal, pk);
                     send(pk, "out", outGateIndex); // do the same as a unicast packet
                 }
                 else {
                     throw cRuntimeError("What do I miss?");
                 }

             } else if (pk->getKind()==PacketType::ACK) {
                 auto seq = pk->getAckSeq();
                 aggrGroup->reset(seq); // release (key,value) for seq
                 EV << "ACK to group " << groupAddr << " arrive." << endl;


             return; // ! do not forget to return here.
        }
    } */
    // ! Deal with unicast packet
    if (isUnicastAddr(destAddr)) {
        if (destAddr == myAddress) {
            EV << "local delivery of packet " << pk->getName() << endl;
            send(pk, "localOut");
            emit(outputIfSignal, -1);  // -1: local
            return;
        }
        int outGateIndex = getRouteGateIndex(srcAddr, destAddr);
        if (outGateIndex == -1) {
            EV << "address " << destAddr << " unreachable, discarding packet " << pk->getName() << endl;
            emit(dropSignal, (intval_t)pk->getByteLength());
            delete pk;
            return;
        }
        EV << "Forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;
        // pk->setHopCount(pk->getHopCount()+1);
        // emit(outputIfSignal, outGateIndex);
        // emit(outputPacketSignal, pk);
        send(pk, "out", outGateIndex);
    }
    // ! code can't run till here
    throw cRuntimeError("Unknown packet dest address %" PRId64, destAddr);
}

void Routing::refreshDisplay() const
{
    if (!getEnvir()->isExpressMode()) {
        char buf[20];
        sprintf(buf, "%" PRId64, myAddress);
        getParentModule()->getDisplayString().setTagArg("t", 0, buf);
    }
}
