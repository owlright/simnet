#include "tcp-dctcp.h"
Define_Module(TcpDctcp);
void TcpDctcp::initialize(int stage)
{
    if (stage==INITSTAGE_LOCAL) {
        m_alpha = par("alpha");
        m_g = par("g");
        alphaSignal = registerSignal("alpha");
        packetsECNSignal = registerSignal("packetsECN");
    }
}

// TcpDctcp::TcpDctcp()
//     : TcpReno(),
//     m_alpha(1.0),
//     m_g(0.0625),
//     m_ackedPacketsEcn(0),
//     m_ackedPacketsTotal(0),
//     m_nextSeq(0),
//     m_nextSeqFlag(false)
// {

// }

void
TcpDctcp::Init(TcpSocketState* tcb)
{
//    obcWnd.setName("observe window");
}

void
TcpDctcp::Reset(TcpSocketState* tcb)
{
    m_nextSeq = tcb->m_nextTxSequence;
    m_ackedPacketsEcn = 0;
    m_ackedPacketsTotal = 0;
}

uint32_t
TcpDctcp::GetSsThresh(const TcpSocketState* tcb, uint32_t bytesInFlight)
{
    return static_cast<uint32_t>((1 - m_alpha / 2.0) * tcb->m_cWnd);
}

void
TcpDctcp::PktsAcked(TcpSocketState* tcb)
{
    EV << "======" << __FUNCTION__ << "======" << endl;
    if (tcb->m_ecnState == TcpSocketState::ECN_ECE_RCVD) {
        m_ackedPacketsEcn += 1;
    }

    if (m_nextSeqFlag == false)
    {
        // EV << "Set the first time m_nextSeq=" << tcb->m_nextTxSequence << endl;
        m_nextSeq = tcb->m_nextTxSequence;
        m_nextSeqFlag = true;
    }
    if (tcb->m_lastAckedSeq >= m_nextSeq) // update every window
    {
        EV << " Last watch window size: " << tcb->m_nextTxSequence - m_nextSeq
            << " with " << m_ackedPacketsEcn << "packets labeled" <<endl;
//        obcWnd.record(tcb->m_nextTxSequence - m_nextSeq);
        double packetsECN = 0.0;
        if (m_ackedPacketsTotal > 0)
        {
            packetsECN = static_cast<double>(m_ackedPacketsEcn * 1.0 / m_ackedPacketsTotal);
        }
        m_alpha = (1.0 - m_g) * m_alpha + m_g * packetsECN;
        emit(alphaSignal, m_alpha);
        emit(packetsECNSignal, packetsECN);
        Reset(tcb);
    }
}

void TcpDctcp::CwndEvent(TcpSocketState* tcb, const TcpSocketState::TcpCAEvent_t newState)
{
    //todo what?
}
