#include "WorkerApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"
#include "simnet/mod/AggPacket_m.h"

Define_Module(WorkerApp);

void WorkerApp::initialize(int stage)
{
    UnicastSenderApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        jobId = par("jobId");
        workerId = par("jobId");
        numWorkers = par("numWorkers");
        numRounds = par("numRounds");
        roundInterval = par("roundInterval").doubleValueInUnit("s");
    }
}

void WorkerApp::onFlowStart()
{
    currentRound += 1;
    sentBytes = 0;
    confirmedBytes = 0;
    currentBaseRTT = 0;
    confirmedDisorders.clear();
    EV_INFO << "current round seq: " << currentRound << endl;
    cong->reset();
    // groupManager->reportFlowStart(jobId, simTime());
}

void WorkerApp::onFlowStop()
{
    if (currentRound < numRounds) {// note it's '<' here
        scheduleAfter(roundInterval, flowStartTimer);
    }
    // groupManager->reportFlowStop(jobId, simTime());
}

Packet* WorkerApp::createDataPacket(SeqNumber seq, B packetBytes)
{
    char pkname[40];
    sprintf(pkname, "ATP-%" PRId64 "-to-%" PRId64 "-seq%" PRId64,
            localAddr, destAddr, seq);
    auto pk = new AggPacket(pkname);
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
    pk->setAggregatorIndex(agtrIndex);
    pk->setWorkerNumber(numWorkers);
    // EV_DEBUG << "aggregator index: " << agtrIndex << endl;
    // pk->setWorkerNumber(groupInfo->hostinfo->numWorkers);

    // pk->setBitmap0(groupInfo->switchinfo->bitmap0);
    // pk->setBitmap1(groupInfo->switchinfo->bitmap1);
    // pk->setFanIndegree0(groupInfo->switchinfo->fanIndegree0);
    // pk->setFanIndegree1(groupInfo->switchinfo->fanIndegree1);
    return pk;
}

void WorkerApp::finish()
{
    UnicastSenderApp::finish();
    if (currentRound != numRounds) {
        EV_WARN << getClassAndFullPath() << " " << localAddr << " complete " << currentRound << " rounds,  not reach " << numRounds << endl;
    }
}
