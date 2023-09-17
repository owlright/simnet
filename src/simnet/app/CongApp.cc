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
simsignal_t CongApp::cwndSignal = registerSignal("cwnd");

int CongApp::maxDisorderNumber = 0;

CongApp::~CongApp() {
    cancelAndDelete(RTOTimeout);
    for (auto& [seq, item] : txBuffer) {
        cancelAndDelete(item.pkt);
    }
    for (auto& [seq, pk] : rxBuffer) {
        cancelAndDelete(pk);
    }
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

void CongApp::echoACK(SeqNumber seq)
{
    auto packet = new Packet();
    packet->setDestAddr(destAddr);
    packet->setDestPort(destPort);
    packet->setByteLength(64);
    packet->setPacketType(ACK);
    packet->setSeqNumber(seq);
    packet->setAckNumber(getNextAckSeq());  // * let other side clear its txBuffer
    insertTxBuffer(packet);
    auto& item = txBuffer.at(seq);
    sendFirstTime(item);
    // ! we think ACK will can't be lost
    delete item.pkt;
    txBuffer.erase(seq);
}


void CongApp::setBeforeSentOut(TxItem& item)
{

    auto pk = item.pkt;
    item.sendTime = simTime();
    auto dest_addr = pk->getDestAddr();
    auto seq = pk->getSeqNumber();
    // ! prepare the packet name
    char pkname[50] = {};
    if (pk->getResend())
        sprintf(pkname, "Resend-");

    char fin_prefix[10] = {};
    if (pk->getFIN()) {
        sprintf(fin_prefix, "FIN-");
        switch (tcpState) {
        case OPEN:
            tcpState = FIN_WAIT_1;
            break;
       case CLOSE_WAIT:
           cancelEvent(RTOTimeout);
            tcpState = LAST_ACK;
            break;
        // ! send duplicate FINs does not change state
        case FIN_WAIT_1:
        case FIN_WAIT_2:
        case TIME_WAIT:
        case LAST_ACK:
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
    // ! nextAckSeq may keep changing when many ACKs arrive at the same time
    // ! we must set it when sending out
    pk->setAckNumber(nextAckSeq);
    if (nextAckSeq > maxSentAckNumber) {
        maxSentAckNumber = nextAckSeq;
    }
}

void CongApp::resetState()
{
    ASSERT(tcpState == CLOSED);
    nextSentSeq = 0;
    nextAskedSeq = 0;
    nextAckSeq = 0;
    nextSeq = 0;
    // resentBytes = 0;
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
    ConnectionApp::initialize(stage);
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
        resendTimeoutSeqs();
        if (nextSentSeq == lastSentSeqMark) {
            // ! resend the oldest packet immediately
            if (!txBuffer.empty()) {
                auto& item = txBuffer.begin()->second;
                if (!item.is_sent) {
                    throw cRuntimeError("Why not sent any packet in last rtt?");
                }
                if ((simTime() - item.sendTime) > 5*estimatedRTT && !item.is_resend_already) {
                    // TODO is this always right?
                    // ! we believe resend packets will always be accepted
                    resend(item);
                }
            }
        }
        lastSentSeqMark = nextSentSeq;
        scheduleAfter(estimatedRTT, RTOTimeout); // we have to repeadedly check
    } else {
        ConnectionApp::handleMessage(msg);
    }
}

void CongApp::sendPendingData()
{
    while (cong->getcWnd() - inflightBytes() >= messageLength) {
        auto pk = createDataPacket();    // * when there is room, ask the app for more data
        if (pk != nullptr) {
            insertTxBuffer(pk);
            auto seq = pk->getSeqNumber();
            auto& item = txBuffer.at(seq);
            nextSentSeq = seq + item.pktSize; // ! affect inflightBytes
            item.is_sent = true;
            auto& destAddresses = pk->getAllDestAddresses();
            if (destAddresses.empty()) {
                sendFirstTime(item);
            } else {
                // * send copy of packets to different destAddresses
                for (auto& dst : destAddresses) {
                    TxItem tmp(item);
                    tmp.pkt->setDestAddr(dst);
                    sendFirstTime(tmp);
                }
                item.sendTime = simTime();  // ! note multicast send is tmp, not tx_item!
            }
        } else {
            break;
        }
    }
    rescheduleAfter(estimatedRTT, RTOTimeout);
}

void CongApp::resendTimeoutSeqs()
{
    for (auto& [seq, item] : txBuffer) {
        if (item.is_sent && simTime() - item.sendTime > 10*estimatedRTT) {
            ASSERT(item.sendTime > 0);
            resend(item);
        } else {
            break;
        }
    }
}

void CongApp::resend(TxItem& item)
{
    // if ((localAddr==272 || localAddr==399||localAddr==527||localAddr==143||localAddr==394||localAddr==401||localAddr==271||localAddr==786||localAddr==659) && item.seq == 7980000) {
    //     std::cout << simTime() << " resend " << localAddr << " " << item.seq << endl;
    // }
    if (!item.is_sent) {
        throw cRuntimeError("You can't resend a not sent packet.");
    }
    if (item.destAddresses.empty()) {
        auto pk = item.pkt;
        pk->setResend(true);
        setBeforeSentOut(item);
        connection->send(pk->dup());
    }
    else {
        for (auto& dst:item.destAddresses) {
            auto pk = item.pkt;
            pk->setDestAddr(dst);
            pk->setResend(true);
            setBeforeSentOut(item);
            connection->send(pk->dup());
        }
    }

    item.is_resend_already = true;
    resentBytes += item.pktSize; // ! this will affect inflightBytes
    auto rtt_count = cong->getcWnd() / messageLength;
    item.resend_timer = maxDisorderNumber > rtt_count ? maxDisorderNumber : rtt_count;
}

void CongApp::sendFirstTime(TxItem& item)
{
    ASSERT(item.destAddresses.empty());
    auto pk = item.pkt;
    setBeforeSentOut(item);
    EV_DEBUG << pk << endl;
    connection->send(pk->dup());
    cong->onSendData(item.seq, item.pktSize);
}

void CongApp::confirmAckNumber(const Packet* pk)
{
    auto ackNumber = pk->getAckNumber();
    // ! we ignore ackNumber==0 here, because if initWindow is >= maxDisorderNumber
    // ! then seq==0 will always be resent, but since we sent is pure ack, this may not be a big deal
    if (ackNumber != 0 && ackNumber <= nextAskedSeq) { // ! the other side want an old sent seq
        EV_DEBUG << "old ack " << ackNumber << endl;
        // ! 1. the ackNumber is a sent but not confirmed seq(it should be the oldest)
        // ! 2. the ackNumber is too old, and the sender must have received this seq, otherwise
        // !    our nextAskedSeq cannot move to next, so do nothing about this ack
        auto it = txBuffer.find(ackNumber);
        if (it != txBuffer.end()) {
            auto& reitem = txBuffer.at(ackNumber);

            reitem.resend_timer--;
            if (reitem.resend_timer <= 0) {
//                if (localAddr == 525 && localPort==2000)
                // std::cout << simTime() << " " << localAddr << " " << it->second.sendTime << " " << reitem.seq <<endl;
                ASSERT(ackNumber==reitem.seq);
                resend(reitem); // ! sendTime will be updated here
            }
        }
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
    auto seq = pk->getSeqNumber();

    // ! transmit tcpState
    if (seq == nextAckSeq) {
        switch (tcpState) {
            case CLOSE_WAIT: // ! case 1: duplicate FINs arrive server
                break;
            case LAST_ACK: // ! receive ACK to FIN
                tcpState = CLOSED;
                break;
            case FIN_WAIT_1:
                tcpState = FIN_WAIT_2;
                break;
            case OPEN: // keep OPEN
            case FIN_WAIT_2:
                // ! case 1: we receieved ack but not server's FIN
            case TIME_WAIT:
                // ! case 1: duplicate FINs arrive client
            case CLOSED:
                // ! case 1: simple flow app that doesn't want to wait 2MSL
                break;
            default:
                throw cRuntimeError("Unthoughtful state case");
        }
    }
    // // ! there are some time between FIN_WAIT_2 and TIME_WAIT because
    // // ! last window of packets sent with the same ACK seq, but only the last packet will labeled FIN
    if (pk->getFIN()) {// * the other side has received all my packets
        switch (tcpState) {
            case OPEN: // ! only server will receieve FIN when OPEN
                tcpState = CLOSE_WAIT;
                break;
            case FIN_WAIT_2:
                tcpState = TIME_WAIT;
                break;

            case FIN_WAIT_1:
            case CLOSE_WAIT: // server sent out FIN and received a FIN(duplicate)
            case CLOSED:
                break;
            default:
                throw cRuntimeError("unthought state case");
        }
    }
}

void CongApp::onReceivedNewPacket(Packet* pk)
{
    confirmSeqNumber(pk);
    confirmAckNumber(pk);
    insertRxBuffer(pk);
}

void CongApp::onReceivedDuplicatedPacket(Packet* pk)
{
    confirmAckNumber(pk); // ! old seq but may carry new ackNumber
}

void CongApp::connectionDataArrived(Connection *connection, Packet* pk)
{
    EV_DEBUG << pk << endl;
    auto ackSeq = pk->getAckNumber(); // * other side want us's seq
    auto seq = pk->getSeqNumber();
    if (nextSentSeq == 0 && ackSeq == 0) {
        // ! server received the first packet
        ASSERT(tcpState==CLOSED);
        tcpState = OPEN;
    }
    if (tcpState == CLOSED) {
        // ! in a many to one connection, there may be many last ACK to FIN
        // ! only deal with the first one arrived at tcpState==LAST_ACK, ignore the others
        delete pk;
        return;
    }
    // if (localAddr==138 && localPort == 2000) {
    //      std::cout << simTime() << CYAN << " " << localAddr << " " << pk->getName() << " " << estimatedRTT.inUnit(SIMTIME_US)<< " " << nextAckSeq << ENDC;
    //  }
//    if ( localPort == 3000) {
//         std::cout << simTime() << BLUE << " " << localAddr << " " << pk->getName() << " " << estimatedRTT.inUnit(SIMTIME_US)<< " " << nextAckSeq << ENDC;
//     }
    cong->onRecvAck(ackSeq, messageLength, pk->getECE()); // let cong algo update cWnd

    // * do something every RTT only once on successful packet
    auto guessAckMySeq = ackSeq - messageLength;
    if (guessAckMySeq < 0) guessAckMySeq = 0;

    if (ackSeq > 0 && seq == nextAckSeq && !pk->getResend()  && txBuffer.find(guessAckMySeq) != txBuffer.end()) {
        auto& item = txBuffer.at(guessAckMySeq);
        ASSERT(item.is_sent);
        if (!item.is_resend_already) {
            // ! we don't know if this ack is for the first packet or the resent packet
            // ! resent packet maybe just sent out
            auto sampleRTT = simTime() - item.sendTime;
            currentBaseRTT = sampleRTT - pk->getQueueTime() - pk->getTransmitTime();
            estimatedRTT = (1 - 0.125) * estimatedRTT + 0.125 * sampleRTT;
            emit(rttSignal,  sampleRTT);
            emit(cwndSignal, cong->getcWnd());
        }
    }

    if (seq >= getNextAckSeq() && rxBuffer.find(seq) == rxBuffer.end()) { // we get new seqs
        if (pk->getPacketType() == ACK && tcpState==LAST_ACK) {
            // this is a ACK to FIN;
            confirmSeqNumber(pk); // just confirm it, don't insert rxBuffer
            confirmAckNumber(pk);
            delete pk;
        }
        else {
            onReceivedNewPacket(pk);
        }
    } else {
        onReceivedDuplicatedPacket(pk);

        delete pk; // duplicate seqs, just delete it
    }
    // ! only client need to ACK the FIN seq
    if ( txBuffer.empty() && (tcpState == TIME_WAIT) ) {
        echoACK(getNextSentSeq());
        tcpState = CLOSED; // ! we assume ack will never be lost
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
    ConnectionApp::handleParameterChange(parameterName);
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
    setField(pk);
    pk->setSeqNumber(nextSeq);
    txBuffer[nextSeq] = TxItem(pk);
    nextSeq += messageLength;
//    ASSERT(pk->getDestAddr() != INVALID_ADDRESS); // cannot check this because of Parameter Server's broadcast behaviour
}

void CongApp::insertRxBuffer(Packet* pk)
{
    auto seq = pk->getSeqNumber();
    auto pk_size = pk->getByteLength();
    if (seq == nextAckSeq) {
        delete pk; // ! I get the seq I want, so it's OK to delete it
        // * move nextAckSeq to the next seq I haven't received yet.
        // * and delete those early arrived packets
        nextAckSeq += pk_size;
        while (rxBuffer.find(nextAckSeq) != rxBuffer.end()) {
            pk_size = rxBuffer[nextAckSeq]->getByteLength();
            delete rxBuffer[nextAckSeq];
            rxBuffer.erase(nextAckSeq);
            nextAckSeq += pk_size;
        }
    }
    else if (seq > nextAckSeq && tcpState != CLOSED) {
        // ! when tcpState == CLOSED, if more resend packets with FIN arrive, don't store them
        // ! these seqs arrive too early, store them for now
        ASSERT(rxBuffer.find(seq) == rxBuffer.end());
        rxBuffer[seq] = pk;
    }
}

void CongApp::setField(Packet* pk)
{
    // pk->setDestAddr(destAddr);
    pk->setRound(currentRound);
    // pk->setKind(DATA);
    pk->setECN(false);
    // pk->setSendTime(simTime().dbl());
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
