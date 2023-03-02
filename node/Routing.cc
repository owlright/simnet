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
using namespace omnetpp;

/**
 * Demonstrates static routing, utilizing the cTopology class.
 */
class Routing : public cSimpleModule
{
private:
    int myAddress;
    bool isSwitch;
    typedef std::map<int, int> RoutingTable;  // destaddr -> gateindex
    RoutingTable rtable;
    typedef std::map<int, std::vector<int> > AggrRoutingTable;
    AggrRoutingTable aggrtable;
    opp_component_ptr<Controller> controller;
    simsignal_t dropSignal;
    simsignal_t outputIfSignal;
    simsignal_t outputPacketSignal;
    std::map<int, Packet*> aggrPacket;
    std::map<int, int> aggrCounter;

private:
    int getRouteGateIndex(int address);
    int getAggrNum(int destAddress);

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
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

int Routing::getAggrNum(int destAddress)
{
    return controller->getGroupAggrNum(destAddress, this->getParentModule()->getIndex());
}

void Routing::handleMessage(cMessage *msg)
{
    Packet *pk = check_and_cast<Packet *>(msg);
    int destAddr = pk->getDestAddr();
    int groupAddr = pk->getGroupAddr();

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
    // RoutingTable::iterator it = rtable.find(destAddr);
    if (outGateIndex == -1) {
        EV << "address " << destAddr << " unreachable, discarding packet " << pk->getName() << endl;
        emit(dropSignal, (intval_t)pk->getByteLength());
        delete pk;
        return;
    }

    // int outGateIndex = (*it).second;


    if (getAggrNum(groupAddr) != -1) { // ! Deal with Aggregation here
        if (pk->getKind()==PacketType::DATA) {
            EV << "Aggregating group " << groupAddr << " left " << getAggrNum(groupAddr) - 1 << " flows." << endl;
            //! update the reverse routing entry
            aggrtable[groupAddr].push_back(pk->getSrcAddr());
            if (aggrCounter.find(groupAddr) == aggrCounter.end()) { // ! the first packet
                aggrCounter[groupAddr] = getAggrNum(groupAddr) - 1;
                aggrPacket[groupAddr] = pk;
            } else { // the following packets
                delete pk;
                aggrCounter[groupAddr] -= 1;
            }

            if (aggrCounter[groupAddr] == 0) { // all packets aggregated
                auto pk = aggrPacket[groupAddr];
                pk->setHopCount(pk->getHopCount()+1); // todo how to count hop
                emit(outputIfSignal, outGateIndex);
                emit(outputPacketSignal, pk);
                send(pk, "out", outGateIndex);
            }
        } else if (pk->getKind()==PacketType::ACK) {
            // find entries and broadcast it
            auto broadcastAddresses = aggrtable[groupAddr];
            for (auto i = 0; i < broadcastAddresses.size(); i++) {
                int outGateIndex = getRouteGateIndex(broadcastAddresses[i]);
                EV << "Forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;
                pk->setDestAddr(broadcastAddresses[i]);
                send(pk->dup(), "out", outGateIndex);
            }
            delete pk;
//            EV << "Forwarding packet " << pk->getName() << " on gate index " << broadcastGateIndexes.back()<< endl;
//            send(pk, broadcastGateIndexes.back());
        } else {
            throw cRuntimeError("Unkonwn packet type!");
        }
    }
    else {
        EV << "Forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;
        pk->setHopCount(pk->getHopCount()+1);
        emit(outputIfSignal, outGateIndex);
        emit(outputPacketSignal, pk);
        send(pk, "out", outGateIndex);
    }

}

