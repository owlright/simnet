#include "WorkerApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/AggPacket_m.h"

Define_Module(WorkerApp);

void WorkerApp::initialize(int stage)
{
    FlowApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        jobId = par("jobId");
        workerId = par("workerId");
        numWorkers = par("numWorkers");
        numRounds = par("numRounds");
        roundInterval = par("roundInterval").doubleValueInUnit("s");
        jobMetricCollector = findModuleFromTopLevel<GlobalMetricCollector>("metricCollector", this);
        if (jobMetricCollector == nullptr)
            EV_WARN << "No job metrics will be collected." << endl;
    }
}

void WorkerApp::setField(AggPacket* pk) {
    pk->setAggSeqNumber(nextAggSeq++);
    pk->setRound(currentRound);
    pk->setJobId(jobId);
    pk->setWorkerNumber(numWorkers);
    pk->setRecordLen(1);
    pk->addRecord(localAddr);
}

void WorkerApp::onFlowStart() {
    FlowApp::onFlowStart();
    EV_INFO << "current round seq: " << currentRound << endl;
    if (jobMetricCollector)
        jobMetricCollector->reportFlowStart(jobId, numWorkers, workerId, simTime());
}

void WorkerApp::onFlowStop()
{
    if (currentRound < numRounds) {// note it's '<' here
        scheduleAfter(roundInterval, flowStartTimer);
        appState = Scheduled;
        nextAggSeq = 0;
    }
    if (jobMetricCollector)
        jobMetricCollector->reportFlowStop(jobId, numWorkers, workerId, simTime());
}

Packet* WorkerApp::createDataPacket(B packetBytes)
{
    // char pkname[40];
    // sprintf(pkname, "NOINC-%" PRId64 "-to-%" PRId64 "-seq%" PRId64,
    //         localAddr, destAddr, getNextSeq());
    auto pk = new AggPacket();
    setField(pk);

    return pk;
}

void WorkerApp::finish()
{
    CongApp::finish();
    if (currentRound != numRounds) {
        EV_WARN << getClassAndFullPath() << "job " << jobId << " "
                                        "address " << localAddr
                                        << " round " << currentRound << " not finish." << endl;

    }
}
