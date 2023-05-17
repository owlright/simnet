#include "UnicastSenderApp.h"
#include "simnet/common/AddressResolver.h"
#include "simnet/common/ModuleAccess.h"
Define_Module(UnicastSenderApp);
//signals
simsignal_t UnicastSenderApp::cwndSignal = registerSignal("cwnd");
simsignal_t UnicastSenderApp::rttSignal = registerSignal("rtt");
simsignal_t UnicastSenderApp::fctSignal = registerSignal("fct");
simsignal_t UnicastSenderApp::idealFctSignal = registerSignal("idealFct");
simsignal_t UnicastSenderApp::flowSizeSignal = registerSignal("flowSize");

UnicastSenderApp::~UnicastSenderApp() {
    cancelAndDelete(flowStartTimer);
    cancelAndDelete(jitterTimeout);
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
            EV << "destAddr: " << destAddr << " destPort: " << destPort << endl;
        }
        else {
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
        auto packet = createDataPacket(packetSize);
        connection->send(packet);
        cong->onSendData(packetSize);
    }

}

void UnicastSenderApp::onFlowStart()
{
    sentBytes = 0;
    confirmedBytes = 0;
    flowStartTime = simTime();
    if (loadMode) //flowSize will change only in loadMode
        currentFlowSize = flowSize->intValue();
    cong->reset();
    emit(flowSizeSignal, currentFlowSize);
    emit(idealFctSignal, SimTime((8.0*currentFlowSize)/bandwidth));
}

void UnicastSenderApp::onFlowStop()
{
    currentRound += 1;
    emit(fctSignal, (simTime() - flowStartTime));
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
    // let cong algo update state
    confirmedBytes = pk->getSeqNumber();
    cong->onRecvAck(pk->getSeqNumber(), pk->getECE());


    if (sentBytes < currentFlowSize) {
        if (!jitterTimeout->isScheduled()) // ! in case multiple acks arrived at the same time
            scheduleAfter(jitterBeforeSending->doubleValueInUnit("s"), jitterTimeout);
    } else {
        //TODO if all packets sended

    }
    //TODO if all packets are confirmed
    if (confirmedBytes == currentFlowSize) {
        onFlowStop();
    }

    delete pk;
}

Packet* UnicastSenderApp::createDataPacket(B packetBytes)
{
    char pkname[40];
    sprintf(pkname, "conn%" PRId64 "-%" PRId64 "-to-%" PRId64 "-seq%" PRId64,
            connection->getConnectionId(), localAddr, destAddr, sentBytes);
    auto pk = new Packet(pkname);
    pk->setKind(DATA);
    pk->setSeqNumber(sentBytes);
    pk->setByteLength(packetBytes);
    pk->setECN(false);
    return pk;
}
