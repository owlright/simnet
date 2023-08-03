#include "UnicastEchoApp.h"

Define_Module(EchoApp);

EchoApp::~EchoApp()
{
    for (auto&it : connections) {
        delete  it.second;
        it.second = nullptr;
    }
}

void EchoApp::initialize(int stage)
{
    UnicastApp::initialize(stage);
}

void EchoApp::handleMessage(cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    auto connectionId = pk->getConnectionId();
    auto it = connections.find(connectionId);
    if (it == connections.end()) {
        onNewConnectionArrived(connectionId, pk);
    }
    connections.at(connectionId)->processMessage(pk);
}

void EchoApp::onNewConnectionArrived(IdNumber connId, const Packet* const pk)
{
    connections[connId] = createConnection(connId);
    connections[connId]->bindRemote(pk->getSrcAddr(), pk->getLocalPort());
}

void EchoApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    ASSERT(pk->getKind()==PacketType::DATA);
    ASSERT(pk->getConnectionId()==connection->getConnectionId());

    auto packet = createAckPacket(pk);
    connection->send(packet);
    delete pk;
}

Packet *EchoApp::createAckPacket(const Packet* const pk)
{
    char pkname[40];
    sprintf(pkname, "ACK-%" PRId64 "-to-%" PRId64 "-seq%" PRId64,
            localAddr, pk->getSrcAddr(), pk->getSeqNumber());
    auto packet = new Packet(pkname);
    packet->setSeqNumber(pk->getSeqNumber());
    packet->setKind(PacketType::ACK);
    packet->setByteLength(64);
    // packet->setReceivedBytes(pk->getByteLength());
    packet->setStartTime(pk->getStartTime());
    packet->setQueueTime(pk->getQueueTime());
    packet->setTransmitTime(pk->getTransmitTime());
    if (pk->getECN()) {
        packet->setECE(true);
    }
    // packet->setIsFlowFinished(pk->isFlowFinished());
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
