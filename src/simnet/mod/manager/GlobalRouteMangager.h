#pragma once
#include <omnetpp.h>
#include "simnet/common/Defs.h"
#include <unordered_map>
using namespace omnetpp;

class GlobalRouteManager: public cSimpleModule {
public:
    std::vector<int> getRoutes(IntAddress src, IntAddress dest) const;
    GlobalRouteManager() {};
    ~GlobalRouteManager() {delete topo;};

protected:
    void initialize(int stage) override;

private:
    void setNodes(const cTopology *topo);
    void handleMessage(cMessage * msg) override {throw cRuntimeError("this module doesn't handle messages, it runs only in initialize()");};

private:
    cTopology *topo{nullptr};
    std::vector<int> hostNodes;
    std::unordered_map<IntAddress, IntAddress> node2addr;
    std::unordered_map<IntAddress, IntAddress> addr2node;

};
