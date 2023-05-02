#include "UnicastSenderApp.h"
Define_Module(UnicastSenderApp);

UnicastSenderApp::~UnicastSenderApp() {
    cancelAndDelete(flowStartTimer);
}

void UnicastSenderApp::initialize(int stage)
{
    UnicastApp::initialize(stage);
    if (stage==INITSTAGE_LOCAL) {
        destAddr = par("destAddress");
        destPort = par("destPort");
        messageLength = par("messageLength");
        flowSize = par("flowSize");
        flowInterval = &par("flowInterval");
        connection.setConnectionId(cSimulation::getActiveEnvir()->getUniqueNumber());
        cong = check_and_cast<CongAlgo*>(getSubmodule("cong"));
        cong->setSegmentSize(messageLength);

        //signals
        cwndSignal = registerSignal("cwnd");
        rttSignal = registerSignal("rtt");
        //schedule sending event
        flowStartTimer = new cMessage("flowStart");
        scheduleAfter(flowInterval->doubleValue(), flowStartTimer);
        EV << "destAddr: " << destAddr << " destPort: " << destPort << endl;
    }

}

void UnicastSenderApp::handleMessage(cMessage *msg)
{
    if (msg == flowStartTimer) {
        sendPendingData();
    } else {
        UnicastApp::handleMessage(msg);
    }
}

void UnicastSenderApp::sendPendingData()
{
    while (cong->getcWnd() > inflightBytes() && sentBytes < flowSize) {
        auto packetSize = messageLength;
        if (flowSize != 0 && messageLength + sentBytes > flowSize) {
            packetSize = flowSize - sentBytes; // the data about to send is too small.
        }
        sentBytes += packetSize;
        // make packet
        auto packet = new Packet();
        setCommonField(packet);
        packet->setConnectionId(connection.getConnectionId());
        packet->setKind(PacketType::DATA);
        packet->setSeqNumber(sentBytes);
        packet->setByteLength(packetSize);
        packet->setDestAddr(destAddr);
        packet->setDestPort(destPort);
        connection.sendTo(packet, destAddr, destPort);
        cong->onSendData(packetSize);
    }

}

void UnicastSenderApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    ASSERT(pk->getKind()==PacketType::ACK);
    // let cong algo update state
    confirmedBytes = pk->getSeqNumber();
    cong->onRecvAck(pk->getSeqNumber(), pk->getECE());


    if (sentBytes < flowSize) {
        sendPendingData();
    } else {
        //TODO if all packets sended

    }
    //TODO if all packets are confirmed
    if (confirmedBytes == flowSize) {

    }
    // connection->sendTo();
    delete pk;
}

cMessage *UnicastSenderApp::makeDataPacket(Connection *connection, Packet *pk)
{
    char pkname[40];
    sprintf(pkname, "DATA-%" PRId64 "-to-%" PRId64 "-seq%" PRId64, myAddr, destAddr, sentBytes);
    pk->setName(pkname);
    pk->setECN(false);
    return pk;
}



