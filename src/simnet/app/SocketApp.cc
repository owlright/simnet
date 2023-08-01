#include "simnet/mod/contract/MessageDispatcher.h"
#include "SocketApp.h"

Define_Module(UnicastApp);

void UnicastApp::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        localPort = par("port");
        localAddr = getParentModule()->par("address");
         if (localAddr != INVALID_ADDRESS && localPort != INVALID_PORT) {
             connection = createConnection();
             auto connectedGateIndex = gate("out")->getPathEndGate()->getIndex();
             check_and_cast<PortDispatcher*>(getParentModule()->getSubmodule("at"))->registerPort(localPort, connectedGateIndex);
         }
    }
    else if (stage == INITSTAGE_LAST) {
        if (connection == nullptr) {
            throw cRuntimeError("connection is still nullptr");
        }
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

void UnicastApp::handleParameterChange(const char *parameterName)
{
    if (strcmp(parameterName, "address") == 0) {
        localAddr = par("address");
        // ! localAddr should only changed once
        if (connection != nullptr) {
            throw cRuntimeError("connection has already been setup!");
        }
        connection = createConnection();
        auto connectedGateIndex = gate("out")->getPathEndGate()->getIndex();
        check_and_cast<PortDispatcher*>(getParentModule()->getSubmodule("at"))->registerPort(localPort, connectedGateIndex);
    }
}

Connection* UnicastApp::createConnection(int connId)
{
    auto connectionId = connId == -1 ? cSimulation::getActiveEnvir()->getUniqueNumber() : connId;
    auto conn = new Connection(connectionId);
    ASSERT(localAddr != INVALID_ADDRESS);
    ASSERT(localPort != INVALID_PORT);
    conn->bind(localAddr, localPort);
    conn->setOutputGate(gate("out"));
    conn->setCallback(this);
    return conn;
}
