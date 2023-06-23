#include "UnicastApp.h"
#include "simnet/mod/contract/MessageDispatcher.h"

Define_Module(UnicastApp);

void UnicastApp::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        localAddr = par("address");
        localPort = par("port");
        EV << "node: " << localAddr << " localport: " << localPort << endl;
        connection = createConnection();
        auto connectedGateIndex = gate("out")->getPathEndGate()->getIndex();
        check_and_cast<PortDispatcher*>(getParentModule()->getSubmodule("at"))->registerPort(localPort, connectedGateIndex);
    }
    if (stage==INITSTAGE_LAST && isIdle()) {
        this->callFinish();
        this->deleteModule();
    }
}

void UnicastApp::handleMessage(cMessage *msg)
{
    if (msg->isPacket()) {
        connection->processMessage(msg);
    }

}

void UnicastApp::connectionDataArrived(Connection *connection, cMessage* msg)
{
    delete msg;
}

Connection* UnicastApp::createConnection(int connId)
{
    auto connectionId = connId == -1 ? cSimulation::getActiveEnvir()->getUniqueNumber() : connId;
    auto conn = new Connection(connectionId);
    conn->bind(localAddr, localPort);
    conn->setOutputGate(gate("out"));
    conn->setCallback(this);
    return conn;
}
