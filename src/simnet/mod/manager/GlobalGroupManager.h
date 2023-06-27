#pragma once

#include <omnetpp.h>
#include <tuple>
#include <memory>
#include "GlobalView.h"
using namespace omnetpp;

struct JobHostInfo
{
    uint16_t jobId;
    std::vector<int> PSes;
    std::vector<int> workers;
    std::vector<int> workerPorts;
    std::vector<int> PSPorts;
    int numWorkers;
    int numPSes;
    std::vector<int> multicastAddresses;
};

struct JobSwitchInfo
{
    // for manually set
    IntAddress switch0{-1};
    IntAddress switch1{-1};
    int fanIndegree0{-1};
    int fanIndegree1{-1};
    uint32_t bitmap0;
    uint32_t bitmap1;
};

struct JobSegmentsRoute // ! for segment routing aggregation
{
    std::vector<int> segmentAddrs;
    std::vector<int> fanIndegrees;
};

class GlobalGroupManager : public GlobalView
{
public:
    void reportFlowStart(IntAddress groupAddr, simtime_t roundStartTime);
    void reportFlowStop(IntAddress groupAddr, simtime_t roundStopTime);

protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

private:
    // void readSwitchConfig(const char * fileName);
    void readHostConfig(const char * fileName);
    simsignal_t createSignalForGroup(IntAddress group);
    int getCurrentJobId() const {return jobId;}
    int getCurrentGroupAddr() const {return groupAddress;}
    int getNextJobId() {return ++jobId;}
    int getNextGroupAddr() {return ++groupAddress;}

private:
    struct groupRoundFinishInfo {
        size_t counter{0};
        simtime_t startTime;
        simsignal_t roundFctSignal;
    };
    std::unordered_map<IntAddress, groupRoundFinishInfo*> groupRoundStartTime;

private:
    int jobId{0};
    int groupAddress{GROUPADDR_START};
    std::unordered_map<int, JobHostInfo*> jobInfodb;
    std::unordered_map<int,
                       std::unordered_map<
                           IntAddress,
                           std::unordered_map<IntAddress, JobSegmentsRoute *>>>
        segmentInfodb;

private:
    // for aggregation job
    void placeJobs(const char* policyName);
    void createJobApps(int jobId);
    void calcAggTree(const char* policyName);
    // * jobId is the job's index in jobInfodb
    void insertJobInfodb(const std::vector<int>& workers, const std::vector<int>& pses);
    void buildSteinerTree(cTopology& tree, const std::vector<int>& members, int root);
    // TODO make this function more clearly
    // ! add the shortest path between Node start and stop, note that only stop is in the tree
    void addShortestPath(cTopology& tree, cTopology::Node* start, cTopology::Node* stop);
    std::vector<IntAddress> getShortestPath(cTopology& tree, cTopology::Node* start, cTopology::Node* stop);
};

