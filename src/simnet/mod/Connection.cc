#include "Connection.h"

void Connection::bind(IntAddress localAddr, PortNumber localPort)
{
    this->localAddr = localAddr;
    this->localPort = localPort;
}

void Connection::setConnectionId(IdNumber id)
{
    if (connectionId!=INVALID_ID) {
        throw cRuntimeError("connection id can only be set once!");
    }
    connectionId = id;
}

void Connection::listenFrom(IntAddress destAddr, PortNumber destPort)
{
    this->destAddr = destAddr;
    this->destPort = destPort;
}

void Connection::sendTo(cMessage* msg, IntAddress destAddr, PortNumber destPort)
{
    if (!cb)
        throw cRuntimeError("Connection::sendTo: must be binded before send.");
    if (destAddr == -1 || destPort == INVALID_PORT)
        throw cRuntimeError("Connection::sendTo: check the destAddr  %lld and destPort %hu", destAddr, destPort);
    listenFrom(destAddr, destPort);

    auto pk = cb->makePacket(this, msg, destAddr, destPort);
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