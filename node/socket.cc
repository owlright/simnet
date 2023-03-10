#include "socket.h"
#include "tcp-dctcp.h"
Define_Module(Socket);

void Socket::initialize(int stage)
{
    if (stage == Stage::INITSTAGE_LOCAL) {
        m_cong = (TcpCongestionOps*)(getSubmodule("cong"));
        m_tcb = new TcpSocketState();
        m_tcb->m_cWnd = 1;
        m_tcb->m_ssThresh = UINT32_MAX;
        m_tcb->m_congState = TcpSocketState::CA_OPEN;
        m_tcb->m_obWnd = m_tcb->m_cWnd;
        cwndSignal = registerSignal("cwnd");
        emit(cwndSignal, m_tcb->m_cWnd);
    }
}

void Socket::handleMessage(cMessage *msg)
{
    Recv(check_and_cast<Packet*>(msg));
    sendDirect(msg, getParentModule(), "socketIn"); // send to App
}

void Socket::SetSendersNum(int number)
{
    m_sendersNum = number;
}

Socket::~Socket()
{
    delete m_tcb;
}

uint32_t
Socket::AvailableWindow() const
{
    uint32_t win = m_tcb->m_cWnd;
    uint32_t inflight = m_tcb->m_sentSize - m_tcb->m_acked + m_tcb->m_retrans;
    m_tcb->m_bytesInFlight = inflight;
    return (inflight > win) ? 0 : win - inflight;
}

void Socket::SetPacketCommonField(Packet *pk) const
{
    pk->setSrcAddr(m_addr);
    pk->setDestAddr(m_destAddress);
    pk->setGroupAddr(m_groupAddr);
    pk->setAggrCounter(1);
    if (pk->getKind() == PacketType::DATA) {
        pk->setByteLength(packetBytes);
        pk->setSeq(m_tcb->m_nextTxSequence);
    }
    else if (pk->getKind() == PacketType::ACK) {
        pk->setByteLength(10); // ack packet size
        pk->setSeq(-1);
        pk->setAckSeq(m_tcb->m_ackSeq);

    }
    else {
        throw cRuntimeError("invalid packet");
    }
}

void
Socket::Send(int packets, int packetBytes)
{
    Enter_Method_Silent();
    this->packetNumber = packets;
    this->packetBytes = packetBytes;
    SendPendingData();
}

void
Socket::SendPendingData()
{
    char pkname[40];
    Packet* datapk = nullptr;
    while (AvailableWindow() > 0 && m_tcb->m_sentSize != packetNumber) {  // use != not < here because I want if packetNumber is -1, the socket keep sending packets
        auto nextSeq = m_tcb->m_nextTxSequence;
        sprintf(pkname, "DATA-%d-to-%d-seq%u ", m_addr, m_destAddress, nextSeq);
        datapk = new Packet(pkname);
        datapk->setKind(PacketType::DATA);
        SetPacketCommonField(datapk);
        EV << "sending data packet " << datapk->getName() << endl;
        send(datapk, "out");
        m_tcb->m_sentSize++;
        m_tcb->m_nextTxSequence++; // ! After the loop m_nextTxSequence is the next packet seq
    }
    if (m_tcb->m_congState == TcpSocketState::CA_OPEN) {
        m_tcb->m_highTxMark = m_tcb->m_nextTxSequence - 1;
    }
    EV << "cWnd: " << m_tcb->m_cWnd
       << " total unAck: "<< m_tcb->m_highTxMark - m_tcb->m_lastAckedSeq
       << " next seq: " << m_tcb->m_nextTxSequence << endl;
}

void
Socket::Recv(Packet* pk)
{
    m_destAddress = pk->getSrcAddr();
    // do some check
    ASSERT(m_addr == pk->getDestAddr());
    ASSERT(m_groupAddr == pk->getGroupAddr());
    auto packetKind = pk->getKind();
    if (packetKind == PacketType::DATA) { // data packet
        ReceivedData(pk);
    }
    else if (packetKind == PacketType::ACK) { // ack packet
        ReceivedAck(pk);
    }
    else {
        ASSERT(false);
    }
}

