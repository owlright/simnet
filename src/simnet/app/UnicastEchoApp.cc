#include "UnicastEchoApp.h"

Define_Module(EchoApp);

EchoApp::~EchoApp()
{
    for (auto&it : flows) {
        auto& conn = it.second.connection;
        delete conn;
        conn = nullptr;
    }
}

void EchoApp::initialize(int stage)
{
    ConnectionApp::initialize(stage);
    if (stage==INITSTAGE_LOCAL) {

    }
}

void EchoApp::handleMessage(cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    auto connId = pk->getConnectionId();
    auto it = flows.find(connId);
    if (it == flows.end()) {
        flows[connId].connection = createConnection(connId);
        flows[connId].connection->bindRemote(pk->getSrcAddr(), pk->getLocalPort());
    }
    auto& flow = flows.at(connId);
    if (!flow.isOpen) { // the same src Addr again
        if (pk->getSrcAddr() != flow.connection->getDestAddr())
            throw cRuntimeError("connection not match");
        flow.resetState();
    }
    flows.at(connId).connection->processPacket(pk);
}

void EchoApp::connectionDataArrived(Connection *connection, Packet* pk)
{
    auto connId = pk->getConnectionId();
    auto seqNumber = pk->getSeqNumber();
    auto srcAddr = pk->getSrcAddr();
    auto& flow = flows.at(connId);
    if (pk->getPacketType() == DATA) {
        flow.recvBytes += pk->getByteLength();
    }
    else if (pk->getPacketType() == ACK) {
        delete pk;
        return;
    }
    ASSERT(pk->getPacketType() == DATA);
    ASSERT(connId == connection->getConnectionId());
    // if (localAddr == 1)
    //     std::cout << pk->getName() << endl;

    if (pk->getFIN()) {
        flow.totalBytes = seqNumber + pk->getByteLength();
    }

    if (seqNumber >= flow.nextAckNumber) {
        // ! ignore any disorder or resend packets
        // ! because there are no packet loss happen
        flow.nextAckNumber = seqNumber + pk->getByteLength();
    }

    auto packet = createAckPacket(pk);

    if (flow.recvBytes == flow.totalBytes) {
        packet->setFIN(true);
        flow.isOpen = false;
    }
    char pkname[40];
    auto seq = flow.nextSeq;
    flow.nextSeq += ackPacketSize;
    auto ackSeq = flow.nextAckNumber;
    sprintf(pkname, "ACK-%" PRId64 "-to-%" PRId64 "-seq-%" PRId64 "-ack-%" PRId64,
        localAddr, srcAddr, seq, ackSeq);
    packet->setName(pkname);
    packet->setSeqNumber(seq);
    packet->setAckNumber(ackSeq);
    connection->send(packet);
    delete pk;
}

Packet *EchoApp::createAckPacket(const Packet* const pk)
{
    auto packet = new Packet();
    packet->setDestAddr(pk->getSrcAddr());
    packet->setDestPort(pk->getLocalPort());
    packet->setECE(pk->getECN());
    packet->setKind(PacketType::ACK);
    packet->setByteLength(ackPacketSize);
    // packet->setSendTime(pk->getSendTime());
    packet->setQueueTime(pk->getQueueTime());
    packet->setTransmitTime(pk->getTransmitTime());
    return packet;
}

void EchoApp::refreshDisplay() const
{
    if (!getEnvir()->isExpressMode()) {
        char buf[20];
        sprintf(buf, "local:%" PRId64 ":%u", localAddr, localPort);
        getDisplayString().setTagArg("t", 0, buf);
    }
}
