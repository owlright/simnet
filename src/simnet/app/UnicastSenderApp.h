#pragma once
#include "UnicastApp.h"
#include "simnet/mod/cong/CongAlgo.h"
class UnicastSenderApp : public UnicastApp
{
protected:
    cMessage* makeDataPacket(Connection *connection, Packet* pk) override;
    cMessage* makeAckPacket(Connection *connection, Packet* pk) override {throw cRuntimeError("this is an sender app, do not respond.");};
    // helper functions
    void sendPendingData();
    B inflightBytes() {return sentBytes - confirmedBytes;};

    // inherited functions
    void initialize(int stage) override;
    void handleMessage(cMessage *msg) override;
    void connectionDataArrived(Connection *connection, cMessage *msg) override;

protected:
    // configuration
    IntAddress destAddr{INVALID_ADDRESS};
    PortNumber destPort{INVALID_PORT};

    B messageLength{0};
    cPar* flowSize{nullptr};
    cPar* flowInterval{nullptr};
    // state
    IdNumber flowId{-1};
    cMessage *flowStartTimer = nullptr;
    B sentBytes{0};
    B confirmedBytes{0};
    B currentFlowSize{0};

    // signals
    static simsignal_t cwndSignal;
    static simsignal_t rttSignal;
    static simsignal_t fctSignal;
    static simsignal_t idealFctSignal;
    simtime_t flowStartTime;

    CongAlgo* cong;
    double bandwidth;

public:
    ~UnicastSenderApp();

};
