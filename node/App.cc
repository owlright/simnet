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

#include "App.h"
#include "..\common\Defs.h"
#include "Controller.h"
#include "..\common\ModuleAccess.h"
// struct FlowItem {
//     bool isDirectionIn; // in or out
//     long seq; // the packet sequence has been confirmed by now
// } ;
// std::ostream& operator<<(std::ostream& os, const FlowItem& fl)
// {
//     std::string direction = fl.isDirectionIn ? "in" : "out";
//     os << "direction = " << direction  << "  flow_seq =" << fl.seq; // no endl!
//     return os;
// }

Define_Module(App);
int
App::GetMyAddr() const {
    return this->myAddress;
}
App::~App()
{
    cancelAndDelete(generatePacket);
    for (auto it = socketsTable.begin(); it != socketsTable.end(); it++) {
        delete it->second;
    }
}

void App::initialize(int stage)
{
    if (stage == Stage::INITSTAGE_LOCAL) {
        myAddress = par("address");
        destAddress = par("destAddress");
        groupAddress = par("groupAddress");
        packetTotalCount = par("packetTotalCount");
        packetLengthBytes = &par("packetLength");
        sendIATime = &par("sendIaTime");  // volatile parameter
        packetLossCounter = 0;
        pkCounter = 0;
        disableSending = (destAddress==-1);
        WATCH(pkCounter);
        WATCH(myAddress);
        WATCH(destAddress);
        WATCH_VECTOR(destAddresses);
        WATCH_PTRMAP(socketsTable);
        endToEndDelaySignal = registerSignal("endToEndDelay");
        hopCountSignal = registerSignal("hopCount");
        sourceAddressSignal = registerSignal("sourceAddress");

        if (disableSending) {
            EV << myAddress << " don't send packets." << endl;
        }
        if (!disableSending) {
            generatePacket = new cMessage("nextPacket");
            scheduleAt(sendIATime->doubleValue(), generatePacket);
        }
    }
    if (stage == Stage::INITSTAGE_REPORT) {
        if (!disableSending && groupAddress != -1) {
            auto controller = getModuleFromPar<Controller>(this->getParentModule()->par("globalController"), this->getParentModule());
            controller->updateAggrGroup(groupAddress, myAddress);
        }
    }
    if (stage == Stage::INITSTAGE_CONTROLL) {
        auto controller = getModuleFromPar<Controller>(this->getParentModule()->par("globalController"), this->getParentModule());
        if (controller->isGroupTarget(myAddress)) {
            groupSenders = controller->getAggrSendersNum(myAddress);
            EV << "node "<< myAddress << " have " << groupSenders << " senders" << endl;
        };
    }


//    const char *destAddressesPar = par("destAddresses");
//    cStringTokenizer tokenizer(destAddressesPar);
//    const char *token;
    //! not use multiple destinations by now.
    // while ((token = tokenizer.nextToken()) != nullptr)
    //     destAddresses.push_back(atoi(token));

    // if (destAddresses.size() == 0)
    //     throw cRuntimeError("At least one address must be specified in the destAddresses parameter!");



}

void App::handleMessage(cMessage *msg)
{
    if (msg == generatePacket) {
        // Sending packet
        // int destAddress = destAddresses[intuniform(0, destAddresses.size()-1)];
        pkCounter++;
        socketsTable[destAddress] = new Socket(myAddress, destAddress, groupAddress);
        socketsTable[destAddress]->SetApp(this);
        socketsTable[destAddress]->SendData(packetTotalCount, packetLengthBytes->intValue());
    }
    else {
        // Handle incoming packet
        Packet *pk = check_and_cast<Packet *>(msg);
        // EV << "received packet " << pk->getName() << " after " << pk->getHopCount() << "hops" << endl;
        int senderAddr = pk->getSrcAddr();
        // short packetKind = pk->getKind();
        // if (senderAddr == myAddress) { // ignore the packet send from myself
        //     delete pk;
        //     return;
        // }

        if (hasGUI())
            getParentModule()->bubble("Arrived!");

        if (socketsTable.find(senderAddr) == socketsTable.end()) {
            socketsTable[senderAddr] = new Socket(myAddress, senderAddr, -1);
            socketsTable[senderAddr]->SetApp(this);
        }
        socketsTable[senderAddr]->Recv(pk);
        delete pk;
        pk=nullptr;
        // if (packetKind == 1) { // data packet
        //     socketsTable[senderAddr]->ProcessData(pk);
        // }
        // else if (packetKind == 0) { // ack packet
        //     socketsTable[senderAddr]->ProcessAck(pk);
        // }
    }
}

