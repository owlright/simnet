#pragma once
#include <unordered_map>
#include "UnicastApp.h"
using namespace omnetpp;

class UnicastEchoApp : public UnicastApp
{
protected:
    // configuration
    std::unordered_map<IdNumber, Connection*> connections;
    B receivedBytes;

public:
    virtual ~UnicastEchoApp();

protected:
    // inherited functions
    void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    int numInitStages() const override { return Stage::NUM_INIT_STAGES; }
    // helper functions
    virtual void onNewConnectionArrived(IdNumber connId, const Packet* const packet);

    // for callback function use
    virtual void connectionDataArrived(Connection *connection, cMessage *msg) override;
    virtual Packet* createAckPacket(const Packet* const pk);
    virtual void refreshDisplay() const override;
};
