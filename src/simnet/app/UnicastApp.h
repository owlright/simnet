#pragma once

#include <omnetpp.h>
#include "../mod/Packet_m.h"
#include "../mod/Connection.h"

using namespace omnetpp;

class UnicastApp : public cSimpleModule, public Connection::ICallback
{
protected:
    Connection* connection{nullptr}; // this connection is just for listen incoming connections.
    IntAddress localAddr{INVALID_ADDRESS};
    PortNumber localPort{INVALID_PORT};

public:
    virtual ~UnicastApp() {};

protected:
    // inherited functions
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return Stage::NUM_INIT_STAGES; }
    // for callback function use
    virtual void connectionDataArrived(Connection *connection, cMessage *msg) override;

    virtual Connection* createConnection(int connId=-1);

};
