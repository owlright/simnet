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
}

void UnicastSenderApp::finish() {
    EV << "retransmit bytes: " << retransmitBytes << endl;
    if (currentRound != numRounds) {
        EV_WARN << "Complete " << currentRound << " rounds,  not reach " << numRounds << endl;
    }
    for (auto&it: cong->getDisorders()) {
        EV_WARN << it.first << " " << it.second << endl;
    }
//    for (auto&it: confirmedDisorders) {
//        EV_WARN << it << " "<< endl;
//    }
}

void UnicastSenderApp::initialize(int stage)
{
    UnicastApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        // two ways to set destAddr
        destAddr = par("destAddress");
        numRounds = par("numRounds");
        if (destAddr == -1) {
            std::vector<std::string> v = cStringTokenizer(par("destAddresses").stringValue()).asVector();
            destAddresses = AddressResolver::resolve(v);
            if (destAddresses.size() > 0) // TODO
                destAddr = destAddresses[0];
        }
        //HACK
        bandwidth = check_and_cast<cDatarateChannel *>(
                                    getParentModule()
                                    ->gateHalf("port", cGate::Type::OUTPUT, 0)
                                    ->getChannel())->getDatarate();
        EV_DEBUG << "port bandwidth: " << bandwidth << " bps" << endl;
        load = par("load");
        if (0.0 < load && load <= 1.0)
        {
            loadMode = true;
        }
        flowSize = &par("flowSize");
        flowInterval = &par("flowInterval");
        jitterBeforeSending = &par("jitterBeforeSending");
    }

    if (stage == INITSTAGE_ASSIGN) {
        tpManager = findModuleFromTopLevel<TrafficPatternManager>("trafficPatternManager", this);
        if (tpManager!=nullptr)
            destAddr = tpManager->getDestAddr(localAddr);
    }

    if (stage == INITSTAGE_LAST) {
        if (destAddr >= 0) {
            destPort = par("destPort");
            messageLength = par("messageLength");
            if (!loadMode)
            {
                currentFlowSize = flowSize->intValue();
                currentFlowInterval = flowInterval->doubleValueInUnit("s"); // convert any unit into s
            }
            else
            {
                // flowSize will change every time, only flowSizeMean is known
                B flowSizeMean = par("flowSizeMean").intValue();
                // calc interval by load
                ASSERT(flowSizeMean > 0 && load > 0);
                currentFlowInterval = SimTime(flowSizeMean / (bandwidth * load)); // load cannot be zero
            }

            connection->bindRemote(destAddr, destPort); // ! bind remote before using send
            cong = check_and_cast<CongAlgo*>(getSubmodule("cong"));
            cong->setSegmentSize(messageLength);

            //schedule sending event
            flowStartTimer = new cMessage("flowStart");
            jitterTimeout = new cMessage("jitterTimeout");
            scheduleAfter(currentFlowInterval, flowStartTimer);
            if (isUnicastSender)
                EV_DEBUG << "destAddr: " << destAddr << " destPort: " << destPort << endl;
        }
        else {
            if (isUnicastSender)
                EV_WARN << "address " << localAddr << "'s app has no destAddress" << endl;
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
    } else {
        UnicastApp::handleMessage(msg);
    }
}

void UnicastSenderApp::sendPendingData()
{
    while (cong->getcWnd() > inflightBytes() && sentBytes < currentFlowSize) {
        auto packetSize = messageLength;
        if (messageLength + sentBytes > currentFlowSize) {
            packetSize = currentFlowSize - sentBytes; // ! incase the data about to send is too small, such as the last packet or flowSize is too small
        }
        sentBytes += packetSize;
        auto packet = createDataPacket(sentBytes, packetSize);
        connection->send(packet);
        cong->onSendData(packetSize);
    }

}

void UnicastSenderApp::onFlowStart()
{
    currentRound += 1;
    sentBytes = 0;
    confirmedBytes = 0;
    currentBaseRTT = 0;
    confirmedDisorders.clear();
    flowStartTime = simTime();
    EV_INFO << "Current round seq: " << currentRound << endl;
    if (loadMode) //flowSize will change only in loadMode
        currentFlowSize = flowSize->intValue();
    cong->reset();
    emit(flowSizeSignal, currentFlowSize);

}

void UnicastSenderApp::onFlowStop()
{
    emit(fctSignal, (simTime() - flowStartTime));
    emit(idealFctSignal, currentBaseRTT + SimTime((currentFlowSize*8) / bandwidth));
    if (currentRound < numRounds) {// note it's '<' here
        if (!loadMode) {
            ASSERT(flowInterval != nullptr);
            currentFlowInterval = flowInterval->doubleValueInUnit("s");
        }
        scheduleAfter(currentFlowInterval, flowStartTimer);
    }
}

void UnicastSenderApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    ASSERT(pk->getKind()==PacketType::ACK);
    auto seq = pk->getSeqNumber();
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
            EV_WARN << "resend seq " << it.first << endl;
            auto packet = createDataPacket(it.first, messageLength); // ! TODO what if it's the last packet?
            connection->send(packet);
//            cong->onSendData(messageLength);
            retransmitBytes += messageLength; // affect inflightBytes
            // count down, reset counter
            // it.second = 3; // ! do not change anything owned by cong
        }
    }
    // check if resend packet is receieved
    if (disorders.find(seq) != disorders.end()) { // first time we receive the ack
        confirmedRetransBytes += messageLength; // ! TODO what if it's the last packet?
        confirmedDisorders.insert(seq);
    }
    else if (confirmedDisorders.find(seq) != confirmedDisorders.end()) {
        // the resend packets's ack received more than once
        // ! which is plus above
        confirmedBytes -= pk->getReceivedBytes();
    }
    auto pkRTT = simTime() - SimTime(pk->getStartTime());
    emit(rttSignal, pkRTT);
    currentBaseRTT = pkRTT - pk->getQueueTime() - pk->getTransmitTime();
    if (sentBytes < currentFlowSize) {
        if (!jitterTimeout->isScheduled()) // ! in case multiple acks arrived at the same time
        {
            auto jitter = jitterBeforeSending->doubleValueInUnit("s");
            scheduleAfter(jitter, jitterTimeout);
        }

    } else {
        //TODO if all packets sended

    }
    //TODO if all packets are confirmed
    if (confirmedBytes == currentFlowSize) {
        onFlowStop();
    }

    delete pk;
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
    if (sentBytes == currentFlowSize)
        pk->setIsFlowFinished(true);
    return pk;
}
