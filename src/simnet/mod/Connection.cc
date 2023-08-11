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

void Connection::setOutputGate(cGate *const toUnicast)
{
    gateToUnicast = toUnicast;
    parentModule = check_and_cast<cSimpleModule*>(gateToUnicast->getOwnerModule());
}

void Connection::send(Packet* pk)
{
    pk->setSrcAddr(localAddr);
    pk->setLocalPort(localPort);
    if (destAddr != INVALID_ADDRESS || destPort != INVALID_PORT) {
        pk->setDestAddr(destAddr);
        pk->setDestPort(destPort);
    }
    pk->setConnectionId(getConnectionId());
    parentModule->send(pk, gateToUnicast);
}

void Connection::processPacket(Packet* pk)
{
    if (cb)
        cb->connectionDataArrived(this, pk);
    else
        throw cRuntimeError("Connection::processPacket: must be binded before send.");
}

void Connection::setCallback(ICallback *callback)
{
    cb = callback;
}
