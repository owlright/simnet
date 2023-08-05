#include "WorkerApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/common/utils.h"
#include <functional>
class SRWorker : public WorkerApp
{
protected:
    void initialize(int stage) override;
    Packet* createDataPacket(B packetBytes) override;
    void setField(AggUseIncPacket* pk);

private:
    std::vector<std::vector<IntAddress>> segments;
    std::vector<int> fanIndegrees;
};


Define_Module(SRWorker);

void SRWorker::initialize(int stage)
{
    WorkerApp::initialize(stage);
    if (stage == INITSTAGE_LAST) {
        EV << "SRWorker(" << localAddr << ":" << localPort << ") accept job " << jobId;
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
        EV << "sid: " << segments << endl;
        EV << "arg: " << fanIndegrees << endl;
    }

}

Packet* SRWorker::createDataPacket(B packetBytes)
{
    char pkname[40];
    sprintf(pkname, "sr%" PRId64"-to-%" PRId64 "-seq%" PRId64,
            localAddr, destAddr, getNextSeq());
    auto pk = new AggUseIncPacket(pkname);
    pk->setByteLength(messageLength);
    setField(pk);
    return pk;
}

void SRWorker::setField(AggUseIncPacket* pk)
{
    WorkerApp::setField(pk);
    auto seqNumber = pk->getSeqNumber();
    auto jobID = pk->getJobId();

    std::size_t agtrIndex = seqNumber ^ jobID;
    hash_combine(agtrIndex, jobID);
    hash_combine(agtrIndex, seqNumber);

    EV_DEBUG << "seqNumber " << seqNumber << " jobID " << jobID << " aggregator: " << agtrIndex % MAX_AGTR_COUNT << endl;
    pk->setAggregatorIndex(agtrIndex % MAX_AGTR_COUNT);
    pk->setWorkerNumber(numWorkers);
    pk->setDistance(0);
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
