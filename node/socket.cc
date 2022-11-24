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
    m_tcb->m_initialCwnd = initCwnd;
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
    char pkname[40];
    sprintf(pkname, "pk-%d-to-%d-#%ld", m_addr, m_destAddress, (long) 1);
    EV << "generating packet and build socket " << pkname << endl;
    Packet *pk = new Packet(pkname);
    pk->setByteLength(packetBytes);
    pk->setKind(1); // 1 is data
    pk->setSeq(1);
    pk->setSrcAddr(m_addr);
    pk->setDestAddr(m_destAddress);
    m_app->send(pk, "out");
    // auto availableWindow = AvailableWindow();
    // if (availableWindow > 0) {
    //     // send the packet

    // }
    // UpdateRttHistory(seq, sz, isRetransmission);
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
    delete pk;
}

void
Socket::ProcessData(Packet* pk)
{
    assert(m_addr==pk->getSrcAddr());
    int outPortIndex = pk->par("outGateIndex");
    double rate = m_app->getParentModule()->gate("port$o", outPortIndex)->getChannel()->par("datarate");
    EV << pk->getName() <<"comes from port " << outPortIndex << " channelrate is " << rate <<endl;
    char pkname[40];
    auto pkSeq = pk->getSeq();
    sprintf(pkname, "Ack-%d-to-%d-#%ld", m_addr, m_destAddress, pkSeq);
    Packet *ackpk = new Packet(pkname);
    EV << pk->getName() << ackpk->getName() << endl;
    ackpk->setByteLength(1);
    ackpk->setKind(0);
    ackpk->setAckSeq(pkSeq);
    ackpk->setSrcAddr(m_addr);
    ackpk->setDestAddr(m_destAddress);
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
