#include "Connection.h"
#include "simnet/mod/Packet_m.h"
void Connection::bind(IntAddress localAddr, PortNumber localPort)
{
    this->localAddr = localAddr;
    this->localPort = localPort;
}

void Connection::bindRemote(IntAddress destAddr, PortNumber destPort)
{
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

void Connection::sendTo(cMessage* msg, IntAddress destAddr, PortNumber destPort)
{
    if (!cb)
        throw cRuntimeError("Connection::sendTo: must be binded before send.");
    if (destAddr == -1 || destPort == INVALID_PORT)
        throw cRuntimeError("Connection::sendTo: check the destAddr  %lld and destPort %hu", destAddr, destPort);
    this->destAddr = destAddr;
    this->destPort = destPort;
    auto pk = check_and_cast<Packet *>(msg);
    pk->setSrcAddr(localAddr);
    pk->setDestAddr(destAddr);
    pk->setLocalPort(localPort);
    pk->setDestPort(destPort);
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