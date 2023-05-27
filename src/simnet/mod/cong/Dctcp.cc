#include "Dctcp.h"

Define_Module(Dctcp);

simsignal_t Dctcp::alphaSignal = registerSignal("alpha");
simsignal_t Dctcp::bytesWithECERatio= registerSignal("eceRatio");

void Dctcp::initialize(int stage)
{
    Reno::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        alpha = par("alpha");
        g = par("g");
    }
}

void Dctcp::resetCounter()
{
    lastRTTAckedBytes = 0;
    ackedBytesWithECE = 0;
    nextSeq = sentBytes; // begin a new observe window
}

void Dctcp::onRecvAck(SeqNumber seq, B segmentSize, bool congestion)
{
    lastRTTAckedBytes += segmentSize;
    if (congestion) {
        ackedBytesWithECE += segmentSize;
    }
    if (nextSeqFlag == false) {
        nextSeq = sentBytes;
        nextSeqFlag = true;
        lastRTTAckedBytes = 0;
    }

    // update every window
    if (maxAckedSeqNumber > nextSeq) { // finish an observe window
        auto congestionRatio = 0.0;
        if (lastRTTAckedBytes > 0) {
            congestionRatio = static_cast<double>(ackedBytesWithECE * 1.0 / lastRTTAckedBytes);
        }
        emit(bytesWithECERatio, congestionRatio);
        alpha = (1.0 - g) * alpha + g * congestionRatio; // the window will be halved in getSsThresh() below
        emit(alphaSignal, alpha);
        resetCounter();
    }

    Reno::onRecvAck(seq, segmentSize, congestion);
}

B Dctcp::getSsThresh()
{
    auto win = std::max<B>(static_cast<B>((1.0 - alpha / 2.0) * cWnd), segmentSize);
    return win;
}
