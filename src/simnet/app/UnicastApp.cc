#include "UnicastApp.h"
#include "simnet/mod/contract/MessageDispatcher.h"

Define_Module(UnicastApp);

void UnicastApp::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        localAddr = par("address");
        localPort = par("port");
        connection.bind(localAddr, localPort, gate("out"));
        connection.setCallback(this);
        EV << "node: " << localAddr << " localport: " << localPort << endl;
        auto connectedGateIndex = gate("out")->getPathEndGate()->getIndex();
        check_and_cast<PortDispatcher*>(getParentModule()->getSubmodule("at"))->registerPort(localPort, connectedGateIndex);
    }
}

void UnicastApp::handleMessage(cMessage *msg)
{
    if (msg->isPacket()) {
        connection.processMessage(msg);
    }

}

cMessage* UnicastApp::makePacket(Connection *connection, cMessage* msg, IntAddress destAddr, PortNumber destPort) {
    auto pk = check_and_cast<Packet*>(msg);
    ASSERT(pk->getConnectionId() == connection->getConnectionId());

    switch (pk->getKind()) {
        case DATA:
            return makeDataPacket(connection, pk);
        case ACK:
            return makeAckPacket(connection, pk);
        default:
            throw cRuntimeError("Unknow packet type");
    }
}

void UnicastApp::setCommonField(Packet *packet)
{
    packet->setSrcAddr(localAddr);
    packet->setLocalPort(localPort);

}

void UnicastApp::connectionDataArrived(Connection *connection, cMessage* msg)
{
    delete msg;
}
