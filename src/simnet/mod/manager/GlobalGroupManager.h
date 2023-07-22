#pragma once

#include <omnetpp.h>
#include <unordered_map>
#include "simnet/common/Defs.h"
#include "simnet/common/utils.h"
#include "GlobalManager.h"

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

class GlobalGroupManager : public GlobalManager
{
protected:
    virtual void initialize(int stage) override;

private:
    // void readSwitchConfig(const char * fileName);
    void readHostConfig(const char * fileName);
    int getCurrentJobId() const {return jobId;}
    int getCurrentGroupAddr() const {return groupAddress;}
    int getNextJobId() {return ++jobId;}
    int getNextGroupAddr() {return ++groupAddress;}

private:
    int jobId{0};
    int groupAddress{GROUPADDR_START};
    std::map<int, JobHostInfo*> jobInfodb;
    std::unordered_map<int,
                       std::unordered_map<
                           IntAddress,
                           std::unordered_map<IntAddress, JobSegmentsRoute *>>>
        segmentInfodb;
    const char* placementPolicy;
    const char* aggTreeType;
    bool useInc{false};

private:
    // for aggregation job
    void placeJobs(const char* policyName);
    void createJobApps(int jobId);
    void calcAggTree(const char* policyName);
    // * jobId is the job's index in jobInfodb
    void insertJobInfodb(const std::vector<int>& workers, const std::vector<int>& pses);
    /* This function modifies the variable "tree" in-place to generate
       a Steiner tree based on "members" and "root".
       Note that members and root are both the indexes in GlobalView::topo
    */
    void buildSteinerTree(cTopology& tree, const std::vector<cTopology::Node*>& leaves, cTopology::Node* root);
    // TODO make this function more clearly
    // ! add the shortest path between Node start and stop, note that only stop is in the tree
    void addShortestPath(cTopology& tree, cTopology::Node* start, cTopology::Node* stop);
    std::vector<IntAddress> getShortestPath(cTopology& tree, cTopology::Node* start, cTopology::Node* stop);
};

