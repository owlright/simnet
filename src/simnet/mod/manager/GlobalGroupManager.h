#pragma once

#include <omnetpp.h>
#include <tuple>
#include <unordered_map>
#include "simnet/common/Defs.h"

using namespace omnetpp;

class GlobalGroupManager : public cSimpleModule
{
public:
    // for switch node use
    IntAddress getGroupAddress(IntAddress fromNode) const;
    IntAddress getGroupRootAddress(IntAddress groupAddr) const;
    // for host node use
    int getTreeIndex(IntAddress fromNode) const;
    int getFanIndegree(IntAddress group, int treeIndex, IntAddress switchAddress) const;
    int getBufferSize(IntAddress group, IntAddress switchAddress) const;
    // for signals collection
    void reportFlowStart(IntAddress groupAddr, simtime_t roundStartTime);
    void reportFlowStop(IntAddress groupAddr, simtime_t roundStopTime);

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override
        {throw cRuntimeError("this module doesn't handle messages, it runs only in initialize()");};
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

private:
    void readSwitchConfig(const char * fileName);
    void readHostConfig(const char * fileName);
    simsignal_t createSignalForGroup(IntAddress group);

private:
    struct hashFunctionInt2
    {
        size_t operator()(const std::pair<IntAddress , int64_t> &x) const{
            return x.first ^ x.second;
        }
    };
    struct hashFunctionInt3
    {
        size_t operator()(const std::tuple<IntAddress, IntAddress, int64_t> &x) const{
            return std::get<0>(x) ^ std::get<1>(x) ^ std::get<2>(x);
        }
    };
    std::unordered_map<IntAddress, std::vector<int64_t> > hostGroupInfo;
    std::unordered_map<std::pair<IntAddress, int64_t>, IntAddress, hashFunctionInt2> groupRoot;
    std::unordered_map<std::pair<IntAddress, int64_t>, std::vector<int64_t>, hashFunctionInt2> groupSources;
    std::unordered_map<std::tuple<IntAddress, IntAddress, int64_t>, int, hashFunctionInt3> switchFanIndegree;
    std::unordered_map<std::pair<IntAddress, IntAddress>, B, hashFunctionInt2> switchBufferSize;

    struct groupRoundFinishInfo {
        int counter{0};
        simtime_t startTime;
        simsignal_t roundFctSignal;
    };
    std::unordered_map<IntAddress, groupRoundFinishInfo*> groupRoundStartTime;

private:
    // for aggregation job
    void prepareAggGroup(const char* policyName);
};

