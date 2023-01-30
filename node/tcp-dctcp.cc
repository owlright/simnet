#include "tcp-dctcp.h"

void
TcpDctcp::Init(TcpSocketState* tcb)
{
    obcWnd.setName("observe window");
}

void
TcpDctcp::PktsAcked(TcpSocketState* tcb)
{
    EV << "======" << __FUNCTION__ << "======" << endl;
    if (tcb->m_congState == TcpSocketState::CA_CWR) {
        m_ackedBytesEcn += 1;
    }

    if (m_nextSeqFlag == false)
    {
        EV << "Set the first time m_nextSeq=" << tcb->m_nextTxSequence << endl;
        m_nextSeq = tcb->m_nextTxSequence;
        m_nextSeqFlag = true;
    }
    if (tcb->m_lastAckedSeq == m_nextSeq)
    {
        EV << " Last window size: " << tcb->m_nextTxSequence - m_nextSeq << endl;
        obcWnd.record(tcb->m_nextTxSequence - m_nextSeq);
        //todo calc the ecn ratio here
        //..
        m_nextSeq = tcb->m_nextTxSequence;
        m_ackedBytesEcn = 0;
    }
}