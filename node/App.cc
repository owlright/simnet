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
    // m_socket = new Socket();
    // m_socket->SetInitialCwnd(1);
    // m_socket->SetInitialSSThresh(UINT32_MAX);


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
        socketsTable[destAddress] = new Socket(myAddress, destAddress, 1, UINT32_MAX);
        socketsTable[destAddress]->SetApp(this);
        // socketsTable[destAddress]->SetSendCb(std::bind(&cSimpleModule::send, this, std::placeholders::_1, std::placeholders::_2, -1));
        socketsTable[destAddress]->SendData(packetTotalCount, packetLengthBytes->intValue());
        // Packet *pk = new Packet(pkname);
        // pk->setByteLength(packetLengthBytes->intValue());
        // pk->setKind(DATA);
        // pk->setSeq(pkCounter-1);
        // pk->setSrcAddr(myAddress);
        // pk->setDestAddr(destAddress);
        // send(pk, "out");
        // scheduleAt(simTime() + sendIATime->doubleValue(), generatePacket);
        // if (hasGUI())
        //     getParentModule()->bubble("Generating packet...");
    }
    else {
        // Handle incoming packet
        // data packet
        Packet *pk = check_and_cast<Packet *>(msg);
        EV << "received packet " << pk->getName() << " after " << pk->getHopCount() << "hops" << endl;
        int senderAddr = pk->getSrcAddr();
        short packetKind = pk->getKind();
        if (senderAddr == myAddress) { // the packet send from myself
            delete pk;
            return;
        }
        // ack packet



    //     emit(endToEndDelaySignal, simTime() - pk->getCreationTime());
    //     emit(hopCountSignal, pk->getHopCount());
    //     emit(sourceAddressSignal, pk->getSrcAddr());

    //     if (senderAddr==myAddress) { // the packet send from myself
    //         delete pk;
    //         return;
    //     }
        if (hasGUI())
            getParentModule()->bubble("Arrived!");

    //     char pkname[40];
        if (packetKind == 1) {
    //         int outPortIndex = pk->par("outGateIndex");
    //         double rate = getParentModule()->gate("port$o", outPortIndex)->getChannel()->par("datarate");
    //         EV << pk->getName() <<"comes from port " << outPortIndex << " channelrate is " << rate <<endl;
            socketsTable[senderAddr]->ProcessData(pk);

    //         else {
    //             // deal with the first data packet
    //             ftable[senderAddr] = new FlowItem();
    //             ftable[senderAddr]->isDirectionIn = true;
    //             ftable[senderAddr]->seq = pkseq + 1;
    //         }

    //         sprintf(pkname, "Ack-%d-to-%d-#%ld", myAddress, senderAddr, ftable[senderAddr]->seq);
    //         Packet *ackpk = new Packet(pkname);
    //         ackpk->setByteLength(1);
    //         ackpk->setKind(ACK);
    //         ackpk->setAckSeq(ftable[senderAddr]->seq);
    //         ackpk->setSrcAddr(myAddress);
    //         ackpk->setDestAddr(senderAddr);

    //         if (hasGUI())
    //             getParentModule()->bubble("Generating ack packet...");
    //         send(ackpk, "out");
    //         delete pk;
       }
       else if (packetKind == 0) {
        assert(socketsTable.find(senderAddr) != socketsTable.end());
        socketsTable[senderAddr]->ProcessAck(pk);
    //         if (pkCounter-packetLossCounter < packetTotalCount) {
    //             auto pkAckSeq = pk->getAckSeq();
    //             auto it = ftable.find(myAddress);
    //             assert(!it->second->isDirectionIn); // the flow must start with this app.
    //             if (pkAckSeq == it->second->seq + 1) {
    //                 it->second->seq++;
    //                 pkCounter++;
    //             }
    //             else {
    //                 packetLossCounter++;
    //             }
    //             char pkname[40];
    //             sprintf(pkname, "pk-%d-to-%d-#%ld", myAddress, senderAddr, it->second->seq);
    //             Packet *npk = new Packet(pkname);
    //             npk->setByteLength(packetLengthBytes->intValue());
    //             npk->setKind(DATA);
    //             npk->setSeq(it->second->seq);
    //             npk->setSrcAddr(myAddress);
    //             npk->setDestAddr(senderAddr);
    //             if (hasGUI())
    //                 getParentModule()->bubble("Generating next data packet...");
    //             send(npk, "out");
    //         }
    //         delete pk;
       }
    }
}

