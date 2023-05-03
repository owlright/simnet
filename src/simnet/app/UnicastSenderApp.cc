#include "UnicastSenderApp.h"
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
        destAddr = par("destAddress");
        destPort = par("destPort");
        messageLength = par("messageLength");
        flowSize = &par("flowSize");
        flowInterval = &par("flowInterval");
        connection.setConnectionId(cSimulation::getActiveEnvir()->getUniqueNumber());
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
        // make packet
        auto packet = new Packet();
        setCommonField(packet);
        packet->setConnectionId(connection.getConnectionId());
        packet->setKind(PacketType::DATA);
        packet->setSeqNumber(sentBytes);
        packet->setByteLength(packetSize);
        packet->setDestAddr(destAddr);
        packet->setDestPort(destPort);
        connection.sendTo(packet, destAddr, destPort);
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

cMessage *UnicastSenderApp::makeDataPacket(Connection *connection, Packet *pk)
{
    char pkname[40];
    sprintf(pkname, "flow%" PRId64 "-%" PRId64 "-to-%" PRId64 "-seq%" PRId64,
            flowId, myAddr, destAddr, sentBytes);
    pk->setName(pkname);
    pk->setECN(false);
    return pk;
}
