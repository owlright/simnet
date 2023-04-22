#include "Reno.h"

Define_Module(Reno);

simsignal_t Reno::cwndSignal = registerSignal("cwnd");
simsignal_t Reno::rttSignal = registerSignal("rtt");

void Reno::initialize(int stage)
{
    if (stage==INITSTAGE_LOCAL) {
        cWnd = par("initWinSize");
    }
}
void Reno::onRecvAck(SeqNumber seq, bool congestion)
{
    if (ackedBytes + segmentSize < seq) { // ! avoid the last packet is smaller than a segmentSize, so >= is possible
        // ! there is no dealing with seq between cwndleft and cwndright other than next seq
        throw cRuntimeError("Can't deal with disordering packets.");
    }
    ackedBytes = seq;
    // * do calculations after each RTT finished
    if (ackedBytes == firstWndSeq) {
        emit(cwndSignal, cWnd);
    }

    if (!congestion) {
        increaseWindow(); // no congestion happened, we can increase anyway
        switch (congState) {
            case OPEN: // keep this state to OPEN
                break;
            case CWR: // congestion happened last time
                congState = OPEN; // change to OPEN
                break;
            default:
                throw cRuntimeError("Unknown congetsion state type");
        }
    } else {
        EV_DEBUG << "Received congestion signal on packet " << seq << endl;
        switch (congState) {
            case OPEN:
                ssThresh = getSsThresh();
                cWnd = ssThresh; // half the window(most of the time)
                EV_DEBUG << "Reduce ssThresh and cWnd to " << cWnd << endl;
                recover = sentBytes; // * assume sentBytes is about a RTT away
                EV << "ssThresh will not be updated until packet " << recover << " is received" << endl;
                congState = CWR;
                break;
            case CWR: // * continus congestion events
                if (ackedBytes >= recover) { // only half once a RTT/window
                    cWnd = getSsThresh(); // half the window(most of the time)
                    congState = OPEN; // reset the state after a RTT
                }
                break;
            default:
                throw cRuntimeError("unknow congetsion state type");
        }
    }


}

void Reno::increaseWindow()
{

    if (cWnd < ssThresh)
    {
        EV_TRACE << "In slow start, cWnd " << cWnd << " ssThresh " << ssThresh << endl;
        slowStart();
    }
    else
    {
        EV_TRACE  << "In cong. avoidance, cWnd: " << cWnd << " ssThresh: "<< ssThresh << endl;
        congestionAvoidance();
    }

}

void Reno::slowStart()
{
    cWnd = std::min(cWnd + segmentSize, ssThresh);
    EV_TRACE << "After slow start, m_cWnd " << cWnd << " ssThresh " << ssThresh << endl;
}



void Reno::congestionAvoidance()
{

    auto w = cWnd;

    // Floor w to 1 if w == 0
    if (w == 0)
    {
        w = 1;
    }

    EV_TRACE << "w in segments " << w << " cWndCnt " << cWndCnt << endl;
    if (cWndCnt >= w) // * wait for a window data sended
    {
        cWndCnt = 0;
        cWnd += 1;
        EV_TRACE << "Adding 1 segment to cWnd" << endl;
    } // ! after this, cWndCnt must < w or = 0, max(w-1, 0)

    cWndCnt += 1;
    EV_DEBUG << "Adding 1 segment to cWndCnt";
    // ! I don't know when will trigger the code below
    // if (cWndCnt >= w)
    // {
    //     SeqNumber delta = cWndCnt / w;
    //     cWndCnt -= static_cast<SeqNumber>(delta * w);
    //     cWnd += delta * 1;
    //     EV_DEBUG << "Subtracting delta * w from m_cWndCnt " << delta * w << endl;
    // }
    EV_TRACE<< "At end of CongestionAvoidance(), cWnd: " << cWnd
            << " m_cWndCnt: " << cWndCnt << endl;
}

SeqNumber Reno::getSsThresh()
{
    return std::max<SeqNumber>(cWnd >> 1, 2);
    // In Linux, it is written as:  return max(tp->snd_cwnd >> 1U, 2U);
    // if ((tcb->m_cWnd)>>1 == 0 ) {
    //     EV_WARN << "window too small" << endl;
    //     return 1;
    // } else {
    //     return tcb->m_cWnd >> 1;
    // }
    // return std::max<uint32_t>(2 * state->m_segmentSize, state->m_cWnd / 2);
}

void Reno::onSendData(B numBytes) {
    sentBytes += numBytes;
    if (ackedBytes >= firstWndSeq) {
        firstWndSeq = sentBytes;
    }
};
