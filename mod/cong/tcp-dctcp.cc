#include "tcp-dctcp.h"
Define_Module(TcpDctcp);
void TcpDctcp::initialize(int stage)
{
    if (stage==INITSTAGE_LOCAL) {
        m_alpha = par("alpha");
        m_g = par("g");
        isAggressive = par("aggressive");
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
    auto newWin = static_cast<uint32_t>((1.0 - m_alpha / 2.0) * tcb->m_cWnd);
//    if (!isAggressive) {
//        if (newWin > tcb->m_aggWin) {
//            newWin = tcb->m_aggWin + (newWin - tcb->m_aggWin)/tcb->m_aggNum; // slow the speed to avoid
//        }
//    }
    return newWin;
}

/*uint32_t
TcpDctcp::SlowStart(TcpSocketState* tcb)
{
    if (isAggressive) {
        TcpReno::SlowStart(tcb); // default behaviour
        return 1;
    }

    if (tcb->m_cWnd >= tcb->m_aggWin) {
        if (m_aggWinCnt >= tcb->m_aggNum) {
            tcb->m_cWnd = std::min(tcb->m_cWnd + 1, tcb->m_ssThresh);
            m_aggWinCnt = 0;
        }
        m_aggWinCnt += 1;
    }
    else {
        TcpReno::SlowStart(tcb);
    }
    EV << "After slow start, m_cWnd " << tcb->m_cWnd << " m_ssThresh " << tcb->m_ssThresh << endl;
    return 1;
}*/

void
TcpDctcp::CongestionAvoidance(TcpSocketState* tcb) {
    if (isAggressive) {
        TcpReno::CongestionAvoidance(tcb); // use default
    }
    else { // not aggressive and cwnd bigger than aggWin, slow down
        uint32_t w;
        if (tcb->m_cWnd < tcb->m_aggWin)
            w = tcb->m_cWnd;
        else
            w = tcb->m_cWnd * tcb->m_aggNum;

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
}

void
TcpDctcp::PktsAcked(TcpSocketState* tcb)
{
    Enter_Method("dctcp");
    m_ackedPacketsTotal += 1;
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

// void TcpDctcp::CwndEvent(TcpSocketState* tcb, const TcpSocketState::TcpCAEvent_t newState)
// {
//     //todo what?
// }
