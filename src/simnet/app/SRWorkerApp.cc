#include "UnicastSenderApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"
#include "simnet/mod/AggPacket_m.h"

class SRWorker : public UnicastSenderApp
{
protected:
    void initialize(int stage) override;
    void onFlowStart() override;
    void onFlowStop() override;
    virtual Packet* createDataPacket(SeqNumber seq, B packetBytes) override;
    virtual void finish() override;

private:
    GlobalGroupManager* groupManager;
    const GroupInfoWithIndex* groupInfo;
    int jobId;
    int workerId{-1};
    std::vector<IntAddress> segments;
    std::vector<int> fanIndegrees;
};


Define_Module(SRWorker);

void SRWorker::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        isUnicastSender = false;
        groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);
        if (groupManager==nullptr) // sometimes for quick debug
            EV_WARN << "You may forget to set groupManager." << endl;
    } else if (stage == INITSTAGE_ASSIGN) {
        if (groupManager==nullptr)
            throw cRuntimeError("WorkerApp::initialize: groupManager not found!");
        groupInfo = groupManager->getGroupHostInfo(localAddr);
        if (groupInfo != nullptr && groupInfo->isWorker) {
            workerId = groupInfo->index;
            jobId = groupInfo->hostinfo->jobId;
            EV << "host " << localAddr << " accept job " << jobId;
            EV << " possible PSes: ";
            EV << groupInfo->hostinfo->PSes << endl;
            segments = groupInfo->segmentAddrs;
            fanIndegrees = groupInfo->fanIndegrees;
            EV << "sid: " << segments << endl;
            EV << "arg: " << fanIndegrees << endl;
            // ! TODO FIXME only send to a single sever
            // ! Split Worker App from UnicastApp or Let UnicastApp have multiple Paramter servers ?
            destAddr = groupInfo->hostinfo->PSes.at(0);
        }
        else {
            setIdle();
            EV_WARN << "host " << localAddr << " have an idle SRWorker" << endl;
        }
    }
    UnicastSenderApp::initialize(stage);
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
    // ASSERT(destAddr == -1); // destAddr is useless
    IntAddress dest = groupInfo->hostinfo->PSes.at(0); // TODO use more PSes
    char pkname[40];
    sprintf(pkname, " sr%lld-to-%lld-seq%lld",
            localAddr, dest, seq);
    auto pk = new SRAggPacket(pkname);
    pk->setRound(currentRound);
    pk->setJobId(jobId);
    pk->setDestAddr(dest);
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
    pk->setWorkerNumber(groupInfo->hostinfo->numWorkers);

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
    if (isIdle()) {
        EV << "this SRWorker is idle, so delete it" << endl;
    } else {
        UnicastSenderApp::finish();
    }
}
