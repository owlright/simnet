#include "Reno.h"

Define_Module(Reno);

simsignal_t Reno::cwndSignal = registerSignal("cwnd");
// simsignal_t Reno::rttSignal = registerSignal("rtt");

void Reno::initialize(int stage) {
    if (stage==INITSTAGE_LOCAL) {
        cWnd = par("initWinSize");
        // rightEdge = cWnd;
        rttBytes = cWnd;
        // maxDisorderNumber = par("maxDisorderNumber");
    }
}

void Reno::reset()
{
    cWnd = par("initWinSize");
    // leftEdge = 0;
    rttBytes = cWnd;
    maxAckedSeqNumber = 0;
    // confirmedBytes = 0;
    // sentBytes = 0;
    ssThresh = INT64_MAX;
    congState = OPEN;
    cWndCnt = 0;
    // disorderSeqs.clear();
}

void Reno::onRecvAck(SeqNumber seq, B segmentSize, bool congestion) {
    Enter_Method("Reno::onRecvAck");
    EV_DEBUG << "recv " << seq << endl;
    rttBytes -= segmentSize;
    maxAckedSeqNumber = seq > maxAckedSeqNumber ? seq : maxSentSeqNumber;
    // else {
    //     if (seq > expectedSeq) { // ! always accept new packets
    //         confirmedBytes += segmentSize;
    //         maxAckedSeqNumber = seq;
    //         EV_WARN << "disordering(ahead) expect  " << expectedSeq
    //         << " but get " << seq << endl;
    //         ASSERT(disorderSeqs.find(seq) == disorderSeqs.end());
    //         disorderSeqs[expectedSeq] = maxDisorderNumber;
    //     } else {
    //         EV_WARN << "disordering(old) expect  " << expectedSeq
    //                      << " but get " << seq << endl;
    //     }
    // }
    // // ! check if former disordered packets are received
    // std::vector<SeqNumber> removed;
    // for (auto& it:disorderSeqs) {
    //    if (seq != it.first) {
    //         // ! this seq is retransmitted by app last turn
    //         // ! we need to wait a RTT time to see if retransmitted is successful
    //         if (it.second == 0) {
    //             auto wait_rtt = cWnd/segmentSize;
    //             it.second = wait_rtt > maxDisorderNumber ? wait_rtt : maxDisorderNumber;
    //         }
    //         it.second--;
    //    } else {
    //        // this retramsmitted seq is acked successfully, remove it
    //        removed.push_back(seq);
    //    }
    // }
    // for (auto& r:removed) {
    //    disorderSeqs.erase(r);
    // }

    // * do calculations after each RTT finished
    if (rttBytes == 0) {
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
                throw cRuntimeError("Unknown congestion state type");
        }
    } else {
        EV_DEBUG << "Received congestion signal on packet " << seq << endl;
        switch (congState) {
            case OPEN:
                ssThresh = getSsThresh();
                cWnd = ssThresh; // half the window(most of the time)
                EV_DEBUG << "Reduce ssThresh and cWnd to " << cWnd << endl;
                recover = seq + cWnd; // * wait for about a RTT
                EV << "ssThresh will not be updated until packet " << recover << " is received" << endl;
                congState = CWR;
                break;
            case CWR: // * continuous congestion events
                if (maxAckedSeqNumber >= recover) { // only half once a RTT/window
                    cWnd = getSsThresh(); // half the window(most of the time)
                    congState = OPEN; // reset the state after a RTT
                }
                break;
            default:
                throw cRuntimeError("unknow congestion state type");
        }
    }
}

void Reno::increaseWindow() {
    if (cWnd < ssThresh) {
        EV_DEBUG << "In slow start, cWnd " << cWnd << " ssThresh " << ssThresh << endl;
        slowStart();
    } else {
        EV_DEBUG  << "In cong. avoidance, cWnd: " << cWnd << " ssThresh: "<< ssThresh << endl;
        congestionAvoidance();
    }

}

void Reno::slowStart() {
    cWnd = std::min(cWnd + segmentSize, ssThresh);
    EV_TRACE << "After slow start, m_cWnd " << cWnd << " ssThresh " << ssThresh << endl;
}

void Reno::congestionAvoidance() {
    auto w = cWnd / segmentSize;
    // Floor w to 1 if w == 0
    if (w == 0) {
        w = 1;
    }

    EV_DEBUG << "w in segments " << w << " cWndCnt " << cWndCnt << endl;
    if (cWndCnt >= w) // * wait for a window data sended
    {
        cWndCnt = 0;
        cWnd += segmentSize;
        EV_DEBUG << "Adding 1 segment to cWnd" << endl;
    } // ! after this, cWndCnt must < w or = 0, max(w-1, 0)

    cWndCnt += 1;
}

B Reno::getSsThresh() {
    return std::max<SeqNumber>(cWnd >> 1, segmentSize);
}

void Reno::onSendData(SeqNumber seq, B segmentSize) {
    // sentBytes += segmentSize;
    if (seq > maxSentSeqNumber)
        maxSentSeqNumber = seq;
    if (rttBytes == 0)
        rttBytes = cWnd;
};
