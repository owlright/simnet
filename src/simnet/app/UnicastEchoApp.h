#pragma once
#include <unordered_map>
#include "SocketApp.h"
using namespace omnetpp;

class EchoApp : public UnicastApp
{
protected:
    // configuration
    std::unordered_map<IdNumber, Connection*> connections;
    std::unordered_map<IntAddress, SeqNumber> maxReceivedSeq;
    B receivedBytes;

public:
    virtual ~EchoApp();

protected:
    // inherited functions
    void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    // helper functions
    virtual void onNewConnectionArrived(IdNumber connId, const Packet* const packet);

    // for callback function use
    virtual void connectionDataArrived(Connection *connection, cMessage *msg) override;
    virtual Packet* createAckPacket(const Packet* const pk);
    virtual void refreshDisplay() const override;
};
