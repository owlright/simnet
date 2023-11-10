#pragma once
#include "GlobalGroupManager.h"
#include "GlobalManager.h"
#include "simnet/common/ModuleAccess.h"
#include <algorithm>
#include <omnetpp.h>
#include <unordered_map>
using namespace omnetpp;

class TrafficPatternManager : public GlobalManager {
public:
    IntAddress getDestAddr(IntAddress srcAddr) const;
    std::vector<IntAddress> getDestAddrs(IntAddress srcAddr, int n = 1) const;

protected:
    void initialize(int stage) override;

protected:
    std::string trafficPattern;
    std::vector<IntAddress> idleHosts;
    opp_component_ptr<GlobalGroupManager> groupManager;

private:
    std::map<IntAddress, int> host2odIndex;
    std::vector<std::vector<int>> odMatrix;
};
