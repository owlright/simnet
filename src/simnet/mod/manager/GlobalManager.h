#pragma once
#include "GlobalView.h"
#include "simnet/common/Defs.h"
#include "simnet/graph/graph.h"
#include <omnetpp.h>
using namespace omnetpp;

class GlobalManager : public cModule {
public:
    IntAddress getAddr(int nodeId) const;
    IntAddress getAddr(cModule* mod) const;
    IntAddress getAddr(cTopology::Node* node) const;
    int getNodeId(IntAddress addr) const;
    cModule* getMod(IntAddress addr) const;
    cTopology::Node* getNode(IntAddress addr) const;

protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    const std::vector<int>& getHostIds() const { return hostIds; }

protected:
    opp_component_ptr<GlobalView> globalView;
    // Mat<double> distAdj;
    // Mat<double> costAdj;
    cTopology* topo { nullptr };
    std::unordered_map<int, IntAddress> nodeId2Addr;
    std::unordered_map<IntAddress, int> addr2NodeId;
    simnet::Graph network;

private:
    std::vector<int> hostIds;
};