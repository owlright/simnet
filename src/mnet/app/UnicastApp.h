
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif
#pragma once

#include <vector>
#include <omnetpp.h>
#include "../mod/Packet_m.h"
#include "../common/Defs.h"
using namespace omnetpp;

struct Connection
{
public:
    class ICallback {
      public:
        virtual ~ICallback() {}

        /**
         * Notifies about data arrival, packet ownership is transferred to the callee.
         */
        virtual void connectionDataArrived(Connection *connection, Packet *packet) = 0;

    };

private:
    const IdNumber connectionId = cSimulation::getActiveEnvir()->getUniqueNumber(); // TODO will this be inited correctly?
    IntAddress localAddr{-1};
    IntAddress destAddr{-1};
    PortNumber localPort{MAX_PORT_NUMBER};
    PortNumber destPort{MAX_PORT_NUMBER};
    std::string ccAlgorithmName;
    ICallback *cb = nullptr;
    cGate *gateToUnicast = nullptr;

public:
    void bind(IntAddress localAddr, PortNumber localPort);
    void setCallback(ICallback *callback);
    void setOutputGate(cGate *toUnicast) {gateToUnicast = toUnicast;};
    const IdNumber getConnectionId() const {return connectionId;};
    void sendTo(Packet* pk, IntAddress destAddr, PortNumber destPort);
    void sendToUnicast(cMessage* msg);
    void processMessage(cMessage* msg) {};
};

void Connection::bind(IntAddress localAddr, PortNumber localPort)
{
    this->localAddr = localAddr;
    this->localPort = localPort;
}

void Connection::sendTo(Packet* pk, IntAddress destAddr, PortNumber destPort)
{
    if (destAddr == -1 || destPort == MAX_PORT_NUMBER)
        throw cRuntimeError("Connection::sendTo: check the destAddr  %lld and destPort %hu", destAddr, destPort);
    pk->setSrcAddr(localAddr);
    pk->setLocalPort(localPort);
    pk->setDestAddr(destAddr);
    pk->setDestPort(destPort);
    sendToUnicast(pk);
}

void Connection::sendToUnicast(cMessage *msg) {
    if (!gateToUnicast)
        throw("Connection: setOutputGate() must be invoked before connection can be used");
    check_and_cast<cSimpleModule *>(gateToUnicast->getOwnerModule())->send(msg, gateToUnicast);
}

void Connection::setCallback(ICallback *callback)
{
    cb = callback;
}



class UnicastApp : public cSimpleModule, public Connection::ICallback
{
private:
    // configuration
    Connection connection;
    IntAddress myAddr;
    PortNumber localPort;
    IntAddress destAddr;
    PortNumber destPort;
    double startTime;
    B messageLength;
    B flowSize;
    cPar *flowInterval;

private:
    // state
    cMessage *flowStartTimer = nullptr;
    B sentBytes;

    // signals
    simsignal_t cwndSignal;
    simsignal_t rttSignal;

public:
    virtual ~UnicastApp();
    // for callback function use
    void connectionDataArrived(Connection *connection, Packet *packet) override;

private:
    // inherited funcitons
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return Stage::NUM_INIT_STAGES; }

    // helper functions
    void processSend();

};
