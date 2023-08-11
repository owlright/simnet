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
    virtual void onSendData(SeqNumber seq, B segmentSize) override;

protected:
    SeqNumber recover; // as a rtt timer triggered when congestion happen

    B cWnd{INT64_MAX};
    SeqNumber ssThresh{INT64_MAX};
    // int maxDisorderNumber;

    congStateType congState{OPEN};
    SeqNumber cWndCnt{0}; // ! Linear increase counter

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

