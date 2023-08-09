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

int CongApp::maxDisorderNumber = 0;

CongApp::~CongApp() {
    cancelAndDelete(RTOTimeout);
}

bool CongApp::bindRemote()
{
    if(destAddr != INVALID_ADDRESS && destPort != INVALID_PORT) {
        connection->bindRemote(destAddr, destPort); // ! bind remote before using send
        return true;
    }
    else {
        return false;
    }
}

void CongApp::onConnectionClose()
{
    resetState();
}

void CongApp::setPacketBeforeSentOut(Packet* pk)
{
    auto dest_addr = pk->getDestAddr();
    auto seq = pk->getSeqNumber();
    // ! prepare the packet name
    char pkname[50] = {};
    if (pk->getResend())
        sprintf(pkname, "Resend-");

    char fin_prefix[10] = {};
    if (pk->getFIN()) {
        sprintf(pkname, "FIN-");
        switch (tcpState) {
        case FIN_WAIT_1:
        case TIME_WAIT:
        case CLOSE_WAIT:
            break;
        case OPEN:
            tcpState = FIN_WAIT_1;
            break;
        default:
            throw cRuntimeError("Unknown state case");
        }
    }
    strcat(pkname, fin_prefix);

    char pktype[10] = {};
    switch (pk->getPacketType()) {
        case ACK:
            sprintf(pktype, "ACK-");
            break;
        case MACK:
            sprintf(pktype, "MACK-");
            break;
        case DATA:
            sprintf(pktype, "DATA-");
            break;
        case AGG:
            sprintf(pktype, "AGG-");
            break;
        default:
            throw cRuntimeError("Unknown packet type");
    }
    strcat(pkname, pktype);

    char src_dest_seq_ack[50];
    sprintf(src_dest_seq_ack, "%" PRId64 "-to-%" PRId64 "-seq-%" PRId64 "-ack-%" PRId64,
                        localAddr, dest_addr, seq, nextAckSeq);
    strcat(pkname, src_dest_seq_ack);

    pk->setName(pkname);
    pk->setAckNumber(nextAckSeq);
}

void CongApp::resetState()
{
    ASSERT(tcpState == CLOSED);
    nextSentSeq = 0;
    nextAskedSeq = 0;
    nextAckSeq = 0;
    nextSeq = 0;
    resentBytes = 0;
    currentBaseRTT = 0;
    cong->reset();
}

void CongApp::finish() {
    if (destAddr == INVALID_ADDRESS)
        return;
    EV << localAddr << " retransmit bytes: " << resentBytes << endl;
    ASSERT(txBuffer.empty());
    ASSERT(rxBuffer.empty());
}

void CongApp::initialize(int stage)
{
    UnicastApp::initialize(stage);
    // ! if this moudle is created by manager in ASSIGN stage, the stages before it will not be executed
    if (stage == INITSTAGE_LOCAL) {
        maxDisorderNumber = par("maxDisorderNumber");
        bandwidth = getParentModule()->par("bandwidth");
        RTOTimeout = new cMessage("RTOTimeout");
        estimatedRTT = par("initRTO");
        // flowInterval = &par("flowInterval");
        messageLength = par("messageLength");
        cong = check_and_cast<CongAlgo*>(getSubmodule("cong"));
        cong->setSegmentSize(messageLength);
        destAddr = par("destAddress");
        destPort = par("destPort");
    }
}

void CongApp::handleMessage(cMessage *msg)
{
    if (msg == RTOTimeout) {
        if (tcpState == TIME_WAIT) {
            tcpState = CLOSED;
            ASSERT(txBuffer.size() == 1);
            auto item = txBuffer.begin()->second;
            delete item.pkt;
            txBuffer.erase(item.seq);
            onConnectionClose();
            return;
        }
        else if (markSeq == nextSentSeq) {
            resendOldestSeq();
        }
        markSeq = nextSentSeq;
        scheduleAfter(2*estimatedRTT, RTOTimeout); // we have to repeadedly check, this will be canceled when appState==finished
    } else {
        UnicastApp::handleMessage(msg);
    }
}

