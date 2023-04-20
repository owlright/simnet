#pragma once
#include "CongAlgo.h"
using namespace omnetpp;

class Reno : public CongAlgo {
public:
    virtual B getSndWin() override {return std::max<B>(cWnd - (cWndRight - cWndLeft), 0);};
    virtual void onRecvAck(SeqNumber seq, bool congestion) override;
    // virtual void onRecvData(SeqNumber seq, B pkSize) override {};
    virtual void onSendData(SeqNumber seq) override;

protected:
    // SeqNumber lastTxSeq{0};
    SeqNumber markSeq{0};
    SeqNumber cWndLeft{0};
    SeqNumber cWndRight{0};
    B cWnd{INT64_MAX};
    SeqNumber ssThresh{INT64_MAX};

    congStateType congState{OPEN};
    SeqNumber cWndCnt{0}; // ! Linear increase counter

private:
    // signals
    static simsignal_t cwndSignal;
    static simsignal_t rttSignal;

private:
    void increaseWindow();
    void slowStart();
    void congestionAvoidance();
    SeqNumber getSsThresh();

    virtual void initialize(int stage) override;
};

