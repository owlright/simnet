#pragma once
#include <omnetpp.h>
#include "GlobalManager.h"
#include <unordered_map>
using namespace omnetpp;

class TrafficPatternManager: public GlobalManager
{
public:
    IntAddress getDestAddr(IntAddress srcAddr) const;
    std::vector<IntAddress> getDestAddrs(IntAddress srcAddr, int n = 1) const;

protected:
    void initialize(int stage) override;

protected:
    std::string trafficPattern;
};
