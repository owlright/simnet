#pragma once
#include "Reno.h"

class Dctcp: public Reno {
public:

protected:
    virtual void initialize(int stage) override;

private:
    double alpha;
    double g;

    SeqNumber nextSeq{0};
    bool nextSeqFlag{true};
    B ackedBytesWithECE{0};
    B lastAckedBytes{0};
    static simsignal_t alphaSignal;
    static simsignal_t bytesWithECERatio;

private:
    void resetCounter();
    void onRecvAck(SeqNumber seq, bool congestion) override;
    B getSsThresh() override;

};
