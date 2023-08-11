#pragma once
#include <omnetpp.h>
#include "simnet/common/Defs.h"
#include <unordered_map>
using namespace omnetpp;


class CongAlgo : public cSimpleModule {
protected:
    enum congStateType {
        OPEN, //!< Normal state, no dubious events
        CWR, //!< cWnd was reduced due to some congestion notification event, such as ECN,
        RECOVERY,
        LAST_STATE  //!< Used only in debug messages
    };

public:
    virtual B getcWnd() {return INT64_MAX;};
    virtual void onSendData(SeqNumber seq, B segmentSize) {};
    virtual void onSendAck(SeqNumber seq) {};
    virtual void onRecvAck(SeqNumber seq, B segmentSize, bool congestion) = 0;

    void setSegmentSize(B segmentSize) {this->segmentSize=segmentSize;};
    virtual void reset() = 0;
    inline const SeqNumber& getMaxSentSeqNumber() {return maxSentSeqNumber;}
    inline const SeqNumber& getMaxAckedSeqNumber() {return maxAckedSeqNumber;};

protected:
    virtual void initialize(int stage) override{};
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override { throw cRuntimeError("this module doesn't handle messages, it runs only in initialize()"); }

protected:
    B segmentSize{1500};
    // SeqNumber rightEdge{0}; // when you wanna update something every rtt interval
    // SeqNumber leftEdge{0}; // the max seq received in order
    SeqNumber maxAckedSeqNumber{0}; // max seq acked by now
    SeqNumber maxSentSeqNumber{0};
    // std::unordered_map<SeqNumber, int> disorderSeqs;
};
