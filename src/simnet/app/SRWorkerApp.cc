#include "WorkerApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"
#include "simnet/mod/AggPacket_m.h"

class SRWorker : public WorkerApp
{
protected:
    void initialize(int stage) override;
    virtual void onFlowStart() override;
    virtual void onFlowStop() override;
    virtual Packet* createDataPacket(SeqNumber seq, B packetBytes) override;
    virtual void finish() override;

private:
    GlobalGroupManager* groupManager;
    std::vector<int> segments;
    std::vector<int> fanIndegrees;
};


Define_Module(SRWorker);

void SRWorker::initialize(int stage)
{
    WorkerApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL || stage == INITSTAGE_ACCEPT) {
//        destAddr = par("destAddress");
        segments = cStringTokenizer(par("segmentAddrs").stringValue()).asIntVector();
        fanIndegrees = cStringTokenizer(par("fanIndegrees").stringValue()).asIntVector();
        groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);


        if (groupManager == nullptr) // sometimes for quick debug
            EV_WARN << "You may forget to set groupManager." << endl;

        EV << "SRWorker(" << localAddr << ":" << localPort << ") accept job " << jobId;
        EV << " PS(" << destAddr << ":" << destPort << ")" << endl;

        EV << "sid: " << segments << endl;
        EV << "arg: " << fanIndegrees << endl;

    }

}

void SRWorker::onFlowStart()
{
    UnicastSenderApp::onFlowStart();
    groupManager->reportFlowStart(jobId, simTime());
}

void SRWorker::onFlowStop()
{
    EV_DEBUG << "round " << currentRound << " is finished." << endl;
    UnicastSenderApp::onFlowStop();
    groupManager->reportFlowStop(jobId, simTime());
}

Packet* SRWorker::createDataPacket(SeqNumber seq, B packetBytes)
{
    char pkname[40];
    sprintf(pkname, "sr%" PRId64"-to-%" PRId64 "-seq%" PRId64,
            localAddr, destAddr, seq);
    auto pk = new SRAggPacket(pkname);
    pk->setRound(currentRound);
    pk->setJobId(jobId);
    pk->setDestAddr(destAddr);
    pk->setSeqNumber(seq);
    pk->setByteLength(packetBytes);
    pk->setECN(false);
    // some cheating fields
    pk->setRecordLen(1);
    pk->addRecord(localAddr);
    pk->setStartTime(simTime().dbl());
    pk->setTransmitTime(0);
    pk->setQueueTime(0);
    if (sentBytes == confirmedBytes)
        pk->setIsFlowFinished(true);

    auto seqNumber = pk->getSeqNumber();
    auto jobID = pk->getJobId();
    if (seq < sentBytes)
        pk->setResend(true);
    // TODO avoid overflow
    auto hseq = reinterpret_cast<uint16_t&>(seqNumber);
    auto hjobid = reinterpret_cast<uint16_t&>(jobID);
    auto agtrIndex = hashAggrIndex(hjobid, hseq);
//    EV_DEBUG << "aggregator index: " << agtrIndex << endl;
    pk->setAggregatorIndex(agtrIndex);
    pk->setWorkerNumber(numWorkers);

    // segment routing
    pk->setSIDSize(segments.size());
    pk->setLastEntry(segments.size() - 1);
    auto sit = segments.rbegin();
    auto fit = fanIndegrees.rbegin();
    for (auto i = 0; i < segments.size(); i++)
    {
        pk->setSegments(i, *sit++);
        pk->setFuns(i, "aggregation");
        auto indegree = std::to_string(*fit++);
        pk->setArgs(i, indegree.c_str());
    }
    return pk;
}

void SRWorker::finish()
{
    UnicastSenderApp::finish();
}

