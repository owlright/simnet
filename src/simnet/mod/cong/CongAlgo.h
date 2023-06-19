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
    virtual void onSendData(SeqNumber seq) {};
    virtual void onSendAck(SeqNumber seq) {};
    // virtual void onRecvAck(SeqNumber seq, bool congestion) = 0;
    virtual void onRecvAck(SeqNumber seq, B segmentSize, bool congestion) = 0;
    // virtual void onRecvData(SeqNumber seq, B pkSize) = 0;
    void setSegmentSize(B segmentSize) {this->segmentSize=segmentSize;};
    virtual void reset() = 0;
    const std::unordered_map<SeqNumber, int>& getDisorders() const {return disorderSeqs;}
    
protected:
    virtual void initialize(int stage) override{};
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override { throw cRuntimeError("this module doesn't handle messages, it runs only in initialize()"); }

protected:
    B segmentSize{1500};
    std::unordered_map<SeqNumber, int> disorderSeqs;
};
