#include "UnicastEchoApp.h"

Define_Module(UnicastEchoApp);

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
        onNewConnectionArrived(pk);
    }
    connections.at(connectionId)->processMessage(pk);
}

void UnicastEchoApp::onNewConnectionArrived(const Packet* const pk)
{
    IdNumber connectionId = pk->getConnectionId();
    connections[connectionId] = createConnection(connectionId);
    connections[connectionId]->bindRemote(pk->getSrcAddr(), pk->getLocalPort());
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
    if (pk->getECN()) {
        packet->setECE(true);
    }
    return packet;
}
