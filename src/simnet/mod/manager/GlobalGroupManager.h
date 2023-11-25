#pragma once
#include "GlobalManager.h"
#include "simnet/common/Defs.h"
#include "simnet/common/utils.h"
#include <memory>
#include <omnetpp.h>
#include <unordered_map>

using namespace omnetpp;
using std::make_shared;
using std::shared_ptr;
using std::vector;
struct JobHostInfo {
    uint16_t jobId;
    std::vector<IntAddress> PSes;
    std::vector<IntAddress> workers;
    std::vector<PortNumber> workerPorts;
    std::vector<PortNumber> PSPorts;
    int numWorkers;
    int numPSes;
    std::vector<IntAddress> multicastAddresses;
};

struct JobSwitchInfo {
    // for manually set
    IntAddress switch0 { -1 };
    IntAddress switch1 { -1 };
    int fanIndegree0 { -1 };
    int fanIndegree1 { -1 };
    uint32_t bitmap0;
    uint32_t bitmap1;
};

struct JobSegmentsRoute // ! for segment routing aggregation
{
    std::vector<vector<IntAddress>> segmentAddrs;
    std::vector<int> fanIndegrees;
};

class GlobalGroupManager : public GlobalManager {
public:
    const std::vector<IntAddress>& getUnicastHosts() const { return unicastHosts; }

protected:
    virtual void initialize(int stage) override;

private:
    // void readSwitchConfig(const char * fileName);
    void readHostConfig(const char* fileName);
    int getCurrentJobId() const { return jobId; }
    int getCurrentGroupAddr() const { return groupAddress; }
    int getNextJobId() { return ++jobId; }
    int getNextGroupAddr() { return ++groupAddress; }

private:
    std::vector<IntAddress> unicastHosts;
    int jobId { 0 };
    int groupAddress { GROUPADDR_START };
    std::map<int, shared_ptr<JobHostInfo>> jobInfodb;
    std::unordered_map<int,
        std::unordered_map<IntAddress, std::unordered_map<IntAddress, shared_ptr<JobSegmentsRoute>>>>
        segmentInfodb;
    const char* placementPolicy;
    const char* aggTreeType;
    bool useInc { false };
    simnet::Graph network;

private:
    // for aggregation job
    void placeJobs(const char* policyName);
    void createJobApps(int jobId);
    void calcAggTree(const char* policyName);
    // * jobId is the job's index in jobInfodb
    void insertJobInfodb(const std::vector<IntAddress>& workers, const std::vector<IntAddress>& pses);
    /* Return an aggregation tree which in this case is
       a Steiner tree based on "members" and "root". indegree >= 2 will be push into aggNodes
    */
    cTopology* buildSteinerTree(const vector<IntAddress>& leaves, const IntAddress& root, vector<IntAddress>& aggNodes);
    std::unordered_map<IntAddress, vector<IntAddress>> findEqualCostAggNodes(
        const cTopology* tree, vector<IntAddress>& aggNodes, double costThreshold);
    // TODO make this function more clearly
    // ! add the shortest path between Node start and stop, note that only stop is in the tree
    void addShortestPath(cTopology* tree, cTopology::Node* start, cTopology::Node* stop);
    std::vector<IntAddress> getShortestPath(cTopology* tree, cTopology::Node* start, cTopology::Node* stop);
    void addCost(const cTopology* tree, double node_cost, double edge_cost);
};
