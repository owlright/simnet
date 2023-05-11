#pragma once
#include <omnetpp.h>
#include "GlobalView.h"
#include <unordered_map>
using namespace omnetpp;

class TrafficPatternManager: public GlobalView
{
public:
    IntAddress getDestAddr(IntAddress srcAddr) const;

protected:
    void initialize(int stage) override;
    cTopology::Node* getNode(IntAddress address) const;

protected:
    std::string trafficPattern;
};
