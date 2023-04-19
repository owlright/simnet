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

        //signals
        cwndSignal = registerSignal("cwnd");
        rttSignal = registerSignal("rtt");
        //schedule sending event
        flowStartTimer = new cMessage("flowStart");
        scheduleAfter(exponential(flowInterval->intValue()), flowStartTimer);
        EV_DETAIL << "node: " << myAddr << " " << "destAddr: " << destAddr << endl;
    }

}

void UnicastSenderApp::handleMessage(cMessage *msg)
{
    if (msg == flowStartTimer) {
        processSend();
        return;
    }
    UnicastApp::handleMessage(msg);
}

void UnicastSenderApp::processSend()
{
    auto packetSize = messageLength;
    if (flowSize != 0 && messageLength + sentBytes > flowSize) {
        packetSize = flowSize - sentBytes; // the data about to send is too small.
    }
    sentBytes += packetSize;
    // make packet
    auto packet = new Packet();
    packet->setKind(PacketType::DATA);
    setCommonField(packet);
    packet->setDestAddr(destAddr);
    packet->setDestPort(destPort);
    connection.sendTo(packet, destAddr, destPort);
}

void UnicastSenderApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    ASSERT(pk->getKind()==PacketType::ACK);
    //TODO if all packets sended
    //TODO if all packets are confirmed
    // connection->sendTo();
    delete pk;
}

cMessage *UnicastSenderApp::makeDataPacket(Connection *connection, Packet *pk)
{
    char pkname[40];
    sprintf(pkname, "DATA-%" PRId64 "-to-%" PRId64 "-seq%" PRId64, myAddr, destAddr, sentBytes);
    pk->setName(pkname);
    pk->setECN(false);
    pk->setByteLength(messageLength);
    return pk;
}



