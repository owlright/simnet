#pragma once
#include <omnetpp.h>
#include <unordered_map>
#include "simnet/common/Defs.h"
using namespace omnetpp;

class GlobalView : public cSimpleModule
{
public:
    virtual ~GlobalView();

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage * msg) override
    {throw cRuntimeError("this module doesn't handle messages, it runs only in initialize()");};

protected:
    static cTopology *topo;
    static std::vector<int> hostNodes;
    // TODO rename the below two items
    static std::unordered_map<int, IntAddress> node2addr;
    static std::unordered_map<IntAddress, int> addr2node;
    
    static std::unordered_map<IntAddress, cModule*> addr2mod;

private:
    void collectNodes(cTopology* topo);
    static bool isInitialized;

};
