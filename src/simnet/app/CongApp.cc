#include "CongApp.h"
#include "simnet/common/AddressResolver.h"
#include "simnet/common/ModuleAccess.h"
Define_Module(CongApp);
//signals
simsignal_t CongApp::fctSignal = registerSignal("fct");
simsignal_t CongApp::idealFctSignal = registerSignal("idealFct");
simsignal_t CongApp::flowSizeSignal = registerSignal("flowSize");
simsignal_t CongApp::rttSignal = registerSignal("rtt");
simsignal_t CongApp::inflightBytesSignal = registerSignal("inflightBytes");

CongApp::~CongApp() {
    cancelAndDelete(flowStartTimer);
    cancelAndDelete(jitterTimeout);
    cancelAndDelete(RTOTimeout);
}

bool CongApp::bindRemote()
{
    if(destAddr != INVALID_ADDRESS && destPort != INVALID_PORT) {
        connection->bindRemote(destAddr, destPort); // ! bind remote before using send
        appState = Scheduled;
        return true;
    }
    else {
        return false;
    }
}

void CongApp::finish() {
    if (destAddr == INVALID_ADDRESS)
        return;

    EV << localAddr << " retransmit bytes: " << retransmitBytes << endl;
    // if (currentRound != numRounds) {
    //     EV_WARN << getClassAndFullPath() << " " << localAddr << " complete " << currentRound << " rounds,  not reach " << numRounds << endl;
    // }
    for (auto& [seq, n]: disorders) {
        EV_WARN << seq  << endl;
    }
//    for (auto&it: confirmedDisorders) {
//        EV_WARN << it << " "<< endl;
//    }
}

void CongApp::initialize(int stage)
{
    UnicastApp::initialize(stage);
    // ! if this moudle is created by manager in ASSIGN stage, the stages before it will not be executed
    if (stage == INITSTAGE_LOCAL) {
        maxDisorderNumber = par("maxDisorderNumber");
        appState = Idle;
        messageLength = par("messageLength");
        useJitter = par("useJitter");
        bandwidth = getParentModule()->par("bandwidth");
        flowSize = par("flowSize");
        flowStartTime = par("flowStartTime");
        flowStartTimer = new cMessage("flowStart");
        RTOTimeout = new cMessage("RTOTimeout");
        estimatedRTT = par("initRTO");
        // flowInterval = &par("flowInterval");
        if (useJitter)
            jitterBeforeSending = &par("jitterBeforeSending");
        cong = check_and_cast<CongAlgo*>(getSubmodule("cong"));
        cong->setSegmentSize(messageLength);
        destAddr = par("destAddress");
        destPort = par("destPort");
        if (bindRemote()) {
            scheduleAt(flowStartTime, flowStartTimer);
        }
    }
}

void CongApp::handleMessage(cMessage *msg)
{
    if (msg == flowStartTimer) { // new flow
        if (!getEnvir()->isExpressMode())
            getParentModule()->bubble("a new flow!");
        onFlowStart();
        sendPendingData();
    } else if (msg == jitterTimeout) { // most times is a new window
        sendPendingData();
    } else if (msg == RTOTimeout) {
        switch(appState) {
            case AllDataSended: // ! if the last window's data are lost, no more acks arrive, we have to trigger it by ourself
                if (!sentButNotAcked.empty()) {
                    for (const auto& [seq, pkSize] : sentButNotAcked) {
                        addRetransPacket(seq, pkSize);
                    }
                    sendPendingData();
                }
                break;
            case Sending:
                if (leftEdge == oldLeftEdge) { // ! leftEdge should be the earliest ack we accept, if it doesnt change, something wrong
                    ASSERT(sentButNotAcked.find(leftEdge) != sentButNotAcked.end());
                    addRetransPacket(leftEdge, sentButNotAcked[leftEdge]);
                    sendPendingData();
                }
                break;
            default:
                throw cRuntimeError("forget to add break.");
        }
        scheduleAfter(estimatedRTT, RTOTimeout); // we have to repeadedly check, this will be canceled when appState==finished
    } else {
        UnicastApp::handleMessage(msg);
    }
}

void CongApp::scheduleNextFlowAt(simtime_t_cref time)
{
    ASSERT(!flowStartTimer->isScheduled());
    scheduleAt(time, flowStartTimer);
    appState = Scheduled;
}

