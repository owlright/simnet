#pragma once
#include <omnetpp.h>
#include "../mod/Packet_m.h"
#include "../mod/socket.h"
#include "../common/Defs.h"
using namespace omnetpp;

class FlowApp : public cSimpleModule
{
public:
    ~FlowApp();
protected:
    bool loadModeEnabled{false};
    simtime_t interval{SIMTIME_ZERO};
private:
    int myAddress;
    int destAddress;
    int groupAddress;
    int packetLengthBytes;
    cPar* flowLength = nullptr;
    Socket* socket = nullptr;
    // state
    cMessage *startFlowTimer{nullptr};
protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
};

