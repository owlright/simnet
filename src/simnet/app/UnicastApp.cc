#include "UnicastApp.h"
#include "simnet/mod/contract/MessageDispatcher.h"

Define_Module(UnicastApp);

void UnicastApp::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL || stage == INITSTAGE_ACCEPT) {
        localAddr = getParentModule()->par("address");
        localPort = par("port");
        // EV_DEBUG << " localport: " << localPort << endl;
        if (localAddr != INVALID_ADDRESS && localPort != INVALID_PORT) {
            connection = createConnection();
            auto connectedGateIndex = gate("out")->getPathEndGate()->getIndex();
            check_and_cast<PortDispatcher*>(getParentModule()->getSubmodule("at"))->registerPort(localPort, connectedGateIndex);
        }
    }
    else if (stage==INITSTAGE_LAST) {
       if (isIdle()) {
           this->callFinish();
           this->deleteModule();
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
