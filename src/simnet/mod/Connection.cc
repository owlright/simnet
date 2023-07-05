#include "Connection.h"
#include "simnet/mod/Packet_m.h"
void Connection::bind(IntAddress localAddr, PortNumber localPort)
{
    this->localAddr = localAddr;
    this->localPort = localPort;
}

void Connection::bindRemote(IntAddress destAddr, PortNumber destPort)
{
    ASSERT(destAddr != INVALID_ADDRESS && destPort != INVALID_PORT);
    this->destAddr = destAddr;
    this->destPort = destPort;
}

Connection::Connection(IdNumber connId)
{
    if (connectionId!=INVALID_ID) {
        throw cRuntimeError("connection id can only be set once!");
    }
    connectionId = connId;
}

void Connection::send(cMessage* msg)
{
    auto pk = check_and_cast<Packet *>(msg);
    pk->setSrcAddr(localAddr);
    pk->setLocalPort(localPort);
    if (destAddr != INVALID_ADDRESS || destPort != INVALID_PORT) {
        pk->setDestAddr(destAddr);
        pk->setDestPort(destPort);
    }

    pk->setConnectionId(getConnectionId());
    sendToUnicast(pk);
}

void Connection::sendToUnicast(cMessage *msg) {
    if (!gateToUnicast)
        throw("Connection: setOutputGate() must be invoked before connection can be used");
    check_and_cast<cSimpleModule *>(gateToUnicast->getOwnerModule())->send(msg, gateToUnicast);
}

void Connection::processMessage(cMessage* msg)
{
    if (cb)
        cb->connectionDataArrived(this, msg);
    else
        throw cRuntimeError("Connection::processMessage: must be binded before send.");
}

void Connection::setCallback(ICallback *callback)
{
    cb = callback;
}