void CongApp::sendPendingData()
{
    while (cong->getcWnd() >= inflightBytes() && sentBytes < flowSize) {
        auto packetSize = messageLength;
        if (messageLength + sentBytes > flowSize) {
            packetSize = flowSize - sentBytes; // ! incase the data about to send is too small, such as the last packet or flowSize is too small
        }
        sentBytes += packetSize;
        ASSERT(sentButNotAcked.find(sentBytes) == sentButNotAcked.end());
        sentButNotAcked[sentBytes] = packetSize;
        auto packet = createDataPacket(sentBytes, packetSize);
        connection->send(packet);
        cong->onSendData(sentBytes, packetSize);
        if (sentBytes == packetSize) { // the first packet
            scheduleAfter(estimatedRTT, RTOTimeout);
        }
    }
    while (cong->getcWnd() >= inflightBytes() && !holdRetrans.empty()) {
        auto packet = holdRetrans.front();
        auto seq = packet->getSeqNumber();
        connection->send(packet);
        retransmitBytes += packet->getByteLength();
        holdRetrans.pop();
        if (retrans.find(seq) == retrans.end()) // ! first time we resend
            retrans[seq] = 1;
        else
            retrans[seq]++;
    }
    moveToNextEdge(leftEdge); // move leftEdge to next position

}

void CongApp::addRetransPacket(SeqNumber seq, B packetBytes)
{
    EV_DEBUG << "add resend seq " << seq << endl;
    auto packet = createDataPacket(seq, packetBytes);
    packet->setResend(true);
    holdRetrans.push(packet);
}

void CongApp::onFlowStart()
{
    currentRound += 1;
    sentBytes = 0;
    confirmedNormalBytes = 0;
    retransmitBytes = 0;
    confirmedRetransBytes = 0;
    confirmedRedundantBytes = 0;
    currentBaseRTT = 0;
    flowStartTime = simTime().dbl();
    appState = Sending;
    ASSERT(sentButNotAcked.empty());
    ASSERT(disorders.empty());
    retrans.clear(); // ! it may not come to zero at this round's end, see Case 2 below
    ASSERT(!RTOTimeout->isScheduled());
    if (connection == nullptr) {
        throw cRuntimeError("conneciton is nullptr!");
    }
    EV_INFO << " flowSize: " << flowSize << endl;
    cong->reset();
    emit(flowSizeSignal, flowSize);
}

void CongApp::onFlowStop()
{
    emit(fctSignal, (simTime() - flowStartTime));
    emit(idealFctSignal, currentBaseRTT + SimTime((flowSize*8) / bandwidth));
    appState = Finished;
}

void CongApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    ASSERT(pk->getKind()==PacketType::ACK);
    auto seq = pk->getSeqNumber();
    auto segmentSize = pk->getReceivedBytes();
    auto round = pk->getRound();
    ASSERT(segmentSize > 0);
