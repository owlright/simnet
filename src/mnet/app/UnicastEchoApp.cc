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
    connections[connectionid] = new Connection(connectionid);
    connections[connectionid]->bind(myAddr, localPort, gate("out"));
    connections[connectionid]->setCallback(this);
}

void UnicastEchoApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    ASSERT(pk->getKind()==PacketType::DATA);
    IdNumber connectionid = pk->getConnectionId();
    if (connections.find(connectionid) == connections.end()) {
        onNewConnectionArrived(pk);
    } else {
        connections[connectionid]->processMessage(msg);
    }

    //TODO where should I put these code
    auto packet = new Packet();
    setCommonField(packet);
    packet->setKind(PacketType::ACK);
    packet->setDestAddr(pk->getSrcAddr());
    packet->setDestPort(pk->getLocalPort());
    connection->sendTo(packet, pk->getSrcAddr(), pk->getLocalPort());
    delete pk;
}

cMessage *UnicastEchoApp::makeAckPacket(Connection *connection, Packet *pk)
{
    char pkname[40];
    sprintf(pkname, "ACK%d-to-%d-seq%u ", myAddr, pk->getDestAddr(), pk->getSeqNumber());
    pk->setKind(PacketType::ACK);
    pk->setName(pkname);
    pk->setECN(false);
    pk->setByteLength(64);
    return pk;
}