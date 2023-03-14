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
#include "Packet_m.h"
#include "../common/Defs.h"
#include "../common/Print.h"
#include "../common/ModuleAccess.h"
#include "Controller.h"
#include "AggrGroupInfo.h"
using namespace omnetpp;

/**
 * Demonstrates static routing, utilizing the cTopology class.
 */
class Routing : public cSimpleModule
{
private:
    int myAddress;
    typedef std::map<int, int> RoutingTable;  // destaddr -> gateindex
    RoutingTable rtable;
    typedef std::map<int, std::vector<int> > AggrRoutingTable;
    AggrRoutingTable aggrChildren;
    std::map<int, AggrGroupInfo*> aggrGroupTable;
    opp_component_ptr<Controller> controller;
    simsignal_t dropSignal;
    simsignal_t outputIfSignal;
    simsignal_t outputPacketSignal;
    // std::map<int, Packet*> aggrPacket;
    // std::map<int, int> aggrCounter;
    // std::map<int, int> aggrNumber;

private:
    bool isAggrGroupAdded(int address) const;
    bool isAggrGroup(int address) const;
    AggrGroupInfo* getAggrGroup(int address) const;
    AggrGroupInfo* getOrAddGroup(int address);
    int getRouteGateIndex(int address);

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
        myAddress = getParentModule()->par("address");
        dropSignal = registerSignal("drop");
        outputIfSignal = registerSignal("outputIf");
        outputPacketSignal = registerSignal("outputPacket");
        WATCH_MAP(rtable);
    }
    if (stage == 1) {
        controller = getModuleFromPar<Controller>(par("globalController"), this);
        assert(controller != nullptr);
    }
}

int Routing::getRouteGateIndex(int address)
{
    RoutingTable::iterator it = rtable.find(address);
    if (it != rtable.end()) {
        int outGateIndex = (*it).second;
        return outGateIndex;
    }
    else {
        int outGateIndex = controller->getRoute(this->getParentModule(), address);
        if (outGateIndex != -1) {
            rtable[address] = outGateIndex;
        }
        return outGateIndex;
    }
}

bool Routing::isAggrGroupAdded(int address) const
{
    return aggrGroupTable.find(address)!=aggrGroupTable.end();
}

bool Routing::isAggrGroup(int address) const
{
    return controller->isAggrGroupOnRouter(address, myAddress);
}

AggrGroupInfo* Routing::getOrAddGroup(int address) {
    if (!isAggrGroupAdded(address)) {// ! the first packet of the first round
        auto numberOfChildren = controller->getGroupAggrNum(address, myAddress);
        auto bufferSize = controller->getGroupAggrBuffer(address, myAddress);
        aggrGroupTable[address] = new AggrGroupInfo(address, numberOfChildren, bufferSize);
        EV << "Aggregating group " << address << " with " << numberOfChildren << " children." << endl;

    }

   return getAggrGroup(address);
}

AggrGroupInfo* Routing::getAggrGroup(int address) const {
    return aggrGroupTable.at(address);
}

void Routing::handleMessage(cMessage *msg)
{
    Packet *pk = check_and_cast<Packet *>(msg);
    int destAddr = pk->getDestAddr();
    int groupAddr = pk->getGroupAddr();

    // ! Deal with unicast packet
    if (!isAggrGroup(groupAddr)) {
        if (destAddr == myAddress) {
            int outGateIndex = getRouteGateIndex(pk->getSrcAddr());
            EV << "local delivery of packet " << pk->getName() << endl;
            pk->addPar("outGateIndex"); // todo its very bad to add par here.
            pk->par("outGateIndex") = outGateIndex;
            send(pk, "localOut");
            emit(outputIfSignal, -1);  // -1: local
            return;
        }
        int outGateIndex = getRouteGateIndex(destAddr);
        if (outGateIndex == -1) {
            EV << "address " << destAddr << " unreachable, discarding packet " << pk->getName() << endl;
            emit(dropSignal, (intval_t)pk->getByteLength());
            delete pk;
            return;
        }
        EV << "Forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;
        pk->setHopCount(pk->getHopCount()+1);
        emit(outputIfSignal, outGateIndex);
        emit(outputPacketSignal, pk);
        send(pk, "out", outGateIndex);
        return; // ! do not forget to return here
    }


    // ! Deal with aggr packet here
    auto aggrGroup = getOrAddGroup(groupAddr);

    if (pk->getKind()==PacketType::DATA) {
        auto seq = pk->getSeq();
        if (!aggrGroup->isChildrenFull()) {
            aggrGroup->insertChildNode(pk->getSrcAddr());
        }
        if (aggrGroup->isRecordedAggr(seq)) {
            auto aggpacket = aggrGroup->aggrPacket(seq, pk);
            // * when a round finish, then we have the aggr packet otherwise just update info
            if (aggpacket != nullptr) { // ! all packets are aggregated
                int outGateIndex = getRouteGateIndex(destAddr);
                aggpacket->setSrcAddr(myAddress);
                aggpacket->setHopCount(aggpacket->getHopCount()+1); // todo how to count hop
                emit(outputIfSignal, outGateIndex);
                emit(outputPacketSignal, aggpacket);
                send(aggpacket, "out", outGateIndex);
            }
        }
        else  if (aggrGroup->isGroupHasBuffer() && !aggrGroup->isRecordedNotAggr(seq) ) {
            // the first time we see the seq and there is space to store
            aggrGroup->aggrPacket(seq, pk); //just update info
        }
        else if (aggrGroup->isRecordedNotAggr(seq) || !aggrGroup->isGroupHasBuffer()){
            // nospace or the seq is set to noaggr, just send it out
            aggrGroup->recordNotAggr(seq);
            int outGateIndex = getRouteGateIndex(destAddr);
            emit(outputIfSignal, outGateIndex);
            emit(outputPacketSignal, pk);
            send(pk, "out", outGateIndex); // do the same as a unicast packet
        }

    } else if (pk->getKind()==PacketType::ACK) {
        auto seq = pk->getAckSeq();
        aggrGroup->reset(seq); // release (key,value) for seq
        EV << "ACK to group " << groupAddr << " arrive." << endl;
        // find entries and broadcast it
        auto childrenAddresses = aggrGroup->getChildren();
        for (auto& addr : childrenAddresses ) {
            int outGateIndex = getRouteGateIndex(addr);
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


}

void Routing::refreshDisplay() const
{
    char buf[20];
    sprintf(buf, "%d", myAddress);
    getParentModule()->getDisplayString().setTagArg("t", 0, buf);
}
