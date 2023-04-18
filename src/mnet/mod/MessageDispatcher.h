#pragma once
#include <omnetpp.h>
#include "Connection.h"
#include "Packet_m.h"
using namespace omnetpp;

class  PortDispatcher : public cSimpleModule {
public:
    std::map<PortNumber, int> portToGateIndex;
    std::map<IntAddress, int> addrToGateIndex; // * store incoming connections
private:
    int findGateIndexByPort(PortNumber port);
private:
    // inherited funcitons
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return Stage::NUM_INIT_STAGES; }
};
