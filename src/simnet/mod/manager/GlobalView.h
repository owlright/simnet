#pragma once
#include "simnet/common/Defs.h"
#include "simnet/graph/graph.h"
#include <omnetpp.h>
#include <unordered_map>
using std::vector;

using namespace omnetpp;
class GlobalView final : public cModule {
public:
    cTopology* getGlobalTopo() const { return topo; }
    const std::vector<int>& gethostIds() const { return hostNodes; }
    const std::unordered_map<int, IntAddress>& getNodeIdAddrMap() const { return nodeId2Addr; }
    const std::unordered_map<IntAddress, int>& getAddrNodeIdMap() const { return addr2NodeId; }
    simnet::Graph getNetwork() { return network; };
    // simnet::Graph Mat<double> getDistMat() const {return topoDist;}
    //  Mat<double> getCostAdj() const {return costAdj;}
    ~GlobalView()
    {
        delete topo;
        topo = nullptr;
    }
    Dict<int> primMST(const vector<int>& S, const vector<int>& mstnodes, const Mat<double>& oddist);
    void floyd_warshall(Mat<double>& distance);

protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return Stage::NUM_INIT_STAGES; }

private:
    cTopology* topo { nullptr };
    simnet::Graph network;

    std::vector<int> hostNodes;
    std::unordered_map<int, IntAddress> nodeId2Addr;
    std::unordered_map<IntAddress, int> addr2NodeId;
    std::unordered_map<cTopology::Node*, int> nodeID;

private:
    void collectNodes(cTopology* topo);
};
