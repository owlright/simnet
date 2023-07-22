#pragma once
#include <omnetpp.h>
#include <unordered_map>
#include "simnet/common/Defs.h"
using namespace omnetpp;

class GlobalView : public cModule
{
public:
    cTopology* getGlobalTopo() const {return topo;}
    const std::vector<int> gethostIds() const {return hostNodes;}
    std::unordered_map<int, IntAddress> getNodeIdAddrMap() const {return nodeId2Addr;}
    std::unordered_map<IntAddress, int> getAddrNodeIdMap() const {return addr2NodeId;}
    ~GlobalView() { delete topo; topo=nullptr;}

protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return Stage::NUM_INIT_STAGES; }

private:
    cTopology* topo{nullptr};
    Mat<double> topoDist;
    std::vector<int> hostNodes;

    std::unordered_map<int, IntAddress> nodeId2Addr;
    std::unordered_map<IntAddress, int> addr2NodeId;
    // [[deprecated]] std::unordered_map<IntAddress, cModule*> addr2mod;
    // [[deprecated]] std::unordered_map<cModule*, IntAddress> mod2addr;
    // [[deprecated]] std::unordered_map<cTopology::Node*, IntAddress> node2addr;
    // [[deprecated]] std::unordered_map<IntAddress, cTopology::Node*> addr2node;

private:
    void collectNodes(cTopology* topo);

};
