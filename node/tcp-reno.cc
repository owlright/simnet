#include "tcp-reno.h"
void
TcpReno::IncreaseWindow(TcpSocketState* tcb, uint32_t segmentsAcked)
{
    // Linux tcp_in_slow_start() condition
    if (tcb->m_cWnd < tcb->m_ssThresh)
    {
        EV_DEBUG << "In slow start, m_cWnd " << tcb->m_cWnd << " m_ssThresh " << tcb->m_ssThresh << endl;
        segmentsAcked = SlowStart(tcb, segmentsAcked);
    }
    else
    {
        EV_DEBUG  << "In cong. avoidance, m_cWnd " << tcb->m_cWnd << " m_ssThresh "
                                                   << tcb->m_ssThresh;
        CongestionAvoidance(tcb, segmentsAcked);
    }
}

uint32_t
TcpReno::SlowStart(TcpSocketState* tcb, uint32_t segmentsAcked)
{
    if (segmentsAcked >= 1)
    {
        uint32_t sndCwnd = tcb->m_cWnd;
        tcb->m_cWnd =
            std::min((sndCwnd + (segmentsAcked * tcb->m_segmentSize)), (uint32_t)tcb->m_ssThresh);
        EV << "In SlowStart, updated to cwnd " << tcb->m_cWnd << " ssthresh "
                                                     << tcb->m_ssThresh << endl;
        return segmentsAcked - ((tcb->m_cWnd - sndCwnd) / tcb->m_segmentSize);
    }

    return 0;
}

void
TcpReno::CongestionAvoidance(TcpSocketState* tcb, uint32_t segmentsAcked)
{

    uint32_t w = tcb->m_cWnd / tcb->m_segmentSize;

    // Floor w to 1 if w == 0
    if (w == 0)
    {
        w = 1;
    }

    EV_DEBUG << "w in segments " << w << " m_cWndCnt " << m_cWndCnt << " segments acked "
                                  << segmentsAcked;
    if (m_cWndCnt >= w)
    {
        m_cWndCnt = 0;
        tcb->m_cWnd += tcb->m_segmentSize;
        EV_DEBUG << "Adding 1 segment to m_cWnd" << endl;
    }

    m_cWndCnt += segmentsAcked;
    EV_DEBUG << "Adding 1 segment to m_cWndCnt";
    if (m_cWndCnt >= w)
    {
        uint32_t delta = m_cWndCnt / w;

        m_cWndCnt -= delta * w;
        tcb->m_cWnd += delta * tcb->m_segmentSize;
        EV_DEBUG << "Subtracting delta * w from m_cWndCnt " << delta * w << endl;
    }
    EV_DEBUG << "At end of CongestionAvoidance(), m_cWnd: " << tcb->m_cWnd
                                                             << " m_cWndCnt: " << m_cWndCnt << endl;
}

uint32_t
TcpReno::GetSsThresh(const TcpSocketState* state, uint32_t bytesInFlight)
{
    // NS_LOG_FUNCTION(this << state << bytesInFlight);

    // In Linux, it is written as:  return max(tp->snd_cwnd >> 1U, 2U);
    return std::max<uint32_t>(2 * state->m_segmentSize, state->m_cWnd / 2);
}

void
TcpReno::CwndEvent(TcpSocketState* tcb, const TcpSocketState::TcpCAEvent_t newState)
{

}

void
TcpReno::PktsAcked(TcpSocketState *tcb)
{
    
}
