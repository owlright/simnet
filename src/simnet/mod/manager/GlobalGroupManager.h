#pragma once

#include <omnetpp.h>
#include "simnet/common/Defs.h"
#include "simnet/mod/agroup/AggGroupEntry.h"
using namespace omnetpp;

class GlobalGroupManager : public cSimpleModule
{
public:
    // ! for switch use
    AggGroupEntry* getGroupEntry(IntAddress group);
    // ! for host use
    IntAddress getGroupAddress(IntAddress fromNode) const;
    int getTreeIndex(IntAddress fromNode) const;

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override
        {throw cRuntimeError("this module doesn't handle messages, it runs only in initialize()");};
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

private:
    void readSwitchConfig(const char * fileName);
    void readHostConfig(const char * fileName);

private:
    struct hashFunction
    {
        size_t operator()(const std::pair<IntAddress , int64_t> &x) const{
            return x.first ^ x.second;
        }
    };
    std::unordered_map<IntAddress, std::vector<int64_t> > hostGroupInfo;
    std::unordered_map<std::pair<IntAddress, int64_t>, IntAddress, hashFunction> groupRoot;
    std::unordered_map<std::pair<IntAddress, int64_t>, std::vector<int64_t>, hashFunction> groupSources;

};

