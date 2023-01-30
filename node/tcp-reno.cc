#include "tcp-reno.h"

void
TcpReno::IncreaseWindow(TcpSocketState* tcb)
{
    EV << "======" << __FUNCTION__ << "======" << endl;
    // Linux tcp_in_slow_start() condition
    if (tcb->m_cWnd < tcb->m_ssThresh)
    {
        EV << "In slow start, m_cWnd " << tcb->m_cWnd << " m_ssThresh " << tcb->m_ssThresh << endl;
        SlowStart(tcb);
    }
    else
    {
        EV  << "In cong. avoidance, m_cWnd: " << tcb->m_cWnd << " m_ssThresh: "<< tcb->m_ssThresh << endl;
        CongestionAvoidance(tcb);
    }
}

uint32_t
TcpReno::SlowStart(TcpSocketState* tcb)
{
    tcb->m_cWnd = std::min(tcb->m_cWnd + 1, tcb->m_ssThresh);
    EV << "After slow start, m_cWnd " << tcb->m_cWnd << " m_ssThresh " << tcb->m_ssThresh << endl;
    return 1;
}

void
TcpReno::CongestionAvoidance(TcpSocketState* tcb)
{

    uint32_t w = tcb->m_cWnd;

    // Floor w to 1 if w == 0
    if (w == 0)
    {
        w = 1;
    }

    EV_DEBUG << "w in segments " << w << " m_cWndCnt " << m_cWndCnt << endl;
    if (m_cWndCnt >= w)
    {
        m_cWndCnt = 0;
        tcb->m_cWnd += 1;
        EV_DEBUG << "Adding 1 segment to m_cWnd" << endl;
    }

    m_cWndCnt += 1;
    EV_DEBUG << "Adding 1 segment to m_cWndCnt";
    if (m_cWndCnt >= w)
    {
        uint32_t delta = m_cWndCnt / w;

        m_cWndCnt -= delta * w;
        tcb->m_cWnd += delta * 1;
        EV_DEBUG << "Subtracting delta * w from m_cWndCnt " << delta * w << endl;
    }
    EV_DEBUG << "At end of CongestionAvoidance(), m_cWnd: " << tcb->m_cWnd
                                                             << " m_cWndCnt: " << m_cWndCnt << endl;
}

uint32_t
TcpReno::GetSsThresh(const TcpSocketState* tcb, uint32_t bytesInFlight)
{
    // NS_LOG_FUNCTION(this << state << bytesInFlight);

    // In Linux, it is written as:  return max(tp->snd_cwnd >> 1U, 2U);
    if ((tcb->m_cWnd)>>1 == 0 ) {
        EV_WARN << "window too small" << endl;
        return 1;
    } else {
        return tcb->m_cWnd >> 1;
    }
    // return std::max<uint32_t>(2 * state->m_segmentSize, state->m_cWnd / 2);
}

void
TcpReno::CwndEvent(TcpSocketState* tcb, const TcpSocketState::TcpCAEvent_t newState)
{

}
