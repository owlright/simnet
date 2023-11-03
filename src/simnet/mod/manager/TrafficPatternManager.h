#pragma once
#include <omnetpp.h>
#include "GlobalManager.h"
#include "GlobalGroupManager.h"
#include "simnet/common/ModuleAccess.h"
#include <unordered_map>
#include <algorithm>
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
    std::vector<IntAddress> idleHosts;
    opp_component_ptr<GlobalGroupManager> groupManager;
};
