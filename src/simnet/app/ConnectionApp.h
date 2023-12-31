#pragma once

#include <omnetpp.h>
#include "../mod/Packet_m.h"
#include "../mod/Connection.h"

using namespace omnetpp;

class ConnectionApp : public cSimpleModule, public Connection::ICallback
{
protected:
    Connection* connection{nullptr}; // this connection is just for listen incoming connections.
    IntAddress localAddr{INVALID_ADDRESS};
    PortNumber localPort{INVALID_PORT};

public:
    virtual ~ConnectionApp() {delete connection; connection = nullptr;};

protected:
    // inherited functions
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    // for callback function use
    virtual void connectionDataArrived(Connection *connection, Packet* pk) override;

    virtual Connection* createConnection(int connId=-1);
    virtual void handleParameterChange(const char *parameterName) override;
};
