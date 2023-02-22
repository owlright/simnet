#include "tcp-dctcp.h"

void
TcpDctcp::Init(TcpSocketState* tcb)
{
    obcWnd.setName("observe window");
}

void
TcpDctcp::Reset(TcpSocketState* tcb)
{
    m_nextSeq = tcb->m_nextTxSequence;
    m_ackedPacketsEcn = 0;
    m_ackedPacketsTotal = 0;
}

void
TcpDctcp::PktsAcked(TcpSocketState* tcb)
{
    EV << "======" << __FUNCTION__ << "======" << endl;
    if (tcb->m_congState == TcpSocketState::CA_CWR) {
        m_ackedPacketsEcn += 1;
    }

    if (m_nextSeqFlag == false)
    {
        EV << "Set the first time m_nextSeq=" << tcb->m_nextTxSequence << endl;
        m_nextSeq = tcb->m_nextTxSequence;
        m_nextSeqFlag = true;
    }
    if (tcb->m_lastAckedSeq >= m_nextSeq)
    {
        EV << " Last window size: " << tcb->m_nextTxSequence - m_nextSeq << endl;
        obcWnd.record(tcb->m_nextTxSequence - m_nextSeq);
        double packetsECN = 0.0;
        if (m_ackedPacketsTotal > 0)
        {
            packetsECN = (m_ackedPacketsEcn * 1.0 / m_ackedPacketsTotal);
        }
        m_alpha = (1.0 - m_g) * m_alpha + m_g * packetsECN;
        m_nextSeq = tcb->m_nextTxSequence;
        m_ackedPacketsEcn = 0;
        Reset(tcb);
    }
}