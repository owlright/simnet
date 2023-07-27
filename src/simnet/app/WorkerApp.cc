#include "WorkerApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/AggPacket_m.h"

Define_Module(WorkerApp);

void WorkerApp::initialize(int stage)
{
    UnicastSenderApp::initialize(stage);
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

void WorkerApp::onFlowStart()
{
    UnicastSenderApp::onFlowStart();
    currentRound += 1;
    EV_INFO << "current round seq: " << currentRound << endl;
    if (jobMetricCollector)
        jobMetricCollector->reportFlowStart(jobId, numWorkers, workerId, simTime());
}

void WorkerApp::onFlowStop()
{
    if (currentRound < numRounds) {// note it's '<' here
        scheduleAfter(roundInterval, flowStartTimer);
    }
    if (jobMetricCollector)
        jobMetricCollector->reportFlowStop(jobId, numWorkers, workerId, simTime());
}

Packet* WorkerApp::createDataPacket(SeqNumber seq, B packetBytes)
{
    char pkname[40];
    sprintf(pkname, "NOINC-%" PRId64 "-to-%" PRId64 "-seq%" PRId64,
            localAddr, destAddr, seq);
    auto pk = new AggPacket(pkname);
    pk->setRound(currentRound);
    pk->setJobId(jobId);
    pk->setDestAddr(destAddr);
    pk->setWorkerNumber(numWorkers);

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

    if (seq < sentBytes) {
        throw cRuntimeError("This is a no-inc agg packet, should not have resend packets");
    }
    return pk;
}

void WorkerApp::finish()
{
    UnicastSenderApp::finish();
    if (currentRound != numRounds) {
        EV_WARN << getClassAndFullPath() << "job " << jobId << " "
                                        "address " << localAddr
                                        << " complete " << currentRound << " rounds,  not reach " << numRounds << endl;
    }
}
