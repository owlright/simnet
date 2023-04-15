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

#include "UnicastApp.h"
#include "../common/Defs.h"
#include "../mod/Controller.h"
#include "../common/ModuleAccess.h"

Define_Module(UnicastApp);

UnicastApp::~UnicastApp()
{
    cancelAndDelete(flowStartTimer);
}

void UnicastApp::initialize(int stage)
{
    switch (stage) {
        case Stage::INITSTAGE_LOCAL:
            myAddr = par("addresss");
            localPort = par("port");
            destAddr = par("destAddress");
            destPort = par("destPort");
            messageLength = par("messageLength");
            flowSize = par("flowSize");
            flowInterval = &par("interval");

            connection.bind(myAddr, localPort);
            connection.setCallback(this);
            connection.setOutputGate(gate("unicastOut"));

            startTime = par("startTime");
            cwndSignal = registerSignal("cwnd");
            rttSignal = registerSignal("rtt");

            flowStartTimer = new cMessage("flowStart");
            scheduleAfter(exponential(flowInterval->intValue()), flowStartTimer);

            break;
        case Stage::INITSTAGE_REPORT:
            break;
        case Stage::INITSTAGE_CONTROLL:
            break;
        default:
            throw cRuntimeError("UnicastApp::initialize(int stage): Should not run here");
    }

}

void UnicastApp::handleMessage(cMessage *msg)
{
    if (msg == flowStartTimer) {
        processSend();
    } else if (!msg->isSelfMessage()) { // the message comes from outside
        connection.processMessage(msg);
    } else {
        throw cRuntimeError("UnicastApp::handleMessage: Should not run here");
    }
}

void UnicastApp::processSend() {
    auto packetSize = messageLength;
    if (flowSize != 0 && messageLength + sentBytes > flowSize) {
        packetSize = flowSize - sentBytes; // the data about to send is too small.
    }
    sentBytes += packetSize;
    // make packet
    char pkname[40];
    sprintf(pkname, "DATA-%d-to-%d-seq%u ", myAddr, destAddr, sentBytes);
    auto packet = new Packet(pkname);
    packet->setByteLength(packetSize);
    // packet->setConnectionId(connection.getConnectionId());
    // packet->setFlowSize(flowSize);
    packet->setPacketType(PacketType::DATA);

    connection.sendTo(packet, destAddr, destPort);
}

void UnicastApp::connectionDataArrived(Connection *connection, Packet *packet)
{
    //TODO if all packets sended
    //TODO if all packets are confirmed
}
