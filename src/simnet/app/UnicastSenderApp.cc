#include "UnicastSenderApp.h"
#include "simnet/common/AddressResolver.h"
#include "simnet/common/ModuleAccess.h"
Define_Module(UnicastSenderApp);
//signals
simsignal_t UnicastSenderApp::fctSignal = registerSignal("fct");
simsignal_t UnicastSenderApp::idealFctSignal = registerSignal("idealFct");
simsignal_t UnicastSenderApp::flowSizeSignal = registerSignal("flowSize");
simsignal_t UnicastSenderApp::rttSignal = registerSignal("rtt");
simsignal_t UnicastSenderApp::inflightBytesSignal = registerSignal("inflightBytes");

UnicastSenderApp::~UnicastSenderApp() {
    cancelAndDelete(flowStartTimer);
    cancelAndDelete(jitterTimeout);
    cancelAndDelete(RTOTimeout);
}

bool UnicastSenderApp::bindRemote()
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

void UnicastSenderApp::finish() {
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

void UnicastSenderApp::initialize(int stage)
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

void UnicastSenderApp::handleMessage(cMessage *msg)
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

void UnicastSenderApp::scheduleNextFlowAt(simtime_t_cref time)
{
    ASSERT(!flowStartTimer->isScheduled());
    scheduleAt(time, flowStartTimer);
    appState = Scheduled;
}

void UnicastSenderApp::sendPendingData()
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

void UnicastSenderApp::addRetransPacket(SeqNumber seq, B packetBytes)
{
    EV_DEBUG << "add resend seq " << seq << endl;
    auto packet = createDataPacket(seq, packetBytes);
    packet->setResend(true);
    holdRetrans.push(packet);
}

void UnicastSenderApp::onFlowStart()
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

void UnicastSenderApp::onFlowStop()
{
    emit(fctSignal, (simTime() - flowStartTime));
    emit(idealFctSignal, currentBaseRTT + SimTime((flowSize*8) / bandwidth));
    appState = Finished;
}

void UnicastSenderApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    ASSERT(pk->getKind()==PacketType::ACK);
    auto seq = pk->getSeqNumber();
    auto segmentSize = pk->getReceivedBytes();

    if (localAddr==271 && localPort==2000 && seq==48000) {
        std::cout << seq << " " << retransmitBytes <<  " " << confirmedRetransBytes << endl;
        std::cout << leftEdge << endl;
    }
    cong->onRecvAck(seq, segmentSize, pk->getECE()); // let cong algo update state
    if (sentButNotAcked.find(seq) == sentButNotAcked.end()) {
        // ! receiver may ack to multiple resend packets, ignore the others
        ASSERT(pk->getResend());
        ASSERT(retrans.find(seq) != retrans.end());
        confirmedRedundantBytes += segmentSize;
        retrans[seq]--;
        if (retrans[seq] == 0) // ? saving some memory ?
            retrans.erase(seq);
        // but let cong knows so that the rttBytes in cong can update correctly
        cong->onRecvAck(seq, segmentSize, pk->getECE());
        delete pk;
        return;
    }
    leftEdge = sentButNotAcked.begin()->first; // the closest seq we want

    // ! received a disorder seqnumber
    if (disorders.find(seq) != disorders.end()) { // ! deal with old seq
        if (!pk->getResend()) {
            confirmedNormalBytes += segmentSize; // this ack is just a litte late
        }
        else {
            retrans[seq]--;
            confirmedRetransBytes += sentButNotAcked.at(seq);
        }
        disorders.erase(seq);
    }
    else { // ! this is a total new seq
        ASSERT(seq >= leftEdge);
        confirmedNormalBytes += segmentSize;
    }
    sentButNotAcked.erase(seq); // no matter what just erase this seq

    auto right = cong->getMaxAckedSeqNumber();
    for (auto it:sentButNotAcked) {
        auto tmpseq = it.first;
        if (tmpseq != seq && tmpseq >= leftEdge && tmpseq < right
                && disorders.find(tmpseq) == disorders.end()) {
            disorders[tmpseq] = maxDisorderNumber;
        }
        if (tmpseq > right)
            break;
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
    //TODO if all packets are confirmed
    if (confirmedNormalBytes + confirmedRetransBytes == sentBytes) {
        cancelEvent(RTOTimeout); // ! avoid affecting next flow
        onFlowStop();
        // if (localAddr == 53 || localAddr == 232 || localAddr == 234)
        //     std::cout << localAddr << " flow finished!" << endl;
    }

    delete pk;
}

void UnicastSenderApp::handleParameterChange(const char *parameterName)
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

Packet* UnicastSenderApp::createDataPacket(SeqNumber seq, B packetBytes)
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

void UnicastSenderApp::refreshDisplay() const
{
    if (!getEnvir()->isExpressMode()) {
        char buf[50];
        sprintf(buf, "remote: %" PRId64 ":%u\n local: %" PRId64 ":%u",
                destAddr, destPort, localAddr, localPort);
        getDisplayString().setTagArg("t", 0, buf);
    }
}
