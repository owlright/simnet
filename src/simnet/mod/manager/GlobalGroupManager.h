#pragma once

#include <omnetpp.h>
#include <unordered_map>
#include "simnet/common/Defs.h"
#include "simnet/common/utils.h"
#include "GlobalManager.h"

using namespace omnetpp;
using std::vector;
struct JobHostInfo
{
    uint16_t jobId;
    std::vector<IntAddress> PSes;
    std::vector<IntAddress> workers;
    std::vector<PortNumber> workerPorts;
    std::vector<PortNumber> PSPorts;
    int numWorkers;
    int numPSes;
    std::vector<IntAddress> multicastAddresses;
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
    void insertJobInfodb(const std::vector<IntAddress>& workers, const std::vector<IntAddress>& pses);
    /* This function modifies the variable "tree" in-place to generate
       a Steiner tree based on "members" and "root".
       Note that members and root are both the indexes in GlobalView::topo
    */
    cTopology* buildSteinerTree(const vector<IntAddress>& leaves, const IntAddress& root);
    // TODO make this function more clearly
    // ! add the shortest path between Node start and stop, note that only stop is in the tree
    void addShortestPath(cTopology* tree, cTopology::Node* start, cTopology::Node* stop);
    std::vector<IntAddress> getShortestPath(cTopology* tree, cTopology::Node* start, cTopology::Node* stop);
    void addCostFrom(const cTopology* tree);
};

