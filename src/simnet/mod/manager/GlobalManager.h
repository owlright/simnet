#pragma once
#include <omnetpp.h>
#include "simnet/common/Defs.h"
#include "GlobalView.h"
using namespace omnetpp;

class GlobalManager : public cModule
{
public:
    IntAddress getAddr(int nodeId) const;
    IntAddress getAddr(cModule* mod) const;
    int getNodeId(IntAddress addr) const;
    cModule* getMod(IntAddress addr) const;
    cTopology::Node* getNode(IntAddress addr) const;

protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

protected:
    opp_component_ptr<GlobalView> globalView;
    cTopology* topo{nullptr};
    std::vector<int> hostIds;
    std::unordered_map<int, IntAddress> nodeId2Addr;
    std::unordered_map<IntAddress, int> addr2NodeId;
};