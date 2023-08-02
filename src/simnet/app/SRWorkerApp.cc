#include "WorkerApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/AggPacket_m.h"
#include "simnet/common/utils.h"
#include <functional>
class SRWorker : public WorkerApp
{
protected:
    void initialize(int stage) override;
    virtual Packet* createDataPacket(SeqNumber seq, B packetBytes) override;
    virtual void connectionDataArrived(Connection *connection, cMessage *msg) override;

private:
    std::vector<std::vector<IntAddress>> segments;
    std::vector<int> fanIndegrees;
};


Define_Module(SRWorker);

void SRWorker::initialize(int stage)
{
    WorkerApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
    }
    else if (stage == INITSTAGE_LAST) {
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


Packet* SRWorker::createDataPacket(SeqNumber seq, B packetBytes)
{
    char pkname[40];
    sprintf(pkname, "sr%" PRId64"-to-%" PRId64 "-seq%" PRId64,
            localAddr, destAddr, seq);
    // if (seq == 392000 && jobId == 4 && currentRound >= 4)
    //     std::cout << localAddr << " " << currentRound << endl;
    auto pk = new AggUseIncPacket(pkname);
    pk->setRound(currentRound);
    pk->setJobId(jobId);
    pk->setDestAddr(destAddr);
    pk->setSeqNumber(seq);
    pk->setByteLength(packetBytes);
    pk->setECN(false);
//    if (seq==500000)
//        std::cout << localAddr << simTime() << endl;
    // some cheating fields
    pk->setRecordLen(1);
    pk->addRecord(localAddr);
    pk->setStartTime(simTime().dbl());
    pk->setTransmitTime(0);
    pk->setQueueTime(0);
    if (sentBytes == confirmedNormalBytes)
        pk->setIsFlowFinished(true);

    auto seqNumber = pk->getSeqNumber();
    auto jobID = pk->getJobId();
    // if (seq < sentBytes) {
    //     pk->setResend(true);
    // }
    // I left this for future debuging
    //  if (jobId == 4 && seq >= 500000)
    //     std::cout << localAddr << " round " << currentRound << " will send out " << pk->getSeqNumber() << endl;
    // TODO avoid overflow
    // auto hseq = reinterpret_cast<uint16_t&>(seqNumber);
    // auto hjobid = reinterpret_cast<uint16_t&>(jobID);
    // auto agtrIndex = hashAggrIndex(hjobid, hseq);
    std::size_t agtrIndex = seqNumber ^ jobID;
    hash_combine(agtrIndex, jobID);
    hash_combine(agtrIndex, seqNumber);
//    EV_DEBUG << "aggregator index: " << agtrIndex << endl;
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
    return pk;
}

void SRWorker::connectionDataArrived(Connection *connection, cMessage *msg)
{
    // I left this for future debuging
    //  auto pk = check_and_cast<AggPacket*>(msg);
    //  auto seq = pk->getSeqNumber();
    //  if (jobId == 4 && seq >=500000) {
    //      std::cout << localAddr << " received ack"
    //                << seq << " "
    //                << pk->getRound() << " "
    //                << confirmedNormalBytes << endl;
    //  }
    WorkerApp::connectionDataArrived(connection, msg);

}

