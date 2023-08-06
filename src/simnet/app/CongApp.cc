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
                auto pkt = tx_item.pkt->dup();
                pkt->setResend(true);
                resentBytes += pktSize;
                auto rtt_count = cong->getcWnd() / messageLength;
                tx_item.resend_timer = maxDisorderNumber > rtt_count ? maxDisorderNumber : rtt_count; // wait for about a RTT
                tx_item.is_resend_already = true;
                EV_DEBUG << "resend seq " << pkt->getSeqNumber() << endl;
            }
            else {
                tx_item_it++;
                continue; // * move to next seq
            }
        }
        else {
            tx_item.is_sent = true;
            markSeq = nextSentSeq;
            nextSentSeq = tx_item.seq + pktSize;
        }
        auto pk = tx_item.pkt->dup();
        char pkname[50];
        auto dest_addr = pk->getDestAddr();
        auto seq = pk->getSeqNumber();
        if (pk->getFIN() && pk->getFINACK() ) {
            sprintf(pkname, "FIN-FINACK-%" PRId64 "-to-%" PRId64 "-seq-%" PRId64 "-ack-%" PRId64,
                                        localAddr, dest_addr, seq, nextAckSeq);
            transitTcpStateOnEvent(SEND_FIN);
            transitTcpStateOnEvent(SEND_FINACK);
        }
        else if (pk->getFIN() && !pk->getFINACK()) {
            sprintf(pkname, "FIN-%" PRId64 "-to-%" PRId64 "-seq-%" PRId64 "-ack%-" PRId64,
                            localAddr, dest_addr, seq, nextAckSeq);
            transitTcpStateOnEvent(SEND_FIN);
        }
        else if (pk->getFINACK()  && !pk->getFIN()) {
            sprintf(pkname, "FINACK-%" PRId64 "-to-%" PRId64 "-seq-%" PRId64 "-ack%-" PRId64,
                            localAddr, dest_addr, seq, nextAckSeq);
            transitTcpStateOnEvent(SEND_FINACK);
        }
        else if (pk->getPacketType() == ACK) {
            sprintf(pkname, "ACK-%" PRId64 "-to-%" PRId64 "-seq-%" PRId64 "-ack-%" PRId64,
            localAddr, dest_addr, seq, nextAckSeq);
        }
        else if (pk->getPacketType() == MACK) {
            sprintf(pkname, "MACK-%" PRId64 "-to-%" PRId64 "-seq-%" PRId64 "-ack-%" PRId64,
            localAddr, dest_addr, seq, nextAckSeq);
        }
        else {
            sprintf(pkname, "DATA-round-%d-%" PRId64 "-to-%" PRId64 "-seq-%" PRId64 "-ack%-" PRId64,
                    currentRound, localAddr, dest_addr, seq, nextAckSeq);
        }

        pk->setName(pkname);
        if (localAddr == 268 && destAddr == 138)
            std::cout << pkname << endl;

        pk->setAckNumber(nextAckSeq);
        EV_DEBUG << pk << endl;
        connection->send(pk);
        cong->onSendData(tx_item.seq, pktSize);
        tx_item_it++;
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
    auto entry = txBuffer.begin()->second;
    auto pk = entry.pkt->dup();
    pk->setResend(true);
    connection->send(pk);
    entry.is_resend_already = true;
    resentBytes += entry.pktSize; // ! this will affect inflightBytes
    auto rtt_count = cong->getcWnd() / messageLength;
    entry.resend_timer = maxDisorderNumber > rtt_count ? maxDisorderNumber : rtt_count;

}

void CongApp::onReceivedAck(const Packet* pk)
{
    auto ack_seq = pk->getAckNumber();
    if (txBuffer.empty() && ack_seq == 0) {
        // ! we should bind srcAddr and srcPort here,
        // ! but for mulitcast applications, there are many srcAddrs
        tcpState = OPEN;
    }
    if (ack_seq < nextAskedSeq) { // ! redundant ack
        EV_DEBUG << "old ack " << ack_seq << endl;
        txBuffer.at(ack_seq).resend_timer--;
        return;
    }

    nextAskedSeq = ack_seq;
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
//        last_oldestNotAckedSeq = oldestNotAckedSeq;
//        oldestNotAckedSeq = txBuffer.begin()->first;
    }
}