void
Socket::ReceivedAck(Packet* pk)
{
    auto ackNumber = pk->getAckSeq();
    ASSERT(ackNumber + 1 <= m_tcb->m_nextTxSequence); // ! impossible to receive a ack bigger than have sent
    EV << " ackNumber: " << ackNumber << " next seq: "<< m_tcb->m_nextTxSequence << endl;
    if (m_tcb->m_congState == TcpSocketState::CA_CWR && ackNumber == m_tcb->m_highTxMark) {
        m_tcb->m_congState = TcpSocketState::CA_OPEN; // a window of data is finished
//        m_cong->IncreaseWindow(m_tcb);
//        emit(cwndSignal, m_tcb->m_cWnd);
    }

    if (pk->getECN()) {
        EV << pk->getName() << "is labeled ECN" << endl;
        m_tcb->m_ecnState = TcpSocketState::ECN_ECE_RCVD;
        // Sender should reduce the Congestion Window as a response to receiver's
        // ECN Echo notification only once per window
        if (m_tcb->m_congState != TcpSocketState::CA_CWR) {
            m_tcb->m_ssThresh = m_cong->GetSsThresh(m_tcb, 0);
            EV << "Reduce ssThresh to " << m_tcb->m_ssThresh << endl;
//            m_cong->IncreaseWindow(m_tcb);
        }
        m_tcb->m_congState = TcpSocketState::CA_CWR;
    }
    else {
        m_tcb->m_ecnState = TcpSocketState::ECN_IDLE;
    }

    m_cong->PktsAcked(m_tcb);
    if (m_tcb->m_congState == TcpSocketState::CA_OPEN) {
        m_cong->IncreaseWindow(m_tcb);
    }
    emit(cwndSignal, m_tcb->m_cWnd);
    m_tcb->m_lastAckedSeq = ackNumber;
    m_tcb->m_acked += 1;
    EV << "cWnd: "<< m_tcb->m_cWnd <<" inflight: "<< m_tcb->m_sentSize -  m_tcb->m_acked << endl;

//    if (m_tcb->m_congState == TcpSocketState::CA_OPEN) {
//        m_cong->IncreaseWindow(m_tcb);
//    }
    SendPendingData();
}

void
Socket::ReceivedData(Packet* pk)
{
    auto pkSeq = pk->getSeq();
    m_tcb->m_ackSeq = pkSeq; // ! just ack this packet, do not +1
    if (m_groupAddr != -1) {
        // the first packet
        if (m_sendersCounter.find(pkSeq)==m_sendersCounter.end())
            m_sendersCounter[pkSeq] = 0;

        m_sendersCounter.at(pkSeq) += pk->getAggrCounter();
        if (m_sendersCounter.at(pkSeq)==m_sendersNum) { // ! check if all packets about seq arrive
            EV << "packet" << pkSeq << " are received" << endl;
            m_sendersCounter.erase(pkSeq); // reset the counter
            SendAck(pkSeq, pk->getECN());
        }
        else {
            EV << "packet " << pkSeq << " still has " << m_sendersNum - m_sendersCounter.at(pkSeq) << " packets left" << endl;
        }
    }
    else {
        EV << "packet" << pkSeq << " are received" << endl;
        SendAck(pkSeq, pk->getECN()); // if this packet is labeled ECN just send it back let sender konw
    }

}

void
Socket::SendAck(uint32_t ackno, bool detectECN) {
    char pkname[40];
    sprintf(pkname, "ACK-%d-to-%d-ack%u ", m_addr, m_destAddress, m_tcb->m_ackSeq);
    Packet *ackpk = new Packet(pkname);

    ackpk->setKind(PacketType::ACK);
    SetPacketCommonField(ackpk);
    if (detectECN) { // tell receiver congestion happened
        EV << "Set ack's ECN" <<endl;
        ackpk->setECN(true);
    }
    EV << "sending ackpacket " << ackpk->getName() << endl;
    send(ackpk, "out");

}

void Socket::Bind(int srcaddr, int destaddr, int groupaddr)
{
    m_addr = srcaddr;
    m_destAddress = destaddr;
    m_groupAddr = groupaddr;
}

int Socket::GetDestAddr() const
{
    return this->m_destAddress;
}

int Socket::GetLocalAddr() const
{
    return this->m_addr;
}

std::ostream& operator<<(std::ostream& os, const Socket& socket)
{
    os << "destAddress = " << socket.GetDestAddr(); // no endl!
    return os;
}
