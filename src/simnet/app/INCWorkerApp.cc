#include "WorkerApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/common/utils.h"
#include <functional>
class INCWorker : public WorkerApp
{
protected:
    void initialize(int stage) override;
    virtual void setField(AggPacket* pk) override;

private:
    std::vector<std::vector<IntAddress>> segments;
    std::vector<int> fanIndegrees;
};


Define_Module(INCWorker);

void INCWorker::initialize(int stage)
{
    WorkerApp::initialize(stage);
    if (stage == INITSTAGE_LAST) {
        EV << "INCWorker(" << localAddr << ":" << localPort << ") accept job " << jobId;
        EV << " PS(" << destAddr << ":" << destPort << ")" << endl;
        auto segmentAddrs = cStringTokenizer(par("segmentAddrs").stringValue(), " ").asVector();
        segments.resize(segmentAddrs.size());
        for (auto i = 0; i < segmentAddrs.size(); i++) {
            auto tmp = cStringTokenizer(segmentAddrs[i].c_str(), "[,]").asIntVector();
            std::vector<IntAddress> equal_agg_addrs;
            for_each(tmp.begin(), tmp.end(), [&equal_agg_addrs](int& n){equal_agg_addrs.push_back(n);});
            segments[i].insert(segments[i].end(), equal_agg_addrs.begin(), equal_agg_addrs.end());
        }
        fanIndegrees = cStringTokenizer(par("fanIndegrees").stringValue()).asIntVector();
        // * easy debug
        std::vector<IntAddress> tmp{destAddr};
        segments.push_back(tmp);
        fanIndegrees.push_back(numWorkers);
        EV << "sid: " << segments << endl;
        EV << "arg: " << fanIndegrees << endl;
    }

}

void INCWorker::setField(AggPacket* pk)
{
    WorkerApp::setField(pk); // ! we need the aggSeqNumber
    pk->setAggPolicy(INC);
    auto seqNumber = pk->getAggSeqNumber();
    auto jobID = pk->getJobId();

    std::size_t agtrIndex = seqNumber ^ jobID;
    hash_combine(agtrIndex, jobID);
    hash_combine(agtrIndex, seqNumber);

    EV_DEBUG << "aggSeqNumber " << seqNumber << " jobID " << jobID << " aggregator: " << agtrIndex % MAX_AGTR_COUNT << endl;
    pk->setAggregatorIndex(agtrIndex % MAX_AGTR_COUNT);
    // segment routing
    pk->setSIDSize(segments.size());
    pk->setLastEntry(segments.size() - 1);
    pk->setSegmentsLeft(segments.size());
    auto sit = segments.rbegin();
    auto fit = fanIndegrees.rbegin();
    for (auto i = 0; i < segments.size(); i++)
    {
        pk->setSegments(i, (*sit)[agtrIndex % sit->size()]); // TODO: maybe a hash function with probability is better
        sit++;
        pk->setFuns(i, "aggregation");
        auto indegree = std::to_string(*fit++);
        pk->setArgs(i, indegree.c_str());
    }
}
