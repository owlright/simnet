#pragma once
#include <unordered_map>
#include "simnet/common/ModuleAccess.h"
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
        SeqNumber nextSeq{0};
        SeqNumber nextAckNumber{0};
        SeqNumber totalBytes{0};
        bool isOpen{true};
    public:
        void resetState() {
            nextSeq = 0;
            nextAckNumber = 0;
            totalBytes = 0;
            isOpen = true;
        }
    };
    // configuration
    std::unordered_map<IdNumber, FlowTable> flows;
    B ackPacketSize{64};

protected:
    virtual void connectionDataArrived(Connection *connection, Packet* pk) override;
    virtual Packet* createAckPacket(const Packet* const pk);

};