void CongApp::sendPendingData()
{
    // if (last_oldestNotAckedSeq == oldestNotAckedSeq)
    //     resendOldestSeq();

    // ASSERT(!txBuffer.empty());
    auto tx_item_it = txBuffer.begin();
    // auto pkt = tx_item.pkt->dup();
    // auto pktSize = pkt->getByteLength();
    while (tx_item_it != txBuffer.end() && cong->getcWnd() - inflightBytes() >= tx_item_it->second.pktSize) {
        auto& tx_item = tx_item_it->second;
        auto pktSize = tx_item.pktSize;
        if (tx_item.is_sent) {
            if (tx_item.resend_timer == 0) {
                resend(tx_item);
            }
            else {
                tx_item_it++;
                continue; // * move to next seq
            }
        }
        else {
            tx_item.is_sent = true;
            markSeq = nextSentSeq; // ! if markSeq not change for a RTOTimeout, we resend the oldest not acked seq
            nextSentSeq = tx_item.seq + pktSize;
        }
        auto pk = tx_item.pkt->dup();

//        if (localAddr == 789)
//            std::cout << pk->getName() << endl;
        // ! nextAckSeq may keep changing when many ACKs arrive at the same time
        // ! we must set it when sending out
        setPacketBeforeSentOut(pk);
        EV_DEBUG << pk << endl;
        connection->send(pk);
        cong->onSendData(tx_item.seq, pktSize);
        tx_item_it++;
        auto seq = pk->getSeqNumber();
        if (seq == 0) {
            scheduleAfter(estimatedRTT, RTOTimeout);
        }
        if (tcpState == TIME_WAIT) {
            rescheduleAfter(2*estimatedRTT, RTOTimeout);
        }
    }
}

void CongApp::resendOldestSeq()
{
    if (!txBuffer.empty()) {
        auto item = txBuffer.begin()->second;
        ASSERT(item.is_sent);
        resend(item);
    }
}

void CongApp::resend(TxItem& item)
{
    auto pk = item.pkt->dup();
    pk->setResend(true);
    setPacketBeforeSentOut(pk);
    std::cout << "resend " << pk->getName() << endl;
    connection->send(pk);
    item.is_resend_already = true;
    resentBytes += item.pktSize; // ! this will affect inflightBytes
    auto rtt_count = cong->getcWnd() / messageLength;
    item.resend_timer = maxDisorderNumber > rtt_count ? maxDisorderNumber : rtt_count;
}

void CongApp::confirmAckNumber(const Packet* pk)
{
    auto seqNumber = pk->getSeqNumber();
    auto ackNumber = pk->getAckNumber();
    bool isFIN = pk->getFIN();
    if (nextAckSeq == seqNumber) {
        switch (tcpState) {
            case CLOSE_WAIT: // ! receive ACK to FIN
                tcpState = CLOSED;
                break;
            case FIN_WAIT_1:
                tcpState = FIN_WAIT_2;
                break;
            case OPEN:
            case CLOSED: // ! this is only for simple flow app that doesn't want to wait 2MSL
                break;
            default:
                throw cRuntimeError("Unthoughtful state case");
        }
    }
    // ! there are some time between FIN_WAIT_2 and TIME_WAIT because
    // ! last window of packets sent with the same ACK seq, but only the last packet will labeled FIN
    if (isFIN) {// * the other side has received all my packets
        switch (tcpState) {
            case OPEN: // ! only server will receieve FIN when OPEN
                tcpState = CLOSE_WAIT;
                break;
            case FIN_WAIT_2:
                tcpState = TIME_WAIT;
                break;

            case CLOSE_WAIT: // server sent out FIN and received a FIN(duplicate)
            case CLOSED:
                break;
            default:
                throw cRuntimeError("unthought state case");
        }
    }

    if (ackNumber < nextAskedSeq) { // ! the other side want an old sent seq
        EV_DEBUG << "old ack " << ackNumber << endl;
        ASSERT(txBuffer.find(ackNumber) != txBuffer.end());
        txBuffer.at(ackNumber).resend_timer--;
    }
    else {
        nextAskedSeq = ackNumber;
        for (auto& [seq, pkt_buffer]: txBuffer) {
            if (seq < nextAskedSeq) {
                EV_DEBUG << "delete seq " << seq << endl;
                delete pkt_buffer.pkt;
            }
            else {
                break;
            }
        }
        if (!txBuffer.empty()) {
            auto itup = txBuffer.lower_bound(nextAskedSeq);
            txBuffer.erase(txBuffer.begin(), itup); // ! nextAskedSeq will still be in txBuffer
        }
    }
}