void CongApp::onReceivedData(const Packet* pk)
{
    // * measure something
    auto sampleRTT = simTime() - SimTime(pk->getStartTime());
    emit(rttSignal, sampleRTT);
    currentBaseRTT = sampleRTT - pk->getQueueTime() - pk->getTransmitTime();
    estimatedRTT = (1 - 0.125) * estimatedRTT + 0.125 * sampleRTT;
}

void CongApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    EV_DEBUG << pk << endl;
    auto ackSeq = pk->getAckNumber();
    if (localAddr == 268 && destAddr == 138 && ackSeq == 5110)
        std::cout << pk->getName() << endl;
    bool is_FIN = pk->getFIN();
    bool is_FINACK = pk->getFINACK();

    cong->onRecvAck(ackSeq, messageLength, pk->getECE()); // let cong algo update cWnd

    onReceivedAck(pk);
    onReceivedData(pk);
    insertRxBuffer(pk); // ! pk maybe deleted here, do not use it below

    if (is_FIN) {// * the other side has received all my packets
        transitTcpStateOnEvent(RECV_FIN);
    }

    if (is_FINACK) {
        transitTcpStateOnEvent(RECV_FINACK);
    }

    if ( txBuffer.empty() && (tcpState ==  TIME_WAIT || tcpState == CLOSE_WAIT) ) {
        char pkname[50];
        auto packet = new Packet();
        sprintf(pkname, "FINACK-%" PRId64 "-to-%" PRId64 "-seq-%" PRId64 "-ack-%" PRId64,
        localAddr, destAddr, nextSeq, ackSeq);
        packet->setName(pkname);
        packet->setFIN(true);
        packet->setFINACK(true);
        packet->setByteLength(64);
        insertTxBuffer(packet);
    }
    else if (tcpState == CLOSED) {
        ASSERT(txBuffer.empty());
        onConnectionClose();
        cancelEvent(RTOTimeout);
    }
    else {
        sendPendingData();
    }

}

void CongApp::transitTcpStateOnEvent(const TcpEvent_t& event)
{
    switch (event) {
        case SEND_FIN:
            switch (tcpState) {
                case OPEN: // * client
                    tcpState = FIN_WAIT_1;
                    break;

                case CLOSE_WAIT: // * server
                    tcpState = LAST_ACK;
                    break;

                default:
                    cRuntimeError("unknown state.");
            }
            break;

        case RECV_FIN:
            switch (tcpState) {
                case OPEN: // server may still have data to send
                    tcpState = CLOSE_WAIT;
                    break;

                case CLOSE_WAIT: // ? received mupltiple FINs
                    tcpState = CLOSE_WAIT;
                    break;

                case FIN_WAIT_1:
                case FIN_WAIT_2:
                    tcpState = TIME_WAIT;
                    break;

                default:
                    throw cRuntimeError("unthought case");
            }
            break;

        case RECV_FINACK:
            switch (tcpState) {
                case FIN_WAIT_1: // * client
                    tcpState = FIN_WAIT_2;
                case TIME_WAIT:
                case CLOSE_WAIT: // * server
                    tcpState = CLOSED;
                    break;

                case LAST_ACK: // * server
                    tcpState = CLOSED;
                    break;

                default:
                    cRuntimeError("unknown state.");
            }
            break;

        default:
            cRuntimeError("unknown state.");
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

void CongApp::insertRxBuffer(Packet* pk)
{
    auto seq = pk->getSeqNumber();
    auto pk_size = pk->getByteLength();
    if (rxBuffer.find(seq) == rxBuffer.end() && !pk->getFINACK()) {
        // ! these seqs arrive too early, store them for now
        rxBuffer[seq] = pk;
    }
    else {
        delete pk; // redundant ack or the last ack packet
    }

    if (seq <= nextAckSeq) { // ! we can safely remove some confirmed tx packets
        auto remove_seq = seq;
        auto remove_pk_size = 0;
        while (rxBuffer.find(remove_seq) != rxBuffer.end()) {
            remove_pk_size = rxBuffer[remove_seq]->getByteLength();
            delete rxBuffer[remove_seq];
            rxBuffer.erase(remove_seq);
            remove_seq = remove_seq - remove_pk_size;
        }
        nextAckSeq = seq + pk_size;
    }
}

void CongApp::insertTxBuffer(Packet* pk)
{
    pk->setSeqNumber(nextSeq);
    setField(pk);
    txBuffer[nextSeq] = TxItem(pk);
    nextSeq += pk->getByteLength();
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
