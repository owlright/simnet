#pragma once

#include <omnetpp.h>
#include <tuple>
#include "GlobalView.h"
using namespace omnetpp;

struct GroupHostInfo
{
    uint16_t jobId;
    IntAddress groupAddress;
    std::vector<int> PSes;
    std::vector<int> workers;
    int numWorkers;
    int numPSes;
};

struct GroupSwitchInfo
{
    // for manually set
    std::vector<IntAddress> switchAddrs;
    std::vector<B> bufferSizes;
};

class GlobalGroupManager : public GlobalView
{
public:
    // for switch node use
    IntAddress getGroupAddress(IntAddress fromNode) const;
    IntAddress getGroupRootAddress(IntAddress groupAddr) const;
    int getSendersNumber(IntAddress groupAddr) const;
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
    std::unordered_map<IntAddress, GroupHostInfo*> hostGroupInfo;
    std::unordered_map<IntAddress, GroupSwitchInfo*> switchGroupInfo;

    struct groupRoundFinishInfo {
        size_t counter{0};
        simtime_t startTime;
        simsignal_t roundFctSignal;
    };
    std::unordered_map<IntAddress, groupRoundFinishInfo*> groupRoundStartTime;
private:
    std::vector<GroupHostInfo*> groupHostInfodb;
    std::vector<GroupSwitchInfo*> GroupSwitchInfodb;
private:
    // for aggregation job
    void prepareAggGroup(const char* policyName);
    void buildSteinerTree(cTopology& tree, const std::vector<int>& members, int root);
    // TODO make this function more clearly
    // ! add the shortest path between Node start and stop, note that only stop is in the tree
    void addShortestPath(cTopology& tree, cTopology::Node* start, cTopology::Node* stop);
};

