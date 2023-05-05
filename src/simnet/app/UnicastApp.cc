#include "UnicastApp.h"
#include "simnet/mod/contract/MessageDispatcher.h"

Define_Module(UnicastApp);

void UnicastApp::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        localAddr = par("address");
        localPort = par("port");
        connection.bind(localAddr, localPort);
        connection.setOutputGate(gate("out"));
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

void UnicastApp::setCommonField(Packet *packet)
{
    packet->setSrcAddr(localAddr);
    packet->setLocalPort(localPort);

}

void UnicastApp::connectionDataArrived(Connection *connection, cMessage* msg)
{
    delete msg;
}
