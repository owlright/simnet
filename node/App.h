#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#pragma once

#include <vector>
#include <omnetpp.h>
#include "Packet_m.h"
#include "socket.h"
// #include <functional>
using namespace omnetpp;
/**
 * Generates traffic for the network.
 */
class App : public cSimpleModule
{
public:
    int GetMyAddr() const;
  private:
    // configuration
    int myAddress;
    int groupAddress;
    int packetTotalCount;
    int packetLossCounter;
    bool disableSending;
    std::vector<int> destAddresses;
    int destAddress;
    cPar *sendIATime;
    cPar *packetLengthBytes;
    // Socket* m_socket;
    //packet type
    // int ACK = 0;
    // int DATA = 1;
private:
    std::map<int, Socket*> socketsTable;
    // state
    cMessage *generatePacket = nullptr;
    int pkCounter;
    long ack;
    long seq;
    // signals
    simsignal_t endToEndDelaySignal;
    simsignal_t hopCountSignal;
    simsignal_t sourceAddressSignal;

  public:
    virtual ~App();

private:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};
