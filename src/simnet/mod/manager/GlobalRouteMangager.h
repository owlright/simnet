#pragma once
#include <omnetpp.h>
#include "GlobalView.h"
#include <unordered_map>
using namespace omnetpp;

class GlobalRouteManager: public GlobalView
{
public:
    std::vector<int> getRoutes(IntAddress switchAddr, IntAddress dest) const;
    GlobalRouteManager() {};


protected:
    void initialize(int stage) override;
    cTopology::Node* getNode(IntAddress address) const;
};
