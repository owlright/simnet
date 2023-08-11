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
    flows.at(connId).connection->processPacket(pk);
}

void EchoApp::connectionDataArrived(Connection *connection, Packet* pk)
{
    auto connId = pk->getConnectionId();
    auto seq = pk->getSeqNumber();
    auto nextSeq = pk->getAckNumber(); // * always give what sender wants
    auto srcAddr = pk->getSrcAddr();
    auto& flow = flows.at(connId);
    ASSERT(pk->getKind()==PacketType::DATA);
    ASSERT(connId == connection->getConnectionId());
    // if (localAddr == 2)
    //     std::cout << pk->getName() << endl;

    char pkname[40];

    if (pk->getFIN()) {
        flow.lastAckNumber = seq + pk->getByteLength();
        flow.lastAskedSeq = nextSeq;
    }

    if (seq >= flow.nextAckNumber)
        flow.nextAckNumber = seq + pk->getByteLength();

    auto packet = createAckPacket(pk);

    if (flow.nextAckNumber == flow.lastAckNumber) {
        packet->setFIN(true);
        nextSeq = flow.lastAskedSeq + 1; // ! I don't want sender send another ACK to this FIN
    }

    sprintf(pkname, "ACK-%" PRId64 "-to-%" PRId64 "-seq-%" PRId64 "-ack-%" PRId64,
        localAddr, srcAddr, nextSeq, flow.nextAckNumber);
    packet->setName(pkname);
    packet->setSeqNumber(nextSeq);
    packet->setAckNumber(flow.nextAckNumber);
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
    packet->setByteLength(1);
    packet->setStartTime(pk->getStartTime());
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
