#include "socket.h"
#include "tcp-reno.h"
// void
// Socket::SetSendCb(Callback f)
// {
//     Send=f;
// }
void
Socket::SetApp(cSimpleModule* const app)
{
    m_app = app;
}

Socket::Socket(int src, int dest, uint32_t initCwnd, uint32_t initSSThresh)
{
    m_cong = new TcpReno(); // todo should be defined by user
    m_tcb = new TcpSocketState();
    m_addr = src;
    m_destAddress = dest;
    // m_tcb->m_initialCwnd = initCwnd;
    m_tcb->m_cWnd = initCwnd;
    m_tcb->m_ssThresh = initSSThresh;
    m_tcb->m_congState = TcpSocketState::CA_OPEN;
    m_tcb->m_nextWinBeg = m_tcb->m_cWnd; // ! set the first window right edge
    m_tcb->m_obWnd = m_tcb->m_cWnd;
    // packetsSentCountSignal = registerSignal("packetsSentCount");
    cwnd.setName("congestion window");
}

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
    Send();
}

void
Socket::Send()
{
    char pkname[40];
    uint32_t nextSeq;
    Packet *pk = nullptr;
    while (AvailableWindow() > 0 && m_tcb->m_sentSize < packetNumber) {
        nextSeq = m_tcb->m_seq;
        sprintf(pkname, "DATA-%d-to-%d-seq%u ", m_addr, m_destAddress, nextSeq);
        EV << "sending data packet " << pkname << endl;
        pk = new Packet(pkname);
        pk->setByteLength(packetBytes);
        pk->setKind(1); // 1 is data
        pk->setSeq(nextSeq);
        pk->setSrcAddr(m_addr);
        pk->setDestAddr(m_destAddress);
        m_app->send(pk, "out");
        m_tcb->m_sentSize++;
        m_tcb->m_seq++; // ! next packet seq
    }
}

void
Socket::Retransmit(uint32_t seq) // ! this function is not ready, do not use
{
    char pkname[40];
    sprintf(pkname, "DATA-%d-to-%d-seq%u ", m_addr, m_destAddress, seq);
    EV << "sending retransmitted data packet " << pkname << endl;
    Packet *pk = new Packet(pkname);
    pk->setByteLength(packetBytes);
    pk->setKind(1); // 1 is data
    pk->setSeq(seq);
    pk->setSrcAddr(m_addr);
    pk->setDestAddr(m_destAddress);
    m_app->send(pk, "out");
}

void
Socket::ProcessAck(Packet* pk)
{
    EV << "received ack packet " << pk->getName() << endl;
    EV << "Current window: "<< m_tcb->m_cWnd <<" Inflight packets: "<< m_tcb->m_sentSize -  m_tcb->m_acked << endl;
    EV << "acked: " << pk->getAckSeq() << " current seq: "<< m_tcb->m_seq << endl;
    m_tcb->m_acked += 1;
    m_tcb->m_cWnd++;
    auto ackSeq = pk->getAckSeq();
    assert(ackSeq + 1 <= m_tcb->m_seq); // ! can't receive a ack no bigger than have sent
    assert (ackSeq == m_tcb->m_lastAckedSeq + 1); // ! no packet loss during simulation
    if (pk->getECN()) {
        m_tcb->m_congState = TcpSocketState::CA_CWR;
        m_tcb->m_ackedBytesEcn += pk->getByteLength();
    }

    if (ackSeq + 1 >= m_tcb->m_nextWinBeg) { // ! after receive a rtt bytes
        // EV << "packets has already sent: "<< m_tcb->m_sentSize << endl;
        if (m_tcb->m_congState == TcpSocketState::CA_CWR) {
            EV_WARN << "Observe Window: " << m_tcb->m_obWnd << endl;
            EV_WARN << "Congestion happened, half the window" << endl; // very native control
            if ((m_tcb->m_cWnd)>>1 == 0 ) {
                EV_WARN << "window too small" << endl;
                m_tcb->m_cWnd = 1;
            } else {
                m_tcb->m_cWnd = m_tcb->m_cWnd >> 1;
            }
            EV_WARN << "After half the window: " << m_tcb->m_cWnd << endl;
            EV_WARN << "Receive congestion bytes: " << m_tcb->m_ackedBytesEcn<<endl;
            // ! reset state
            m_tcb->m_obWnd = m_tcb->m_seq - m_tcb->m_nextWinBeg;
            m_tcb->m_congState = TcpSocketState::CA_OPEN; // reset state to normal
            m_tcb->m_nextWinBeg = m_tcb->m_seq;
            m_tcb->m_ackedBytesEcn = 0;
        }


    }


    m_tcb->m_lastAckedSeq = ackSeq;
    Send();
    cwnd.record(m_tcb->m_cWnd); // todo: for debug use only, should change Socket to a SimpleModule and use singals
    delete pk;
}

void
Socket::ProcessData(Packet* pk)
{
    assert(m_addr==pk->getSrcAddr());
    int outPortIndex = pk->par("outGateIndex");
    double rate = m_app->getParentModule()->gate("port$o", outPortIndex)->getChannel()->par("datarate");
    EV << pk->getName() <<"comes from port " << outPortIndex << " channelrate is " << rate <<endl;
    auto pkSeq = pk->getSeq();
    m_tcb->m_ackSeq = pkSeq; // ! just ack this packet, do not +1

    char pkname[40];
    sprintf(pkname, "ACK-%d-to-%d-ack%u ", m_addr, m_destAddress, m_tcb->m_ackSeq);
    Packet *ackpk = new Packet(pkname);
    ackpk->setByteLength(10); // ack packet size
    ackpk->setKind(0);
    ackpk->setSeq(-1); // this is ack packet no data seq
    ackpk->setAckSeq(m_tcb->m_ackSeq); // ack current packet seq, the sender will send next seq
    ackpk->setSrcAddr(m_addr);
    ackpk->setDestAddr(m_destAddress);
    if (pk->getECN()) {
        EV << "detect congestion!" <<endl;
        ackpk->setECN(true);
    }
    EV << ackpk->getName() << endl;
    m_app->send(ackpk, "out");
    delete pk;
}

int
Socket::GetDestAddr() const{
    return this->m_destAddress;
}

std::ostream& operator<<(std::ostream& os, const Socket& socket)
{
    os << "destAddress = " << socket.GetDestAddr(); // no endl!
    return os;
}
