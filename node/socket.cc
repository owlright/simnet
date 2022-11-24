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
    // auto availableWindow = AvailableWindow();
    // if (availableWindow > 0) {
    //     // send the packet

    // }
    // UpdateRttHistory(seq, sz, isRetransmission);
}

void
Socket::Send()
{
    char pkname[40];
    uint32_t nextSeq;
    Packet *pk = nullptr;
    while (AvailableWindow() > 0) {
        nextSeq = m_tcb->m_seq;
        sprintf(pkname, "pk-%d-to-%d-seq%u ", m_addr, m_destAddress, nextSeq);
        EV << "sending data packet " << pkname << endl;
        pk = new Packet(pkname);
        pk->setByteLength(packetBytes);
        pk->setKind(1); // 1 is data
        pk->setSeq(nextSeq);
        pk->setSrcAddr(m_addr);
        pk->setDestAddr(m_destAddress);
        m_app->send(pk, "out");
        m_tcb->m_sentSize++;
        m_tcb->m_seq++;
    }
}
// void
// Socket::ReceieveAck(Packet*)
// {
//     // m_cong->CwndEvent(m_tcb, );
//     // m_cong->CongControl();
// }

void
Socket::ProcessAck(Packet* pk)
{
    EV << "received ack packet " << pk->getName() << endl;
    // assert(pk->getAckSeq() == m_tcb->m_seq + 1);
    m_tcb->m_ackedSeq = pk->getAckSeq();
    if (m_tcb->m_seq <= m_tcb->m_ackedSeq) {
        m_tcb->m_cWnd++;
        Send();
    }
    m_tcb->m_acked++;
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

    char pkname[40];
    sprintf(pkname, "Ack-%d-to-%d-ackseq%u ", m_addr, m_destAddress, pkSeq+1);
    Packet *ackpk = new Packet(pkname);
    ackpk->setByteLength(1); // ack packet size
    ackpk->setKind(0);
    ackpk->setAckSeq(pkSeq+1); // want next packet
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
