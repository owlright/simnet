#pragma once
#include <omnetpp.h>

using namespace omnetpp;

class FlowApp : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

