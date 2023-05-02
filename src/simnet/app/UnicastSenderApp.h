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
    double startTime;
    B messageLength{0};
    B flowSize{0};
    cPar *flowInterval{nullptr};
    // state
    cMessage *flowStartTimer = nullptr;
    B sentBytes{0};
    B confirmedBytes{0};

    // signals
    simsignal_t cwndSignal;
    simsignal_t rttSignal;

    CongAlgo* cong;

public:
    ~UnicastSenderApp();

};
