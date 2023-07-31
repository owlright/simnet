#pragma once
#include "UnicastApp.h"
#include "simnet/mod/cong/CongAlgo.h"


enum AppState_t
{
    Idle,
    Scheduled,
    Sending,
    AllDataSended,
    Finished
};


class UnicastSenderApp : public UnicastApp
{
public:
    const AppState_t getAppState() const {return appState;};
    void setDestAddr(IntAddress addr) {
        destAddr = addr;
        if (getEnvir()->isGUI()) // ! this is only good for debug in gui
            par("destAddress") = destAddr;
    };
    const IntAddress getDestAddr() const { return destAddr;};
    void scheduleNextFlowAt(simtime_t_cref time);

protected:
    // helper functions
    void sendPendingData();
    void retransmitLostPacket(SeqNumber seq, B packetBytes);
    B inflightBytes() {return sentBytes + retransmitBytes - confirmedNormalBytes - confirmedRetransBytes - confirmedRedundantBytes;};
    virtual Packet* createDataPacket(SeqNumber seq, B packetBytes);
    virtual void onFlowStart();
    virtual void onFlowStop();
    virtual void connectionDataArrived(Connection *connection, cMessage *msg) override;


protected:
    // inherited functions
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;
    virtual void handleParameterChange(const char *parameterName) override;

protected:
    // ! parameters
    std::vector<IntAddress> destAddresses;
    IntAddress destAddr{INVALID_ADDRESS};
    PortNumber destPort{INVALID_PORT};
    // int numRounds{0};

    // ! ned parameters
    int maxDisorderNumber{0};
    B messageLength{0};
    B flowSize{0};
    double flowStartTime;
    // cPar* flowInterval{nullptr};
    bool useJitter{false};
    cPar* jitterBeforeSending{nullptr};
    // double load;
    // bool loadMode{false};
    // state
    cMessage *flowStartTimer = nullptr;
    cMessage *jitterTimeout = nullptr;
    cMessage *RTOTimeout = nullptr;

    // ! state
    B sentBytes{0}; // TODO: maybe rename to maxSentSeq is better
    B confirmedNormalBytes{0}; // bytes sent and receive ack only once
    B confirmedRetransBytes{0}; // bytes that are acked by resending packets
    B confirmedRedundantBytes{0}; // bytes resend more than once

    AppState_t appState{Idle};
    // B currentFlowSize{0};

    std::unordered_map<SeqNumber, int> disorders;
    std::unordered_map<SeqNumber, int> retrans;
    SeqNumber leftEdge;
    // std::unordered_set<SeqNumber> confirmedDisorders;
    std::map<SeqNumber, B> sentButNotAcked;
    simtime_t currentBaseRTT{0};
    // simtime_t currentFlowInterval{0};
    // int currentRound{0};
    // statics
     B retransmitBytes{0};
    // ! signals
    static simsignal_t fctSignal;
    static simsignal_t idealFctSignal;
    static simsignal_t flowSizeSignal;
    static simsignal_t rttSignal;
    static simsignal_t inflightBytesSignal; // for debug
    // simtime_t flowStartTime; // to calc the fct

    opp_component_ptr<CongAlgo> cong;
    double bandwidth;

public:
    virtual ~UnicastSenderApp();

private:
    bool bindRemote();
};
