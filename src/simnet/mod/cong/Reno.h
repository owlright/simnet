#pragma once
#include "CongAlgo.h"
using namespace omnetpp;
/**
 * @brief A class for implementation of tcp reno algorithm
 *
 * Note that this class use ECE label to trigger its half-window beaviour, which
 * is not the standard behaviour in real network.
 *
 */
class Reno : public CongAlgo {
public:
    virtual B getcWnd() override {return cWnd;};
    virtual void onRecvAck(SeqNumber seq, B segmentSize, bool congestion) override;
    // virtual void onRecvData(SeqNumber seq, B pkSize) override {};
    virtual void onSendData(SeqNumber seq) override;

protected:
    SeqNumber recover; // the same use as rightEdge, but it's only triggered when seeing congestion
    SeqNumber rightEdge{0}; // when you wanna update something every rtt interval
    SeqNumber maxAckedSeqNumber{0}; // max seq acked by now
    B confirmedBytes{0};
    B sentBytes{0}; // max seq sent by now
    B cWnd{INT64_MAX};
    SeqNumber ssThresh{INT64_MAX};
    int maxDisorderNumber;

    congStateType congState{OPEN};
    SeqNumber cWndCnt{0}; // ! Linear increase counter

private:
    // signals
    static simsignal_t cwndSignal;
    // static simsignal_t rttSignal;

private:
    void increaseWindow();
    void slowStart();
    void congestionAvoidance();
    virtual B getSsThresh();

protected:
    virtual void initialize(int stage) override;

public:
    virtual void reset() override;
};

