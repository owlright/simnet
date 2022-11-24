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

void App::initialize()
{
    myAddress = par("address");
    destAddress = par("destAddress");
    packetTotalCount = par("packetTotalCount");
    packetLengthBytes = &par("packetLength");
    sendIATime = &par("sendIaTime");  // volatile parameter
    packetLossCounter = 0;
    pkCounter = 0;
    seq = 0;

    std::string appType = getParentModule()->par("nodeType");
    disableSending = appType == "Switch" || appType == "Sink";
    if (disableSending)
        EV << "node type is "<< appType << " disable generating packets." << endl;

    WATCH(pkCounter);
    WATCH(myAddress);
    WATCH(destAddress);
    WATCH_VECTOR(destAddresses);
    WATCH_PTRMAP(socketsTable);
    const char *destAddressesPar = par("destAddresses");
    cStringTokenizer tokenizer(destAddressesPar);
    const char *token;
    while ((token = tokenizer.nextToken()) != nullptr)
        destAddresses.push_back(atoi(token));

    if (destAddresses.size() == 0)
        throw cRuntimeError("At least one address must be specified in the destAddresses parameter!");
    if (!disableSending) {
        generatePacket = new cMessage("nextPacket");
        scheduleAt(sendIATime->doubleValue(), generatePacket);
    }

    endToEndDelaySignal = registerSignal("endToEndDelay");
    hopCountSignal = registerSignal("hopCount");
    sourceAddressSignal = registerSignal("sourceAddress");
}

void App::handleMessage(cMessage *msg)
{
    if (msg == generatePacket) {
        // Sending packet
        // int destAddress = destAddresses[intuniform(0, destAddresses.size()-1)];
        pkCounter++;
        socketsTable[destAddress] = new Socket(myAddress, destAddress);
        socketsTable[destAddress]->SetApp(this);
        socketsTable[destAddress]->SendData(packetTotalCount, packetLengthBytes->intValue());
    }
    else {
        // Handle incoming packet
        Packet *pk = check_and_cast<Packet *>(msg);
        // EV << "received packet " << pk->getName() << " after " << pk->getHopCount() << "hops" << endl;
        int senderAddr = pk->getSrcAddr();
        short packetKind = pk->getKind();
        if (senderAddr == myAddress) { // ignore the packet send from myself
            delete pk;
            return;
        }

        if (hasGUI())
            getParentModule()->bubble("Arrived!");

        if (socketsTable.find(senderAddr) == socketsTable.end()) {
            socketsTable[senderAddr] = new Socket(myAddress, senderAddr);
            socketsTable[senderAddr]->SetApp(this);
        }

        if (packetKind == 1) { // data packet
            socketsTable[senderAddr]->ProcessData(pk);
        }
        else if (packetKind == 0) { // ack packet
            socketsTable[senderAddr]->ProcessAck(pk);
        }
    }
}

