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
    B inflightBytes() {return sentBytes + retransmitBytes - confirmedBytes - confirmedRetransBytes;};
    // inherited functions
    virtual Packet* createDataPacket(SeqNumber seq, B packetBytes);
    virtual void onFlowStart();
    virtual void onFlowStop();
    virtual void connectionDataArrived(Connection *connection, cMessage *msg) override;
    virtual void handleParameterChange(const char *parameterName) override;

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;

protected:
    // ! parameters
    std::vector<IntAddress> destAddresses;
    IntAddress destAddr{INVALID_ADDRESS};
    PortNumber destPort{INVALID_PORT};
    // int numRounds{0};

    // ! ned parameters
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

    // ! state
    B sentBytes{0}; // TODO: maybe rename to maxSentSeq is better
    B confirmedBytes{0};
    AppState_t appState{Idle};
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
    // simtime_t flowStartTime; // to calc the fct

    opp_component_ptr<CongAlgo> cong;
    double bandwidth;

public:
    virtual ~UnicastSenderApp();

};
