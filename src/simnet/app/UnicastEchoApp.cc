#include "UnicastEchoApp.h"

Define_Module(UnicastEchoApp);

UnicastEchoApp::~UnicastEchoApp()
{
    for (auto&it : connections) {
        delete  it.second;
        it.second = nullptr;
    }
}

void UnicastEchoApp::initialize(int stage)
{
    UnicastApp::initialize(stage);
}

void UnicastEchoApp::handleMessage(cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    auto connectionId = pk->getConnectionId();
    auto it = connections.find(connectionId);
    if (it == connections.end()) {
        onNewConnectionArrived(connectionId, pk);
    }
    connections.at(connectionId)->processMessage(pk);
}

void UnicastEchoApp::onNewConnectionArrived(IdNumber connId, const Packet* const pk)
{
    connections[connId] = createConnection(connId);
    connections[connId]->bindRemote(pk->getSrcAddr(), pk->getLocalPort());
}

void UnicastEchoApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    ASSERT(pk->getKind()==PacketType::DATA);
    ASSERT(pk->getConnectionId()==connection->getConnectionId());

    auto packet = createAckPacket(pk);
    connection->send(packet);
    delete pk;
}

Packet *UnicastEchoApp::createAckPacket(const Packet* const pk)
{
    char pkname[40];
    sprintf(pkname, "ACK-%" PRId64 "-to-%" PRId64 "-seq%" PRId64,
            localAddr, pk->getDestAddr(), pk->getSeqNumber());
    auto packet = new Packet(pkname);
    packet->setSeqNumber(pk->getSeqNumber());
    packet->setKind(PacketType::ACK);
    packet->setByteLength(64);
    packet->setReceivedBytes(pk->getByteLength());
    packet->setStartTime(pk->getStartTime());
    packet->setQueueTime(pk->getQueueTime());
    packet->setTransmitTime(pk->getTransmitTime());
    if (pk->getECN()) {
        packet->setECE(true);
    }
    packet->setIsFlowFinished(pk->isFlowFinished());
    return packet;
}
