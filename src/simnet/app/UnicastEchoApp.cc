#include "UnicastEchoApp.h"

Define_Module(UnicastEchoApp);

void UnicastEchoApp::initialize(int stage)
{
    UnicastApp::initialize(stage);
    if (stage==INITSTAGE_LOCAL) {
        connection.setCallback(this);
    }
}

// void UnicastEchoApp::handleMessage(cMessage *msg)
// {
//     UnicastApp::handleMessage(msg);
// }

void UnicastEchoApp::onNewConnectionArrived(Packet *pk)
{
    IdNumber connectionid = pk->getConnectionId();
    connections[connectionid] = new Connection();
    connections[connectionid]->setConnectionId(connectionid);
    connections[connectionid]->bind(localAddr, localPort, gate("out"));
    connections[connectionid]->setCallback(this);
}

void UnicastEchoApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    ASSERT(pk->getKind()==PacketType::DATA);
    IdNumber connectionid = pk->getConnectionId();

    if (connection->getConnectionId()==connectionid) {
        //TODO where should I put these code
        dealWithDataPacket(connection, pk);
        delete pk;
        return;
    }
    // ! The code must be put here
    if (connections.find(connectionid) == connections.end()){
        onNewConnectionArrived(pk);
    }

    connections[connectionid]->processMessage(msg); // this make it to run into this function again


}

void UnicastEchoApp::dealWithDataPacket(Connection *connection, Packet* pk)
{
    auto packet = new Packet();
    setCommonField(packet);
    packet->setConnectionId(connection->getConnectionId());
    packet->setSeqNumber(pk->getSeqNumber());
    packet->setKind(PacketType::ACK);
    packet->setDestAddr(pk->getSrcAddr());
    packet->setDestPort(pk->getLocalPort());
    if (pk->getECN()) {
        packet->setECE(true);
    }
    connection->sendTo(packet, pk->getSrcAddr(), pk->getLocalPort());
}

cMessage *UnicastEchoApp::makeAckPacket(Connection *connection, Packet *pk)
{
    char pkname[40];
    sprintf(pkname, "ACK-%" PRId64 "-to-%" PRId64 "-seq%" PRId64, localAddr, pk->getDestAddr(), pk->getSeqNumber());
    pk->setKind(PacketType::ACK);
    pk->setName(pkname);
    pk->setECN(false);
    pk->setByteLength(64);
    return pk;
}
