#include "WorkerApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/common/utils.h"
#include <functional>
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
    std::vector<int> fanIndegrees;
    int maxAgtrCount { 0 };
};

Define_Module(INCWorker);

void INCWorker::initialize(int stage)
{
    WorkerApp::initialize(stage);
    if (stage == INITSTAGE_LAST) {
        maxAgtrCount = par("maxAgtrNum");
        useAgtrIndex = maxAgtrCount > 0;
        EV_TRACE << "INCWorker(" << localAddr << ":" << localPort << ") accept job " << jobId;
        EV_TRACE << " PS(" << destAddr << ":" << destPort << ")" << endl;
        auto segmentAddrs = cStringTokenizer(par("segmentAddrs").stringValue(), " ").asVector();
        segments.resize(segmentAddrs.size());
        for (auto i = 0; i < segmentAddrs.size(); i++) {
            auto equal_agg_addrs = cStringTokenizer(segmentAddrs[i].c_str(), "[,]").asIntVector();
            segments[i].insert(segments[i].end(), equal_agg_addrs.begin(), equal_agg_addrs.end());
        }
        fanIndegrees = cStringTokenizer(par("fanIndegrees").stringValue()).asIntVector();
        // * easy debug
        std::vector<IntAddress> tmp { destAddr };
        segments.push_back(tmp);
        fanIndegrees.push_back(numWorkers);
        EV_TRACE << "sid: " << segments << endl;
        EV_TRACE << "indegree: " << fanIndegrees << endl;
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
    pk->setSIDSize(segments.size());
    pk->setLastEntry(segments.size() - 1);
    pk->setSegmentsLeft(segments.size());
    auto sit = segments.rbegin();
    auto fit = fanIndegrees.rbegin();
    for (auto i = 0; i < segments.size(); i++) {
        if (sit->size() > 1) {
            pk->setSegments(i, (*sit)[seqNumber % sit->size()]); // TODO: agg trees balance the computation nodes, but
                                                                 // may cause disorder
        } else
            pk->setSegments(i, (*sit).at(0));
        sit++;
        pk->setFuns(i, "aggregation");
        auto indegree = std::to_string(*fit++);
        pk->setArgs(i, indegree.c_str());
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
