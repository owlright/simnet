#include "UnicastSenderApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"
#include "simnet/mod/AggPacket_m.h"

class ATPWorker : public UnicastSenderApp
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
};

Define_Module(ATPWorker);

void ATPWorker::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        isUnicastSender = false;
        groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);
        if (groupManager==nullptr) // sometimes for quick debug
            EV_WARN << "You may forget to set groupManager." << endl;
    }
    else if (stage == INITSTAGE_ACCEPT) {
        if (groupManager==nullptr)
            throw cRuntimeError("WorkerApp::initialize: groupManager not found!");
        groupInfo = groupManager->getGroupHostInfo(localAddr);
        if (groupInfo != nullptr && groupInfo->isWorker) {
            workerId = groupInfo->index;
            jobId = groupInfo->hostinfo->jobId;
            EV << "host " << localAddr << " accept job " << jobId;
            EV << " possible PSes: ";
            EV << groupInfo->hostinfo->PSes << endl;
            // ! TODO FIXME only send to a single sever
            // ! Split Worker App from UnicastApp or Let UnicastApp have multiple Paramter servers ?
            destAddr = groupInfo->hostinfo->PSes.at(0);
        }
        else {
            setIdle();
            EV_WARN << "host " << localAddr << " have an idle ATPWorker" << endl;
        }
    }
    UnicastSenderApp::initialize(stage);
}

void ATPWorker::onFlowStart()
{
    UnicastSenderApp::onFlowStart();
    groupManager->reportFlowStart(jobId, simTime());
}

void ATPWorker::onFlowStop()
{
    EV_DEBUG << "round " << currentRound << " is finished." << endl;
    UnicastSenderApp::onFlowStop();
    groupManager->reportFlowStop(jobId, simTime());
}

Packet* ATPWorker::createDataPacket(SeqNumber seq, B packetBytes)
{
    // ASSERT(destAddr == -1); // destAddr is useless
    IntAddress dest = groupInfo->hostinfo->PSes.at(0); // TODO use more PSes
    char pkname[40];
    sprintf(pkname, " %lld-to-%lld-seq%lld",
            localAddr, dest, seq);
    auto pk = new ATPPacket(pkname);
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
    EV_DEBUG << "aggregator index: " << agtrIndex << endl;
    pk->setWorkerNumber(groupInfo->hostinfo->numWorkers);
    pk->setAggregatorIndex(agtrIndex);
    pk->setBitmap0(groupInfo->switchinfo->bitmap0);
    pk->setBitmap1(groupInfo->switchinfo->bitmap1);
    pk->setFanIndegree0(groupInfo->switchinfo->fanIndegree0);
    pk->setFanIndegree1(groupInfo->switchinfo->fanIndegree1);
    return pk;
}

void ATPWorker::finish()
{
    if (isIdle()) {
        EV << "this ATPWorker is idle, so delete it" << endl;
    } else {
        UnicastSenderApp::finish();
    }
}

class TimerWorker : public UnicastSenderApp
{
protected:
    virtual Packet* createDataPacket(SeqNumber seq, B packetBytes) override;
    virtual void initialize(int stage) override;

private:
    IntAddress groupAddr{INVALID_ADDRESS};
    int treeIndex{INVALID_ID};
    GlobalGroupManager* groupManager;

    int numSenders{0};
    intval_t dwellTime{0};
};

Define_Module(TimerWorker);

Packet *TimerWorker::createDataPacket(SeqNumber seq, B packetBytes)
{
    char pkname[40];
    sprintf(pkname, " %lld-to-%lld-seq%lld",
            localAddr, destAddr, sentBytes);
    auto pk = new MTATPPacket(pkname);
    pk->setKind(DATA);
    pk->setSeqNumber(seq);
    pk->setByteLength(packetBytes);
    pk->setECN(false);
    pk->setStartTime(simTime().dbl());
    pk->setTransmitTime(0);
    pk->setQueueTime(0);
    if (sentBytes == confirmedBytes)
        pk->setIsFlowFinished(true);

    pk->setAggCounter(0);
    pk->setWorkerNumber(numSenders);
    pk->setTimer(dwellTime); // in unit ns
    return pk;
}

void TimerWorker::initialize(int stage)
{

    if (stage == INITSTAGE_LOCAL) {
        isUnicastSender = false;
        groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);
        if (groupManager==nullptr)
            throw cRuntimeError("WorkerApp::initialize: groupManager not found!");
        auto t = par("initDwellTime").doubleValueInUnit("s");
        dwellTime = SimTime(t).inUnit(SIMTIME_NS);
    }
    // else if (stage == INITSTAGE_ASSIGN) {
    //     groupAddr = groupManager->getGroupAddress(localAddr);
    //     if (groupAddr > 0 && groupManager->getGroupRootAddress(groupAddr) != localAddr)
    //     {
    //         destAddr = groupAddr;
    //         treeIndex = groupManager->getTreeIndex(localAddr);
    //         EV << "(sender) groupAddr: " << groupAddr <<" localAddr:" << localAddr  << endl;
    //     }
    //     if (groupAddr > 0)
    //         numSenders = groupManager->getSendersNumber(groupAddr);
    // }
    UnicastSenderApp::initialize(stage);
}
