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
    const auto& getAggTrees(int jobId) const {return aggTrees[jobId-1];}
    const auto& getAggNodes(int jobId) const {return aggNodes[jobId-1];}

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
    vector<vector<simnet::Graph>> aggTrees;
    vector<vector<map<int, vector<int>>>> aggNodes;

private:
    // for aggregation job
    void placeJobs(const char* policyName);
    void createJobApps(int jobId);
    void calcAggTree(const char* policyName);
    // * jobId is the job's index in jobInfodb
    void insertJobInfodb(const std::vector<IntAddress>& workers, const std::vector<IntAddress>& pses);
};
