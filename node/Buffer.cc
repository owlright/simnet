#include "Buffer.h"

Define_Module(Buffer);

void Buffer::initialize()
{
    // TODO - Generated method body
}

void Buffer::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("in")) {
        send(msg, "out");
    }
    else {
        send(msg, "in");
    }
}
