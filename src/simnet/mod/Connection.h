#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif
#pragma once
#include <omnetpp.h>
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
        virtual void connectionDataArrived(Connection *connection, cMessage* msg) = 0;

    };

private:
    IdNumber connectionId{INVALID_ID};
    IntAddress localAddr{INVALID_ADDRESS};
    IntAddress destAddr{INVALID_ADDRESS};
    PortNumber localPort{INVALID_PORT};
    PortNumber destPort{INVALID_PORT};

    ICallback *cb = nullptr;
    cGate *gateToUnicast = nullptr;
public:
    const IntAddress getDestAddr() const {return destAddr;};
    const IntAddress getDestPort() const {return destPort;};
    explicit Connection(IdNumber connId);
    void setOutputGate(cGate* const toUnicast) {gateToUnicast = toUnicast;};

public:
    void bind(IntAddress localAddr, PortNumber localPort);
    void bindRemote(IntAddress destAddr, PortNumber destPort);
    void setCallback(ICallback *callback);

    const IdNumber getConnectionId() const {return connectionId;};
    void send(cMessage* msg);
    void processMessage(cMessage* msg);

private:
    void sendToUnicast(cMessage* msg);
};


