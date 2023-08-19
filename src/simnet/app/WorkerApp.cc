#include "WorkerApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/AggPacket_m.h"

Define_Module(WorkerApp);

void WorkerApp::initialize(int stage)
{
    CongApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        flowSize = par("flowSize");
        jobId = par("jobId");
        workerId = par("workerId");
        numWorkers = par("numWorkers");
        numRounds = par("numRounds");
        roundInterval = par("roundInterval").doubleValueInUnit("s");
        roundStartTimer = new cMessage("roundStart");
        scheduleAt(simTime(), roundStartTimer);
        ASSERT(tcpState == CLOSED);
        tcpState = OPEN;
        jobMetricCollector = findModuleFromTopLevel<GlobalMetricCollector>("metricCollector", this);
        if (jobMetricCollector == nullptr)
            EV_WARN << "No job metrics will be collected." << endl;
    }
}

void WorkerApp::handleMessage(cMessage* msg)
{
    if (msg == roundStartTimer) { // new flow
        onRoundStart();
        sendPendingData();
        if (!getEnvir()->isExpressMode()) {
            char mesg[20];
            sprintf(mesg, "Round %d", currentRound);
            getParentModule()->bubble(mesg);
        }
    }
    else {
        CongApp::handleMessage(msg);
    }
}

void WorkerApp::setField(AggPacket* pk) {
    pk->setSeqNumber(getNextSeq());
    pk->setDestAddr(destAddr);
    pk->setDestPort(destPort);
    pk->setAggSeqNumber(nextAggSeq++);
    pk->setRound(currentRound);
    pk->setJobId(jobId);
    pk->setWorkerNumber(numWorkers);
    pk->setRecordLen(1);
    pk->addRecord(localAddr);
    pk->setWorkerNumber(numWorkers);
    pk->setDistance(0);
}

void WorkerApp::prepareTxBuffer()
{
    auto leftData = flowSize;
    while (leftData > 0) {
        auto packetSize = messageLength < leftData ? messageLength : leftData;
        auto pk = createDataPacket(packetSize);
        if (currentRound == numRounds && leftData == packetSize) // last packet of last round
            pk->setFIN(true);
        insertTxBuffer(pk);
        incrementNextSeqBy(packetSize);
        leftData -= packetSize;
    }
}

void WorkerApp::onReceivedNewPacket(Packet* pk)
{
    ASSERT(pk->getRound() == currentRound);
    if (pk->getAckNumber() - roundStartSeq == flowSize) {
        onRoundStop();
    }
    CongApp::onReceivedNewPacket(pk);
}

Packet* WorkerApp::createDataPacket(B packetBytes)
{
    auto pk = new AggPacket();
    pk->setByteLength(packetBytes);
    setField(pk);
    return pk;
}

void WorkerApp::onRoundStart()
{
    currentRound += 1;
    roundStartSeq = getNextSentSeq();
    ASSERT(tcpState == OPEN);
    EV_INFO << "current round:" << currentRound << " flowSize:" << flowSize << endl;
    prepareTxBuffer();
    if (jobMetricCollector)
        jobMetricCollector->reportFlowStart(jobId, numWorkers, workerId, simTime());
}

void WorkerApp::onRoundStop()
{
    cong->reset(); // ! reuse connection but cong must be reset.
    if (currentRound < numRounds) {// note it's '<' here
        scheduleAfter(roundInterval, roundStartTimer);
        nextAggSeq = 0; // TODO is this necessary ?
    }
    if (jobMetricCollector)
        jobMetricCollector->reportFlowStop(jobId, numWorkers, workerId, simTime());
}

void WorkerApp::finish()
{
    CongApp::finish();
    cancelAndDelete(roundStartTimer);
    if (currentRound != numRounds) {
        EV_WARN << getClassAndFullPath() << "job " << jobId << " "
                                        "address " << localAddr
                                        << " round " << currentRound << " not finish." << endl;

    }
}
