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

void FlowApp::confirmAckNumber(const Packet* pk)
{
    if (tcpState == TIME_WAIT) {
        tcpState = CLOSED; // ! for simple flow app, we don't want to wait for 2MSL
    }
    CongApp::confirmAckNumber(pk);
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
        incrementNextSeqBy(packetSize);
        leftData -= packetSize;
    }
}

Packet* FlowApp::createDataPacket(B packetBytes)
{
    auto pk = new Packet();
    pk->setSeqNumber(getNextSeq());
    pk->setDestAddr(destAddr);
    pk->setDestPort(destPort);
    pk->setPacketType(DATA);
    pk->setByteLength(packetBytes);
    return pk;
}

void FlowApp::scheduleNextFlowAt(simtime_t_cref time)
{
    ASSERT(!flowStartTimer->isScheduled());
    scheduleAt(time, flowStartTimer);
    appState = Scheduled;
}