void CongApp::confirmSeqNumber(const Packet* pk)
{
    // * measure something
    auto sampleRTT = simTime() - SimTime(pk->getStartTime());
    emit(rttSignal, sampleRTT);
    currentBaseRTT = sampleRTT - pk->getQueueTime() - pk->getTransmitTime();
    estimatedRTT = (1 - 0.125) * estimatedRTT + 0.125 * sampleRTT;
    auto seq = pk->getSeqNumber();
    auto pk_size = pk->getByteLength();
    if (rxBuffer.empty() && seq == 0) {
        // ! server received the first packet
        ASSERT(tcpState==CLOSED);
        tcpState = OPEN;
    }

    if (seq == nextAckSeq) { // ! I get the seq I want
        nextAckSeq += pk_size;
        clearOldSeqInRxBuffer();
    }
}

void CongApp::onReceivedData(Packet* pk)
{
    confirmSeqNumber(pk);
    confirmAckNumber(pk);
    insertRxBuffer(pk);
}

void CongApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    EV_DEBUG << pk << endl;
    auto ackSeq = pk->getAckNumber();
    if (localAddr == 268 && destAddr == 138 && ackSeq == 5110)
        std::cout << pk->getName() << endl;

    cong->onRecvAck(ackSeq, messageLength, pk->getECE()); // let cong algo update cWnd
    onReceivedData(pk);
    // ! we only send FINACK when txBuffer is empty
    if ( txBuffer.empty() && (tcpState ==  TIME_WAIT) ) {
        // char pkname[50];
        auto packet = createDataPacket(1);
        insertTxBuffer(packet);
    }

    if (tcpState == CLOSED) {
        ASSERT(txBuffer.empty());
        ASSERT(rxBuffer.empty());
        onConnectionClose();
        cancelEvent(RTOTimeout);
    }
    else {
        sendPendingData();
    }

}

void CongApp::handleParameterChange(const char *parameterName)
{
    UnicastApp::handleParameterChange(parameterName);
    // this can happen when node change the app destionation
    if (strcmp(parameterName, "destAddress") == 0) {
        destAddr = par("destAddress");
    }
    else if (strcmp(parameterName, "destPort") == 0) {
        destPort = par("destPort");
    }

}

void CongApp::insertTxBuffer(Packet* pk)
{
    pk->setSeqNumber(nextSeq);
    setField(pk);
    txBuffer[nextSeq] = TxItem(pk);
    nextSeq += pk->getByteLength();
}

void CongApp::insertRxBuffer(Packet* pk)
{
    auto seq = pk->getSeqNumber();
    auto pk_size = pk->getByteLength();
    if (seq == nextAckSeq && (tcpState != CLOSED)) {
        delete pk;// ! I get the seq I want, delete the seq is OK
        pk = nullptr;
    }
    else if (seq > nextAckSeq) {
        ASSERT(rxBuffer.find(seq) == rxBuffer.end());
         // ! these seqs arrive too early, store them for now
        rxBuffer[seq] = pk;
    }
    else { // ! seq < nextAckSeq or seq == nextAckSeq but tcpState == CLOSED, which means this is a ACK to FIN
        delete pk;
        pk = nullptr;
    }
}

void CongApp::setField(Packet* pk)
{
    // pk->setDestAddr(destAddr);
    pk->setRound(currentRound);
    // pk->setKind(DATA);
    pk->setECN(false);
    pk->setStartTime(simTime().dbl());
    pk->setTransmitTime(0);
    pk->setQueueTime(0);
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
