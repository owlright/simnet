#pragma once
#include "UnicastApp.h"
#include "simnet/mod/cong/CongAlgo.h"

class UnicastSenderApp : public UnicastApp
{
protected:
    // helper functions
    void sendPendingData();
    B inflightBytes() {return sentBytes + retransmitBytes - confirmedBytes - confirmedRetransBytes;};
    virtual Packet* createDataPacket(SeqNumber seq, B packetBytes);
    virtual void onFlowStart();
    virtual void onFlowStop();
    // inherited functions
    virtual void finish() override;
    void initialize(int stage) override;
    void handleMessage(cMessage *msg) override;
    void connectionDataArrived(Connection *connection, cMessage *msg) override;
    virtual void handleParameterChange(const char *parameterName) override;

protected:
    // ! parameters
    std::vector<IntAddress> destAddresses;
    IntAddress destAddr{INVALID_ADDRESS};
    PortNumber destPort{INVALID_PORT};
    // int numRounds{0};

    // ! ned parameters
    B messageLength{0};
    B flowSize{0};
    // cPar* flowInterval{nullptr};
    bool useJitter{false};
    cPar* jitterBeforeSending{nullptr};
    // double load;
    // bool loadMode{false};
    // state
    cMessage *flowStartTimer = nullptr;
    cMessage *jitterTimeout = nullptr;

    // ! state
    B sentBytes{0}; // TODO: maybe rename to maxSentSeq is better
    B confirmedBytes{0};

    // B currentFlowSize{0};

    B retransmitBytes{0};
    B confirmedRetransBytes{0};
    std::unordered_set<SeqNumber> disorders;
    std::unordered_set<SeqNumber> confirmedDisorders;
    simtime_t currentBaseRTT{0};
    // simtime_t currentFlowInterval{0};
    // int currentRound{0};

    // ! signals
    static simsignal_t fctSignal;
    static simsignal_t idealFctSignal;
    static simsignal_t flowSizeSignal;
    static simsignal_t rttSignal;
    static simsignal_t inflightBytesSignal; // for debug
    simtime_t flowStartTime; // to calc the fct

    opp_component_ptr<CongAlgo> cong;
    double bandwidth;

public:
    virtual ~UnicastSenderApp();

};
