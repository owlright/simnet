#include "MessageDispatcher.h"

Define_Module(PortDispatcher);

void PortDispatcher::initialize(int stage)
{

}

void PortDispatcher::registerPort(PortNumber port, int gateIndex)
{
    Enter_Method("PortDispatcher::registerPort");
    if (portToGateIndex.find(port) != portToGateIndex.end()) {
        throw cRuntimeError("port %" PRIu16 " is already registered.", port);
    }
    else {
        EV_TRACE << "localIn " << gateIndex << " belongs to localPort "<< port <<endl;
        portToGateIndex[port] = gateIndex;
    }
}

int PortDispatcher::findGateIndexByPort(PortNumber port)
{
    auto it = portToGateIndex.find(port);
    if (it != portToGateIndex.end()) {
        return it->second;
    } else {
        throw cRuntimeError("PortDispatcher::handleMessage: port %" PRIu16" is not registered.", port);
    }
}

void PortDispatcher::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("in")) {
        auto pk = check_and_cast<Packet *>(msg);
        auto port = pk->getDestPort();
        ASSERT(pk->getDestAddr() != INVALID_ADDRESS && port != INVALID_PORT);
        auto outGateIndex = findGateIndexByPort(port);
        send(msg, "localOut", outGateIndex);
        EV_TRACE << "Dispatching packet "<< msg->getName() << " to gate localOut " << outGateIndex  << endl;
    }
    else if (msg->arrivedOn("localIn")) {
        ASSERT(msg->isPacket());
        send(msg, "out");
        EV_TRACE << "Received packet "<< msg->getName() << " from app." << endl;
    }
    else {
        throw cRuntimeError("should not run here");
    }
}
