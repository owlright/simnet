#include "socket.h"
#include "tcp-dctcp.h"
Define_Module(Socket);

// void
// Socket::SetApp(cSimpleModule* const app)
// {
//     m_app = app;
// }

void Socket::SetSendersNum(int number)
{
    m_sendersNum = number;
}

// Socket::Socket(int src, int dest, uint32_t initCwnd, uint32_t initSSThresh)
// {
//     m_cong = new TcpDctcp(); // todo should be defined by user
//     m_tcb = new TcpSocketState();
//     m_addr = src;
//     m_destAddress = dest;
//     m_sendersNum = 1; // just one receiver
//     // m_tcb->m_initialCwnd = initCwnd;
//     m_tcb->m_cWnd = initCwnd;
//     m_tcb->m_ssThresh = initSSThresh;
//     m_tcb->m_congState = TcpSocketState::CA_OPEN;
//     m_tcb->m_obWnd = m_tcb->m_cWnd;
//     cWnd.setName("congestion window");
//     // packetsSentCountSignal = registerSignal("packetsSentCount");

// }

// Socket::Socket(int src, int dest, int group):
//         Socket(src, dest)
// {
//     m_groupAddr = group;
// }
Socket::~Socket()
{
    delete m_cong;
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

void
Socket::SendData(int packets, int packetBytes)
{
    this->packetNumber = packets;
    this->packetBytes = packetBytes;
    SendPendingData();
}

void
Socket::SendPendingData()
{
    char pkname[40];
    uint32_t nextSeq;
    Packet *pk = nullptr;
    while (AvailableWindow() > 0 && m_tcb->m_sentSize < packetNumber) {
        nextSeq = m_tcb->m_nextTxSequence;
        sprintf(pkname, "DATA-%d-to-%d-seq%u ", m_addr, m_destAddress, nextSeq);
        EV << "sending data packet " << pkname << endl;
        pk = new Packet(pkname);
        pk->setByteLength(packetBytes);
        pk->setKind(1); // 1 is data
        pk->setSeq(nextSeq);
        pk->setSrcAddr(m_addr);
        pk->setDestAddr(m_destAddress);
        pk->setGroupAddr(m_groupAddr);
        pk->setAggrCounter(1);
        m_app->send(pk, "out");
        m_tcb->m_sentSize++;
        m_tcb->m_nextTxSequence++; // ! After the loop m_nextTxSequence is the next packet seq
    }
    EV << "cWnd: " << m_tcb->m_cWnd
       << " total unAck: "<< m_tcb->m_sentSize - m_tcb->m_acked
       << " next seq: " << m_tcb->m_nextTxSequence << endl;
}

void
Socket::Recv(Packet* pk)
{
    auto packetKind = pk->getKind();
    if (packetKind == PacketType::DATA) { // data packet
        ReceivedData(pk);
    }
    else if (packetKind == PacketType::ACK) { // ack packet
        ReceivedAck(pk);
    }
    else {
        assert(false);
    }
}

void
Socket::ReceivedAck(Packet* pk)
{
    auto ackSeq = pk->getAckSeq();
    assert(ackSeq + 1 <= m_tcb->m_nextTxSequence); // ! impossible to receive a ack bigger than have sent
    // EV << "received ack packet " << pk->getName() << endl;

    EV << " ackNumber: " << ackSeq << " next seq: "<< m_tcb->m_nextTxSequence << endl;
    assert (ackSeq + 1 == m_tcb->m_nextTxSequence); // ! no packet loss during simulation
    if (pk->getECN())
    {
        EV << "Received ECN" << endl;
        if (m_tcb->m_congState != TcpSocketState::CA_CWR) {
            EV_WARN << "Congestion happened, half the window" << endl; // very native control
            m_tcb->m_ssThresh = m_cong->GetSsThresh(m_tcb, 0);
            m_tcb->m_cWnd = m_tcb->m_ssThresh;
            m_tcb->m_congState = TcpSocketState::CA_CWR;
        }
        else
        {
            m_tcb->m_congState = TcpSocketState::CA_OPEN; // once no ecn received
        }

    }

    ProcessAck(ackSeq);
    SendPendingData();
}

void
Socket::ProcessAck(const uint32_t& ackNumber)
{
    // EV << "======" << __FUNCTION__ << "======" << endl;
    m_tcb->m_lastAckedSeq = ackNumber;
    m_tcb->m_acked += 1;
    EV << "cWnd: "<< m_tcb->m_cWnd <<" inflight: "<< m_tcb->m_sentSize -  m_tcb->m_acked << endl;
    m_cong->PktsAcked(m_tcb); // todo, update ecn calculation here
    m_cong->IncreaseWindow(m_tcb);
    cWnd.record(m_tcb->m_cWnd); // todo: for debug use only, should change Socket to a SimpleModule and use singals in the future

}

void
Socket::ReceivedData(Packet* pk)
{
    assert(m_addr==pk->getDestAddr());
    int outPortIndex = pk->par("outGateIndex");
    double rate = m_app->getParentModule()->gate("port$o", outPortIndex)->getChannel()->par("datarate");
    EV << pk->getName() <<"comes from port " << outPortIndex << " channelrate is " << rate <<endl;
    auto pkSeq = pk->getSeq();
    m_tcb->m_ackSeq = pkSeq; // ! just ack this packet, do not +1
    if (m_sendersCounter.find(pkSeq)==m_sendersCounter.end())
        m_sendersCounter[pkSeq] = 0;
    m_sendersCounter.at(pkSeq) += pk->getAggrCounter();
    if (m_sendersCounter.at(pkSeq)==m_sendersNum) {
        EV << "all packets of " << pkSeq << " are received" << endl;
        SendEchoAck(pkSeq, pk->getECN(), pk->getGroupAddr());
    }
    else {
        EV << "packet " << pkSeq << " still has " << m_sendersNum - m_sendersCounter.at(pkSeq) << " packets left" << endl;
    }

}

void
Socket::SendEchoAck(uint32_t ackno, bool detectECN, int groupid) {
    char pkname[40];
    sprintf(pkname, "ACK-%d-to-%d-ack%u ", m_addr, m_destAddress, m_tcb->m_ackSeq);
    Packet *ackpk = new Packet(pkname);
    ackpk->setByteLength(10); // ack packet size
    ackpk->setKind(PacketType::ACK);
    ackpk->setSeq(-1); // this is ack packet no data seq
    ackpk->setAckSeq(m_tcb->m_ackSeq); // ack current packet seq, the sender will send next seq
    ackpk->setSrcAddr(m_addr);
    ackpk->setDestAddr(m_destAddress);
    if (detectECN) { // tell receiver congestion happened
        EV << "detect congestion!" <<endl;
        ackpk->setECN(true);
    }
    if (groupid!=-1) {
        ackpk->setGroupAddr(groupid);
    }
    EV << ackpk->getName() << endl;
    m_app->send(ackpk, "out");
}

int
Socket::GetDestAddr() const{
    return this->m_destAddress;
}

int Socket::GetLocalAddr() const
{
    return this->m_addr;
}

void Socket::initialize(int stage)
{

}

void Socket::Init(cSimpleModule* app, int src, int dest, int group, uint32_t initCwnd, uint32_t initSSThresh)
{
    m_app = app;
    m_addr = src;
    m_destAddress = dest;
    m_groupAddr = group;
    m_sendersNum = 1;

    m_cong = new TcpDctcp(); // todo should be defined by user
    m_tcb = new TcpSocketState();
    m_tcb->m_cWnd = initCwnd;
    m_tcb->m_ssThresh = initSSThresh;
    m_tcb->m_congState = TcpSocketState::CA_OPEN;
    m_tcb->m_obWnd = m_tcb->m_cWnd;
    cWnd.setName("congestion window");
}

std::ostream& operator<<(std::ostream& os, const Socket& socket)
{
    os << "destAddress = " << socket.GetDestAddr(); // no endl!
    return os;
}
