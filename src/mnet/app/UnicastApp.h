#pragma once

#include <omnetpp.h>
#include "../mod/Packet_m.h"
#include "../mod/Connection.h"

using namespace omnetpp;

class UnicastApp : public cSimpleModule, public Connection::ICallback
{
protected:
    Connection connection;
    IntAddress myAddr;
    PortNumber localPort;

public:
    virtual ~UnicastApp();

protected:
    // inherited funcitons
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return Stage::NUM_INIT_STAGES; }
    // for callback function use
    virtual void connectionDataArrived(Connection *connection, cMessage *msg) override;
    cMessage* makePacket(Connection *connection, cMessage* msg, IntAddress destAddr, PortNumber destPort) override;

    void setCommonField(Packet* pk);
    virtual cMessage* makeDataPacket(Connection *connection, Packet* msg) {};
    virtual cMessage* makeAckPacket(Connection *connection, Packet* msg) {};
};
