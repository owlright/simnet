#pragma once
#include <omnetpp.h>
#include <unordered_map>
#include "simnet/common/Defs.h"
using std::vector;
template <typename T>
using Dict = std::unordered_map<T, T>;

template <typename T>
using Mat = std::vector<std::vector<T>>;

using namespace omnetpp;
class GlobalView : public cModule
{
public:
    cTopology* getGlobalTopo() const {return topo;}
    const std::vector<int> gethostIds() const {return hostNodes;}
    std::unordered_map<int, IntAddress> getNodeIdAddrMap() const {return nodeId2Addr;}
    std::unordered_map<IntAddress, int> getAddrNodeIdMap() const {return addr2NodeId;}
    Mat<double> getDistMat() const {return topoDist;}
    Mat<double> getCostAdj() const {return costAdj;}
    ~GlobalView() { delete topo; topo=nullptr;}
    Dict<int> primMST(const vector<int>& S, const vector<int>& mstnodes, const Mat<double>& oddist);

protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return Stage::NUM_INIT_STAGES; }

private:
    cTopology* topo{nullptr};
    Mat<double> topoDist;
    Mat<double> costAdj;
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
