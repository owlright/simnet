#include "MessageDispatcher.h"

Define_Module(PortDispatcher);

void PortDispatcher::initialize(int stage)
{

}

int PortDispatcher::findGateIndexByPort(PortNumber port) {
    auto it = portToGateIndex.find(port);
    if (it != portToGateIndex.end()) {
        return it->second;
    } else {
        throw cRuntimeError("PortDispatcher::handleMessage: no app use port %u", port);
    }
}

void PortDispatcher::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("in")) {
        Packet *pk = check_and_cast<Packet *>(msg);
        auto port = pk->getDestPort();
        auto outGateIndex = findGateIndexByPort(port);
        send(msg, "localOut", outGateIndex);
        EV_INFO << "Dispatching packet "<< msg->getName() << " to gate localOut " << outGateIndex  << endl;
    }
    else if (msg->arrivedOn("localIn")) {
        if (msg->isPacket()) {
            send(msg, "out");
        }
        else { // * some configuration command from app
            int port = msg->par("appPort");
            portToGateIndex[port] = msg->getArrivalGate()->getIndex();
            delete msg;
        }
    }
    else {
        throw cRuntimeError("should not run here");
    }
}
