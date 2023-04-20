#pragma once
#include "CongAlgo.h"
using namespace omnetpp;

class Reno : public CongAlgo {
public:
    virtual B getSndWin() override {return cWnd;};
    virtual void onRecvAck(SeqNumber seq, bool congestion) override;
    virtual void onRecvData(SeqNumber seq, B pkSize) override {};

protected:
    SeqNumber cWndLeft{0};
    SeqNumber cWndRight{0};
    B cWnd{INT64_MAX};
    SeqNumber ssThresh{INT64_MAX};

    congStateType congState{OPEN};
    SeqNumber cWndCnt{0}; // ! Linear increase counter

private:
    void increaseWindow();
    void slowStart();
    void congestionAvoidance();
    SeqNumber getSsThresh();

    virtual void initialize(int stage) override;
};

