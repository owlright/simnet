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
    for (auto& seq: disorders) {
        EV_WARN << seq << endl;
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
        appState = Idle;
        messageLength = par("messageLength");
        useJitter = par("useJitter");
        bandwidth = getParentModule()->par("bandwidth");
        flowSize = par("flowSize");
        flowStartTime = par("flowStartTime");
        flowStartTimer = new cMessage("flowStart");
        RTOTimeout = new cMessage("RTOTimeout");
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
        if (!sentButNotAcked.empty()) {
            auto leftRoom = cong->getcWnd() - inflightBytes();
            for (auto& it: sentButNotAcked) {
                if (leftRoom >= it.second) {
                    retransmitLostPacket(it.first, it.second);
                    leftRoom -= it.second;
                }
            }
            scheduleAfter(currentBaseRTT, RTOTimeout);
        }
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
    while (cong->getcWnd() > inflightBytes() && sentBytes < flowSize) {
        auto packetSize = messageLength;
        if (messageLength + sentBytes > flowSize) {
            packetSize = flowSize - sentBytes; // ! incase the data about to send is too small, such as the last packet or flowSize is too small
        }
        sentBytes += packetSize;
        ASSERT(sentButNotAcked.find(sentBytes) == sentButNotAcked.end());
        sentButNotAcked[sentBytes] = packetSize;
        auto packet = createDataPacket(sentBytes, packetSize);
        connection->send(packet);
        cong->onSendData(packetSize);
    }

}

void UnicastSenderApp::retransmitLostPacket(SeqNumber seq, B packetBytes)
{
    EV_WARN << "resend seq " << seq << endl;
    auto packet = createDataPacket(seq, packetBytes);
    packet->setResend(true);
    connection->send(packet);
//      cong->onSendData(messageLength); // ? Do cong needs to know this
    // if (seq >= 500000)
    //    std::cout << localAddr <<" retransmit seq " << seq << endl;
    retransmitBytes += packetBytes; // affect inflightBytes
}

void UnicastSenderApp::onFlowStart()
{
    sentBytes = 0;
    confirmedBytes = 0;
    retransmitBytes = 0;
    confirmedRetransBytes = 0;
    currentBaseRTT = 0;
    flowStartTime = simTime().dbl();
    appState = Sending;
    ASSERT(sentButNotAcked.empty());
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
    // if (localAddr == 270)
    //     std::cout << seq << " " << pk->getResend() << endl;
    if (sentButNotAcked.find(seq) == sentButNotAcked.end()) {
        // ! receiver may ack to multiple resend packets, ignore the others
        cong->onRecvAck(seq, pk->getReceivedBytes(), pk->getECE()); // let cong algo update state
        delete pk;
        return;
    }
    confirmedBytes += pk->getReceivedBytes();
    cong->onRecvAck(pk->getSeqNumber(), pk->getReceivedBytes(), pk->getECE()); // let cong algo update state

    // ! check if disordered packets are received
    // std::vector<SeqNumber> removed;
    auto disorderSeqs = cong->getDisorders();

    // resend the countdown packets
    for (auto& it : disorderSeqs) {
        if (it.second == 0) {
            disorders.insert(it.first);
            // count down, must resend this seq
            retransmitLostPacket(it.first, sentButNotAcked.at(it.first));
        }
    }
    // check if resend packet is receieved
    if (disorders.find(seq) != disorders.end()) { // first time we receive the ack
        confirmedRetransBytes += sentButNotAcked.at(seq);
        disorders.erase(seq);
    }

    sentButNotAcked.erase(seq);
    auto pkRTT = simTime() - SimTime(pk->getStartTime());
    emit(rttSignal, pkRTT);
    currentBaseRTT = pkRTT - pk->getQueueTime() - pk->getTransmitTime();
    if (sentBytes < flowSize) {
        if (useJitter && !jitterTimeout->isScheduled()) // ! in case multiple acks arrived at the same time
        {
            auto jitter = jitterBeforeSending->doubleValueInUnit("s");
            scheduleAfter(jitter, jitterTimeout);
        }
        else
            sendPendingData();

    } else {
        rescheduleAfter(pkRTT, RTOTimeout);
        appState = AllDataSended;
    }
    //TODO if all packets are confirmed
    if (confirmedBytes == flowSize) {
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
