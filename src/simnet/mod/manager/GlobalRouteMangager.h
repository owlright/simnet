#pragma once
#include <omnetpp.h>
#include "GlobalManager.h"
#include <unordered_map>
using namespace omnetpp;

class GlobalRouteManager: public GlobalManager
{
public:
    std::vector<int> getRoutes(IntAddress switchAddr, IntAddress dest) const;

protected:
    void initialize(int stage) override;
};
