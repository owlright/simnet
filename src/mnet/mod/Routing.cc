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
#include "../mod/Controller.h"
// #include "../mod/AggrGroupInfo.h"
using namespace omnetpp;

/**
 * Demonstrates static routing, utilizing the cTopology class.
 */
class Routing : public cSimpleModule
{
private:
    int myAddress;
    bool ecmpFlow = false;
    typedef std::map<int, std::vector<int>> RoutingTable;  // destaddr -> gateindex
    RoutingTable rtable;
    typedef std::map<int, std::vector<int> > AggrRoutingTable;
    AggrRoutingTable aggrChildren;
    // std::map<int, AggrGroupInfo*> aggrGroupTable;
    opp_component_ptr<Controller> controller;
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

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void refreshDisplay() const override;
};

Define_Module(Routing);

void Routing::initialize(int stage)
{

    if (stage == 0) {
        myAddress = getParentModule()->par("address"); // HACK
        ecmpFlow = par("ecmpFlow").boolValue();
        dropSignal = registerSignal("drop");
        outputIfSignal = registerSignal("outputIf");
        outputPacketSignal = registerSignal("outputPacket");
        // WATCH_MAP(rtable); // ! this causes error if value is vector
    }
    if (stage == 1) {
        controller = getModuleFromPar<Controller>(par("globalController"), this);
        ASSERT(controller != nullptr);
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
        auto outGateIndexes = controller->getRoutes(this->getParentModule(), destAddr);
        if (!outGateIndexes.empty()) {
            rtable[destAddr] = outGateIndexes;
            return getRouteGateIndex(srcAddr, destAddr);
        }
        else {
            return -1;
        }
    }
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
    int srcAddr = pk->getSrcAddr();
    int destAddr = pk->getDestAddr();
    // int groupAddr = pk->getGroupAddr();
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
                 // find entries and broadcast it
                 auto childrenAddresses = aggrGroup->getChildren();
                 for (auto& addr : childrenAddresses ) {
                     int outGateIndex = getRouteGateIndex(pk->getSrcAddr(), addr);
                     pk->setDestAddr(addr);

                     auto packet = pk->dup();
                     char pkname[40];
                     sprintf(pkname, "ACK-%d-to-%d-ack%u ", packet->getSrcAddr(), addr, packet->getAckSeq());
                     pk->setName(pkname);
                     EV << "Forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;
         //            emit(outputIfSignal, outGateIndex);
         //            emit(outputPacketSignal, packet);
                     send(packet, "out", outGateIndex);
                 }
                 delete pk;
             } else {
                 throw cRuntimeError("Unkonwn packet type!");
             }
             return; // ! do not forget to return here.
        }
    } */
    // ! Deal with unicast packet

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
    emit(outputIfSignal, outGateIndex);
    emit(outputPacketSignal, pk);
    send(pk, "out", outGateIndex);

}

void Routing::refreshDisplay() const
{
    char buf[20];
    sprintf(buf, "%d", myAddress);
    getParentModule()->getDisplayString().setTagArg("t", 0, buf);
}
