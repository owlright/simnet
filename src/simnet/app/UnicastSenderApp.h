#pragma once
#include "UnicastApp.h"

class UnicastSenderApp : public UnicastApp {
private:
    // configuration
    IntAddress destAddr;
    PortNumber destPort;
    double startTime;
    B messageLength;
    B flowSize;
    cPar *flowInterval;

    // state
    cMessage *flowStartTimer = nullptr;
    B sentBytes;

    // signals
    simsignal_t cwndSignal;
    simsignal_t rttSignal;

private:
    cMessage* makeDataPacket(Connection *connection, Packet* pk) override;
    cMessage* makeAckPacket(Connection *connection, Packet* pk) override {throw cRuntimeError("this is an sender app, do not respond.");};
    // helper functions
    void processSend();
public:
    ~UnicastSenderApp();
private:
    // inherited funcitons
    void initialize(int stage) override;
    void handleMessage(cMessage *msg) override;
    void connectionDataArrived(Connection *connection, cMessage *msg) override;
};
