#pragma once
#include <omnetpp.h>
#include <unordered_map>
#include "simnet/common/Defs.h"
using namespace omnetpp;

class GlobalView : public cModule
{
public:
    virtual ~GlobalView();

protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return Stage::NUM_INIT_STAGES; }

protected:
    static cTopology *topo;
    static std::vector<int> hostNodes;
    static std::vector<IntAddress> jobUsedAddrs;
    // TODO rename the below two items
    static std::unordered_map<int, IntAddress> nodeId2addr;
    static std::unordered_map<IntAddress, int> addr2nodeId;
    static std::unordered_map<IntAddress, cModule*> addr2mod;

private:
    void collectNodes(cTopology* topo);
    static bool isInitialized;

};
