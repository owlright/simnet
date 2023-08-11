#include "ConnectionApp.h"
#include "simnet/mod/contract/MessageDispatcher.h"

Define_Module(ConnectionApp);

void ConnectionApp::initialize(int stage)
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

void ConnectionApp::handleMessage(cMessage *msg)
{
    if (msg->isPacket()) {
        connection->processMessage(msg);
    }

}

void ConnectionApp::connectionDataArrived(Connection *connection, cMessage* msg)
{
    delete msg;
}

void ConnectionApp::handleParameterChange(const char *parameterName)
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

Connection* ConnectionApp::createConnection(int connId)
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
