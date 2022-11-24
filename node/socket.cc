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
    m_addr = src;
    m_tcb->m_initialCwnd = initCwnd;
    m_tcb->m_ssThresh = initSSThresh;
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
    sprintf(pkname, "pk-%d-to-%d-#%d", m_addr, m_destAddress, 1);
    EV << "generating packet and build socket" << pkname << endl;
    Packet *pk = new Packet(pkname);
    pk->setByteLength(packetBytes);
    pk->setKind(1); // 1 is data
    pk->setSeq(1);
    pk->setSrcAddr(m_addr);
    pk->setDestAddr(m_destAddress);
    m_app->send(pk, "out", -1);
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

}
void
Socket::ProcessData(Packet* pk)
{

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
