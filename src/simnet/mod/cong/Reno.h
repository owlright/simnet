#pragma once
#include "CongAlgo.h"
using namespace omnetpp;

class Reno : public CongAlgo {
public:
    virtual B getcWnd() override {return cWnd;};
    virtual void onRecvAck(SeqNumber seq, bool congestion) override;
    // virtual void onRecvData(SeqNumber seq, B pkSize) override {};
    virtual void onSendData(SeqNumber seq) override;

protected:
    SeqNumber recover;
    SeqNumber firstWndSeq{0};
    SeqNumber ackedBytes{0}; // sent but not acked
    SeqNumber sentBytes{0}; //max seq sent by now
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

