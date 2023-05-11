#include "UnicastSenderApp.h"
#include "simnet/common/AddressResolver.h"
Define_Module(UnicastSenderApp);
//signals
simsignal_t UnicastSenderApp::cwndSignal = registerSignal("cwnd");
simsignal_t UnicastSenderApp::rttSignal = registerSignal("rtt");
simsignal_t UnicastSenderApp::fctSignal = registerSignal("fct");
simsignal_t UnicastSenderApp::idealFctSignal = registerSignal("idealFct");

UnicastSenderApp::~UnicastSenderApp() {
    cancelAndDelete(flowStartTimer);
}

void UnicastSenderApp::initialize(int stage)
{
    UnicastApp::initialize(stage);
    if (stage==INITSTAGE_LOCAL) {
        // two ways to set destAddr
        destAddr = par("destAddress");
        if (destAddr == -1) {
            std::vector<std::string> v = cStringTokenizer(par("destAddresses").stringValue()).asVector();
            destAddresses = AddressResolver::resolve(v);
            if (destAddresses.size() > 0) // TODO
                destAddr = destAddresses[0];
        }
    }

    if (stage == INITSTAGE_LAST) {
        if (destAddr > 0) {
            destPort = par("destPort");
            messageLength = par("messageLength");
            flowSize = &par("flowSize");
            flowInterval = &par("flowInterval");
            connection->bindRemote(destAddr, destPort); // ! note to bind remote before using send
            cong = check_and_cast<CongAlgo*>(getSubmodule("cong"));
            cong->setSegmentSize(messageLength);
            //HACK
            bandwidth = check_and_cast<cDatarateChannel *>(
                                        getParentModule()
                                        ->gateHalf("port", cGate::Type::OUTPUT, 0)
                                        ->getChannel())->getDatarate();
            //schedule sending event
            flowStartTimer = new cMessage("flowStart");
            scheduleAfter(flowInterval->doubleValue(), flowStartTimer);
            EV << "destAddr: " << destAddr << " destPort: " << destPort << endl;
        }
        else {
            EV_WARN << "address " << localAddr << "'s app has no destAddress" << endl;
        }

    }
}

void UnicastSenderApp::handleMessage(cMessage *msg)
{
    if (msg == flowStartTimer) {
        if (!getEnvir()->isExpressMode())
            getParentModule()->bubble("a new flow!");
        onFlowStart();
        sendPendingData();
    } else {
        UnicastApp::handleMessage(msg);
    }
}

void UnicastSenderApp::sendPendingData()
{
    while (cong->getcWnd() > inflightBytes() && sentBytes < currentFlowSize) {
        auto packetSize = messageLength;
        if (flowSize != 0 && messageLength + sentBytes > currentFlowSize) {
            packetSize = currentFlowSize - sentBytes; // the data about to send is too small.
        }
        sentBytes += packetSize;
        auto packet = createDataPacket(packetSize);
        connection->send(packet);
        cong->onSendData(packetSize);
    }

}

void UnicastSenderApp::onFlowStart()
{
    flowId++;
    sentBytes = 0;
    confirmedBytes = 0;
    flowStartTime = simTime();
    currentFlowSize = flowSize->intValue();
    cong->reset();
    emit(idealFctSignal, SimTime((8.0*currentFlowSize)/bandwidth));
}

void UnicastSenderApp::onFlowStop()
{
    emit(fctSignal, simTime() - flowStartTime);
    scheduleAfter(flowInterval->doubleValue(), flowStartTimer);
}

void UnicastSenderApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    ASSERT(pk->getKind()==PacketType::ACK);
    // let cong algo update state
    confirmedBytes = pk->getSeqNumber();
    cong->onRecvAck(pk->getSeqNumber(), pk->getECE());


    if (sentBytes < currentFlowSize) {
        sendPendingData();
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
