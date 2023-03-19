#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#pragma once

#include <vector>
#include <omnetpp.h>
#include "Packet_m.h"
#include "socket.h"
using namespace omnetpp;
/**
 * Generates traffic for the network.
 */
class App : public cSimpleModule
{
  private:
    // configuration
    int myAddress;
    int destAddress;
    int groupAddress;
    int groupSenders{1};
    int packetTotalCount;
    bool disableSending;
    int packetLengthBytes;
    double startTime;
    // cPar *flowInterval;


private:
//    std::map<int, Socket*> socketsTable;
    Socket* socket = nullptr;
    // state
    cMessage *generatePacket = nullptr;

    // signals
    simsignal_t endToEndDelaySignal;

  public:
    virtual ~App();

private:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
};
