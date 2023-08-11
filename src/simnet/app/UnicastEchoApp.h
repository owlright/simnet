#pragma once
#include <unordered_map>
#include "ConnectionApp.h"
using namespace omnetpp;

class EchoApp : public ConnectionApp
{

public:
    virtual ~EchoApp();

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;

protected:
    struct FlowTable {
        SeqNumber remoteAddr{INVALID_ADDRESS};
        PortNumber remotePort{INVALID_PORT};
        Connection* connection{nullptr};
        SeqNumber nextAckNumber{0};
        SeqNumber lastAckNumber{INT64_MAX};
        SeqNumber lastAskedSeq{INT64_MAX};
    };
    // configuration
    std::unordered_map<IdNumber, FlowTable> flows;
    B receivedBytes;

protected:

    virtual void connectionDataArrived(Connection *connection, Packet* pk) override;
    virtual Packet* createAckPacket(const Packet* const pk);

};
