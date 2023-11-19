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
        flowSize = &par("flowSize");
        flowStartTime = par("flowStartTime");
        flowStartTimer = new cMessage("flowStart");
        if (useJitter)
            jitterBeforeSending = &par("jitterBeforeSending");

        flowMetricCollector = findModuleFromTopLevel<GlobalMetricCollector>("metricCollector", this);
        if (flowMetricCollector == nullptr)
            throw cRuntimeError("metricCollector not found");
    } else if (stage == INITSTAGE_LAST) {
        scheduleNextFlowAt(flowStartTime);
    }
}

void FlowApp::handleMessage(cMessage* msg)
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

void FlowApp::onConnectionClose()
{
    if (!flowStartTimer->isScheduled()) { // ! avoid duplicate fin_ack schedule timer multiple times
        onFlowStop();
        CongApp::resetState();
    }
}

void FlowApp::resend(TxItem& item)
{
    // * do nothing
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
    currFlowSize = flowSize->intValue();
    while (currFlowSize <= 0) { // ! make sure flowsize is > 0
        currFlowSize = flowSize->intValue();
    }
    // if (localAddr == 136) {
    //     std::cout << simTime() << " "<< currentRound << " " << this->getFullPath() << " " << destAddr << " " <<
    //     currFlowSize << endl;
    // }
    EV_INFO << " flowSize: " << currFlowSize << endl;
    leftData = currFlowSize;
}

void FlowApp::onFlowStop()
{
    if (!txBuffer.empty()) {
        throw cRuntimeError("txBuffer not empty");
    }
    if (!rxBuffer.empty()) {
        throw cRuntimeError("rxBuffer not empty");
    }
    appState = Finished;
    flowMetricCollector->reportFlowStop(localAddr, simTime());
    emit(flowSizeSignal, currFlowSize);
    emit(fctSignal, (simTime() - flowStartTime));
    emit(idealFctSignal, getCurrentBaseRTT() + SimTime((currFlowSize * 8) / getMaxSendingRate()));
}

Packet* FlowApp::createDataPacket()
{
    if (leftData > 0) {
        auto packetSize = messageLength < leftData ? messageLength : leftData;
        auto pk = new Packet();
        pk->setSeqNumber(getNextSeq());
        pk->setDestAddr(destAddr);
        pk->setDestPort(destPort);
        pk->setPacketType(DATA);
        pk->setByteLength(packetSize);
        if (leftData == packetSize) // last packet
            pk->setFIN(true);
        leftData -= packetSize;
        return pk;
    } else {
        return nullptr;
    }
}

void FlowApp::scheduleNextFlowAt(simtime_t_cref time)
{
    ASSERT(!flowStartTimer->isScheduled());
    scheduleAt(time, flowStartTimer);
    appState = Scheduled;
}
