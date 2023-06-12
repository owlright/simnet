#pragma once
#include "UnicastApp.h"
#include "simnet/mod/cong/CongAlgo.h"
#include "simnet/mod/manager/TrafficPatternManager.h"
class UnicastSenderApp : public UnicastApp
{
protected:
    // helper functions
    void sendPendingData();
    B inflightBytes() {return sentBytes - confirmedBytes;};
    virtual Packet* createDataPacket(B packetBytes);
    virtual void onFlowStart();
    virtual void onFlowStop();

    // inherited functions
    void initialize(int stage) override;
    void handleMessage(cMessage *msg) override;
    void connectionDataArrived(Connection *connection, cMessage *msg) override;

protected:
    bool isUnicastSender{true};
    // configuration
    std::vector<IntAddress> destAddresses;
    IntAddress destAddr{INVALID_ADDRESS};
    PortNumber destPort{INVALID_PORT};
    int numRounds{0};
    TrafficPatternManager* tpManager;

    B messageLength{0};
    cPar* flowSize{nullptr};
    cPar* flowInterval{nullptr};
    cPar* jitterBeforeSending{nullptr};
    double load;
    bool loadMode{false};
    // state
    cMessage *flowStartTimer = nullptr;
    cMessage *jitterTimeout = nullptr;
    B sentBytes{0}; // TODO: maybe rename to maxSentSeq is better
    B confirmedBytes{0};
    B currentFlowSize{0};
    simtime_t currentBaseRTT{0};
    simtime_t currentFlowInterval{0};
    int currentRound{0};

    // signals
    static simsignal_t fctSignal;
    static simsignal_t idealFctSignal;
    static simsignal_t flowSizeSignal;
    static simsignal_t rttSignal;
    simtime_t flowStartTime;

    CongAlgo* cong;
    double bandwidth;

public:
    ~UnicastSenderApp();

};
