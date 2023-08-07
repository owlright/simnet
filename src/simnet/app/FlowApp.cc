#include "FlowApp.h"
Define_Module(FlowApp);

FlowApp::~FlowApp()
{
    cancelAndDelete(flowStartTimer);
    cancelAndDelete(jitterTimeout);
}

void FlowApp::initialize(int stage)
{
    CongApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        appState = Idle;
        useJitter = par("useJitter");
        flowSize = par("flowSize");
        flowStartTime = par("flowStartTime");
        flowStartTimer = new cMessage("flowStart");
        if (useJitter)
            jitterBeforeSending = &par("jitterBeforeSending");
    }
    else if (stage == INITSTAGE_LAST) {
        scheduleNextFlowAt(flowStartTime);
    }
}

void FlowApp::handleMessage(cMessage *msg)
{
    CongApp::handleMessage(msg);
    if (msg == flowStartTimer) { // new flow
        if (!getEnvir()->isExpressMode())
            getParentModule()->bubble("a new flow!");
        onFlowStart();
        sendPendingData();
    } else if (msg == jitterTimeout) { // most times is a new window
        sendPendingData();
    }
}

void FlowApp::onReceivedAck(const Packet* pk)
{
    if (pk->getFIN()) {
        ASSERT(tcpState == FIN_WAIT_1);
        tcpState = CLOSED; // ! for simple flow app, we don't want to wait for 2MSL
    }
    CongApp::onReceivedAck(pk);
}

void FlowApp::onConnectionClose() {
    if (!flowStartTimer->isScheduled()) {// ! avoid duplicate fin_ack schedule timer multiple times
        onFlowStop();
        CongApp::resetState();
    }
}

void FlowApp::onFlowStart()
{
    if (!bindRemote()) {
        throw cRuntimeError("Failed to setup connection!");
    }
    currentRound += 1;
    ASSERT(tcpState == CLOSED);
    tcpState = OPEN;
    ASSERT(appState == Scheduled);
    appState = Sending;
    flowStartTime = simTime().dbl(); // ! reset flow start_time, so fct is correct
    // ASSERT(!RTOTimeout->isScheduled());
    if (connection == nullptr) {
        throw cRuntimeError("conneciton is nullptr!");
    }
    EV_INFO << " flowSize: " << flowSize << endl;
    prepareTxBuffer();
    emit(flowSizeSignal, flowSize);
}

void FlowApp::onFlowStop()
{
    appState = Finished;
    emit(fctSignal, (simTime() - flowStartTime));
    emit(idealFctSignal, getCurrentBaseRTT() + SimTime((flowSize*8) / getMaxSendingRate()));
}

void FlowApp::prepareTxBuffer()
{
    auto leftData = flowSize;

    while (leftData > 0) {
        auto packetSize = messageLength < leftData ? messageLength : leftData;
        auto pk = createDataPacket(packetSize);
        if (leftData == packetSize) // last packet
            pk->setFIN(true);
        insertTxBuffer(pk);
        leftData -= packetSize;
    }
}

Packet* FlowApp::createDataPacket(B packetBytes)
{
//    char pkname[40];
    auto pk = new Packet();
//    sprintf(pkname, "flow%" PRId64 "-to-%" PRId64 "-seq%" PRId64, localAddr, destAddr, getNextSeq());
    pk->setKind(DATA);
    pk->setDestAddr(destAddr);
    pk->setDestPort(destPort);
    pk->setPacketType(DATA);
    pk->setByteLength(packetBytes);
//    pk->setName(pkname);
    return pk;
}


void FlowApp::scheduleNextFlowAt(simtime_t_cref time)
{
    ASSERT(!flowStartTimer->isScheduled());
    scheduleAt(time, flowStartTimer);
    appState = Scheduled;
}