// #ifndef NDEBUG
//     if (localAddr == 656 && localPort==2000) {
//         std::cout <<round << " " << seq << endl;
//     }
// #endif
    bool is_this_round = round == currentRound;
    if (!is_this_round) {
        // ! Cases that receive ack not this round:
        // ! 1. resend packets may be sent out at last round's last window because of disorder but not lost
        // ! 2. resend too many times and redundant acks arrive this round.
        // ! since we've already begun a new round,
        // ! we don't handle these packets, just delete it and return
        delete pk;
        return;
    }
    else {
        cong->onRecvAck(seq, segmentSize, pk->getECE()); // let cong algo update cWnd
    }
    ASSERT(is_this_round);
    bool is_not_acked = sentButNotAcked.find(seq) != sentButNotAcked.end();
    bool is_redundant_ack = !is_not_acked;

    if (is_redundant_ack) {
        // #ifndef NDEBUG
        // if (localAddr == 784 && localPort==2000 && confirmedRedundantBytes==935000) {
        //     std::cout << "redundant ack " << seq << " " << retransmitBytes << " " << confirmedRedundantBytes << endl;
        // }
        // #endif
        // ! received a seq's ack more than once
        confirmedRedundantBytes += segmentSize;
    }
    else {
        // ! this first ack to this seq, here are some cases:
        // ! 1. original ack arrive
        // ! 2. packet is lost, resend's ack arrive
        // ! 3. packet is not lost, but resend packet's ack arrive earlier.
        if (pk->getResend())
            confirmedRetransBytes += segmentSize;
        else
            confirmedNormalBytes += segmentSize;

        if (retrans.find(seq) != retrans.end()) {
            retrans[seq]--;
            if (retrans[seq] == 0)
                retrans.erase(seq);
        }

        disorders.erase(seq);
        sentButNotAcked.erase(seq);
        // ! this could be empty for example: all data acked, but cWnd becomes 1, so after ack this, you will get empty
        if (sentButNotAcked.empty()) {
            // ! update new disorders
            auto right = cong->getMaxAckedSeqNumber();
            for (auto it:sentButNotAcked) {
                auto tmpseq = it.first;
                if (tmpseq != seq && tmpseq > leftEdge && tmpseq < right
                        && disorders.find(tmpseq) == disorders.end()) {
                    disorders[tmpseq] = maxDisorderNumber;
                }
                if (tmpseq > right) // ! sentButNotAcked must be ordered
                    break;
            }
        }
    }

    // resend the countdown packets
    auto rtt_timer = cong->getcWnd() / messageLength + 1;
    for (auto& it : disorders) {
        if (it.second == 0) {
            // count down, must resend this seq
            auto tmp_seq = it.first;
            ASSERT(sentButNotAcked.find(tmp_seq)!= sentButNotAcked.end());
            addRetransPacket(tmp_seq, sentButNotAcked.at(tmp_seq));
            disorders[tmp_seq] = rtt_timer > maxDisorderNumber ? rtt_timer : maxDisorderNumber; // wait for a RTT
        }
        else {
            it.second--;
        }
    }

    auto sampleRTT = simTime() - SimTime(pk->getStartTime());
    emit(rttSignal, sampleRTT);
    currentBaseRTT = sampleRTT - pk->getQueueTime() - pk->getTransmitTime();
    estimatedRTT = (1 - 0.125) * estimatedRTT + 0.125 * sampleRTT;
    if (sentBytes < flowSize) {
        if (useJitter && !jitterTimeout->isScheduled()) // ! in case multiple acks arrived at the same time
        {
            auto jitter = jitterBeforeSending->doubleValueInUnit("s");
            scheduleAfter(jitter, jitterTimeout);
        }
        else
            sendPendingData();

    } else {
        rescheduleAfter(estimatedRTT, RTOTimeout);
        appState = AllDataSended;
    }

    if (confirmedNormalBytes + confirmedRetransBytes == flowSize) {
        cancelEvent(RTOTimeout); // ! avoid affecting next flow
        if (!flowStartTimer->isScheduled())  // ! avoid schedule timer multiple times
            onFlowStop();
        // if (localAddr == 53 || localAddr == 232 || localAddr == 234)
        //     std::cout << localAddr << " flow finished!" << endl;
    }

    delete pk;
}

void CongApp::handleParameterChange(const char *parameterName)
{
    UnicastApp::handleParameterChange(parameterName);
    // this can happen when node change the app destionation
    if (strcmp(parameterName, "destAddress") == 0) {
        destAddr = par("destAddress");
        bindRemote();
    }
    else if (strcmp(parameterName, "destPort") == 0) {
        destPort = par("destPort");
        bindRemote();
    }

}

Packet* CongApp::createDataPacket(SeqNumber seq, B packetBytes)
{
    char pkname[40];
    sprintf(pkname, "conn%" PRId64 "-%" PRId64 "-to-%" PRId64 "-seq%" PRId64,
            connection->getConnectionId(), localAddr, destAddr, seq);
    auto pk = new Packet(pkname);
    pk->setRound(currentRound);
    pk->setKind(DATA);
    pk->setSeqNumber(seq);
    pk->setByteLength(packetBytes);
    pk->setECN(false);
    pk->setStartTime(simTime().dbl());
    pk->setTransmitTime(0);
    pk->setQueueTime(0);
    if (sentBytes == flowSize)
        pk->setIsFlowFinished(true);
    return pk;
}

void CongApp::refreshDisplay() const
{
    if (!getEnvir()->isExpressMode()) {
        char buf[50];
        sprintf(buf, "remote: %" PRId64 ":%u\n local: %" PRId64 ":%u",
                destAddr, destPort, localAddr, localPort);
        getDisplayString().setTagArg("t", 0, buf);
    }
}
