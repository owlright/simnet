#pragma once

#include <omnetpp.h>
#include "simnet/common/Defs.h"
#include "AggGroupEntry.h"
using namespace omnetpp;

class GlobalGroupManager : public cSimpleModule
{
public:
    AggGroupEntry* getGroupEntry(IntAddress group);

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override
        {throw cRuntimeError("this module doesn't handle messages, it runs only in initialize()");};
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
};

