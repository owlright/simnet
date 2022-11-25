#include "socket.h"
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
    m_cong = new TcpReno();
    m_tcb = new TcpSocketState();
    m_addr = src;
    m_destAddress = dest;
    // m_tcb->m_initialCwnd = initCwnd;
    m_tcb->m_cWnd = initCwnd;
    m_tcb->m_ssThresh = initSSThresh;
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
Socket::Retransmit(uint32_t seq)
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
    // assert(pk->getAckSeq() == m_tcb->m_seq + 1);
    m_tcb->m_ackedSeq = pk->getAckSeq();
    if (m_tcb->m_ackedSeq + 1 <= m_tcb->m_seq) {
        m_tcb->m_acked++;
        if (m_tcb->m_ackedSeq + 1 == m_tcb->m_seq) { // ask exactly what I want to send
            m_tcb->m_cWnd++;
        }
        Send();
    }
    else { //! the code below should not be triggered
        Retransmit(m_tcb->m_ackedSeq);
    }

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
    if (pkSeq == m_tcb->m_ackSeq + 1) {
        // m_tcb->m_seq = pkSeq;
        m_tcb->m_ackSeq = pkSeq;
    }

    char pkname[40];
    sprintf(pkname, "ACK-%d-to-%d-ack%u ", m_addr, m_destAddress, m_tcb->m_ackSeq);
    Packet *ackpk = new Packet(pkname);
    ackpk->setByteLength(10); // ack packet size
    ackpk->setKind(0);
    ackpk->setSeq(-1); // this is ack packet no data seq
    ackpk->setAckSeq(m_tcb->m_ackSeq); // ack current packet seq, the sender will send next seq
    ackpk->setSrcAddr(m_addr);
    ackpk->setDestAddr(m_destAddress);
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
