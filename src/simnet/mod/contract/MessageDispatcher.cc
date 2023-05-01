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
        EV_INFO << "Gate localIn " << gateIndex << " belongs to port "<< port <<endl;
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
        Packet *pk = check_and_cast<Packet *>(msg);
        auto port = pk->getDestPort();
        auto outGateIndex = findGateIndexByPort(port);
        send(msg, "localOut", outGateIndex);
        EV_DEBUG << "Dispatching packet "<< msg->getName() << " to gate localOut " << outGateIndex  << endl;
    }
    else if (msg->arrivedOn("localIn")) {
        ASSERT(msg->isPacket());
        send(msg, "out");
        EV_DEBUG << "Received packet "<< msg->getName() << " from app." << endl;
    }
    else {
        throw cRuntimeError("should not run here");
    }
}
