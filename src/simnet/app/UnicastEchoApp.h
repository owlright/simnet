#pragma once
#include <unordered_map>
#include "UnicastApp.h"
using namespace omnetpp;

class UnicastEchoApp : public UnicastApp
{
private:
    // configuration
    std::unordered_map<IdNumber, Connection*> connections;

    B receivedBytes;

    // signals
    simsignal_t cwndSignal;
    simsignal_t rttSignal;

public:
    ~UnicastEchoApp() {};

protected:
    // inherited functions
    void initialize(int stage) override;
    // void handleMessage(cMessage *msg) override;
    int numInitStages() const override { return Stage::NUM_INIT_STAGES; }
    // helper functions
    void onNewConnectionArrived(Packet *packet);
    virtual void dealWithDataPacket(Connection *connection, Packet *packet);
    // for callback function use
    void connectionDataArrived(Connection *connection, cMessage *msg) override;
    cMessage* makeAckPacket(Connection *connection, Packet* pk);
};
