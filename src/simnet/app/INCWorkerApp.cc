#include "WorkerApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/common/utils.h"
#include "simnet/graph/algorithms.h"
#include "simnet/mod/manager/GlobalGroupManager.h"
#include <functional>
using namespace simnet::algorithms;
class INCWorker : public WorkerApp {
protected:
    void initialize(int stage) override;
    virtual void setField(AggPacket* pk) override;
    virtual void onReceivedNewPacket(Packet* pk) override;

private:
    void insertAgtrIndex(AggPacket* pk);

private:
    bool useAgtrIndex { false };
    std::unordered_set<std::size_t> usedAgtrIndex;
    std::vector<std::vector<IntAddress>> segments;
    std::vector<vector<int>> fanIndegrees;
    int maxAgtrCount { 0 };
    opp_component_ptr<GlobalGroupManager> groupManager;
};

Define_Module(INCWorker);

void INCWorker::initialize(int stage)
{
    WorkerApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);
        ASSERT(groupManager);
        maxAgtrCount = par("maxAgtrNum");
        useAgtrIndex = maxAgtrCount > 0;
        EV_TRACE << "INCWorker(" << localAddr << ":" << localPort << ") accept job " << jobId;
        EV_TRACE << " PS(" << destAddr << ":" << destPort << ")" << endl;
    } else if (stage == INITSTAGE_ACCEPT) {
        auto& aggTrees = groupManager->getAggTrees();
        auto getAddr
            = std::bind(static_cast<IntAddress (GlobalGroupManager::*)(int) const>(&GlobalGroupManager::getAddr),
                this->groupManager, std::placeholders::_1);
        auto getNodeId
            = std::bind(static_cast<int (GlobalGroupManager::*)(IntAddress) const>(&GlobalGroupManager::getNodeId),
                this->groupManager, std::placeholders::_1);
        for (auto i = 0; i < aggTrees.size(); i++) {
            auto& t = aggTrees[i];
            vector<int> p;
            dijistra(t, getNodeId(localAddr), getNodeId(destAddr), &p);
            vector<IntAddress> path;
            vector<int> ind;
            for (auto i = 1; i < p.size(); i++) {
                if (t.indegree(p[i]) >= 2) {
                    path.push_back(getAddr(p[i]));
                    ind.push_back(t.indegree(p[i]));
                }
            }
            path.push_back(destAddr); // no use, easy debugging
            ind.push_back(numWorkers); // no use, easy debugging
            segments.push_back(path);
            fanIndegrees.push_back(ind);
        }
        EV_DEBUG << localAddr << endl;
        EV_DEBUG << segments << endl;
        EV_DEBUG << fanIndegrees << endl;
        EV_DEBUG << endl;
        ASSERT(segments.size() == fanIndegrees.size());
    }
}

void INCWorker::setField(AggPacket* pk)
{
    WorkerApp::setField(pk); // ! we need the aggSeqNumber
    pk->setAggPolicy(INC);
    if (useAgtrIndex)
        insertAgtrIndex(pk);
    auto seqNumber = pk->getAggSeqNumber();
    // auto jobID = pk->getJobId(); // unused-variable
    // segment routing
    int numPaths = segments.size();
    ASSERT(numPaths > 0);
    auto chooseIndex = seqNumber % numPaths;
    auto& sit = segments[chooseIndex];
    auto& ind = fanIndegrees[chooseIndex];
    ASSERT(sit.size() == ind.size());
    pk->setSIDSize(sit.size());
    pk->setLastEntry(sit.size()-1);
    pk->setSegmentsLeft(sit.size()); // we include the ps addr
    auto segSize = sit.size();
    for (auto i = 0; i < segSize; i++) {
        pk->setSegments(i, sit[segSize-i-1]);
        pk->setFuns(i, "aggregation");
        pk->setArgs(i, std::to_string(ind[segSize-i-1]).c_str());
    }
}

void INCWorker::onReceivedNewPacket(Packet* pk)
{
    auto apk = check_and_cast<AggPacket*>(pk);
    auto agtrIndex = apk->getAggregatorIndex();
    usedAgtrIndex.erase(agtrIndex); // save memory
    WorkerApp::onReceivedNewPacket(pk);
}

void INCWorker::insertAgtrIndex(AggPacket* pk)
{
    auto seqNumber = pk->getAggSeqNumber();
    auto jobID = pk->getJobId();

    std::size_t agtrIndex = seqNumber ^ jobID;
    hash_combine(agtrIndex, jobID);
    hash_combine(agtrIndex, seqNumber);
    agtrIndex = agtrIndex % maxAgtrCount;
    while (usedAgtrIndex.find(agtrIndex) != usedAgtrIndex.end()) {
        agtrIndex = (agtrIndex + 1) % maxAgtrCount; // ! WARN may endless
    }
    usedAgtrIndex.insert(agtrIndex);
    EV_DEBUG << "aggSeqNumber " << seqNumber << " jobID " << jobID << " aggregator: " << agtrIndex << endl;
    pk->setAggregatorIndex(agtrIndex);
}
