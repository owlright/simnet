//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "../common/Defs.h"
#include "../mod/Packet_m.h"
using namespace omnetpp;

/**
 * Point-to-point interface module. While one frame is transmitted,
 * additional frames get queued up; see NED file for more info.
 */
class L2Queue : public cSimpleModule
{
  private:
    B frameSize{0};
    B capacity{0};
    intval_t ecnThreshold{0};

    cQueue queue;
    B queueBytes{0};
    cMessage *endTransmissionEvent{nullptr};
    bool isBusy;

    static simsignal_t qlenSignal;
    // simsignal_t busySignal;
    // simsignal_t queueingTimeSignal;
    // simsignal_t dropSignal;
    // simsignal_t txBytesSignal;
    // simsignal_t rxBytesSignal;
    // simsignal_t congestionSignal;
    static simsignal_t outputPacketSignal;

  public:
    B getQueueBytes() const {return queueBytes;};
    void insertQueue(cMessage *);
    cMessage* popQueue();
    virtual ~L2Queue();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
    virtual void startTransmitting(cMessage *msg);
};

Define_Module(L2Queue);

simsignal_t L2Queue::outputPacketSignal = registerSignal("outputPacket");
simsignal_t L2Queue::qlenSignal = registerSignal("qlen");

void L2Queue::insertQueue(cMessage *msg) {
    queue.insert(msg);
    queueBytes += check_and_cast<cPacket *>(msg)->getByteLength();
}

cMessage* L2Queue::popQueue() {
    auto msg = (cMessage *)queue.pop();
    queueBytes -= check_and_cast<cPacket *>(msg)->getByteLength();
    return msg;
}

L2Queue::~L2Queue()
{
    cancelAndDelete(endTransmissionEvent);
}

void L2Queue::initialize()
{
    queue.setName("queue");
    endTransmissionEvent = new cMessage("endTxEvent");

    if (par("useCutThroughSwitching"))
        gate("line$i")->setDeliverImmediately(true);

    capacity = par("capacity");
    ecnThreshold = par("ecnThreshold");
    frameSize = par("frameSize");

    // busySignal = registerSignal("busy");
    // queueingTimeSignal = registerSignal("queueingTime");
    // dropSignal = registerSignal("drop");
    // txBytesSignal = registerSignal("txBytes");
    // rxBytesSignal = registerSignal("rxBytes");
    // congestionSignal = registerSignal("congestion");
//    emit(qlenSignal, getQueueBytes());
    // emit(busySignal, false);
    isBusy = false;
}

void L2Queue::startTransmitting(cMessage *msg)
{
    EV_TRACE << "Starting transmission of " << msg << endl;
    isBusy = true;
    int64_t numBytes = check_and_cast<cPacket *>(msg)->getByteLength();
    auto speed = check_and_cast<cDatarateChannel*> (gate("line$o")->getTransmissionChannel())->getDatarate();
    auto pk = check_and_cast<Packet*> (msg);
    pk->setTransmitTime(pk->getTransmitTime() + (numBytes*8)/speed);
    send(msg, "line$o");
    emit(outputPacketSignal, check_and_cast<Packet*>(msg));
    // emit(txBytesSignal, numBytes);

    // Schedule an event for the time when last bit will leave the gate.
    simtime_t endTransmission = gate("line$o")->getTransmissionChannel()->getTransmissionFinishTime();
    scheduleAt(endTransmission, endTransmissionEvent);
}

void L2Queue::handleMessage(cMessage *msg)
{
    if (msg == endTransmissionEvent) {
        // Transmission finished, we can start next one.
        EV_TRACE << "Transmission finished.\n";
        isBusy = false;
        if (queue.isEmpty()) {
            // emit(busySignal, false);
        }
        else {
            msg = popQueue();
            auto qTime = simTime() - msg->getTimestamp();
            auto pk = check_and_cast<Packet *> (msg);
            pk->setQueueTime(pk->getQueueTime() + qTime.dbl()); // accumulate queue time
            // emit(queueingTimeSignal, qTime);
            emit(qlenSignal, getQueueBytes());
            startTransmitting(msg);
        }
    }
    else if (msg->arrivedOn("line$i")) {
        // pass up
        // emit(rxBytesSignal, (intval_t)check_and_cast<cPacket *>(msg)->getByteLength());
        send(msg, "out");
    }
    else {  // arrived on gate "in"
        if (endTransmissionEvent->isScheduled()) {
            if (ecnThreshold > 0 && getQueueBytes() >= ecnThreshold * frameSize)
            {
                if (!getEnvir()->isExpressMode()) {
                    getParentModule()->bubble("congestion!");
                }
                // emit(congestionSignal, ecnThreshold);
                EV_TRACE << "Current queue length " << queue.getLength()
                    << " and ECN threshold is " << ecnThreshold <<". Mark ECN!\n";
                check_and_cast<Packet *>(msg)->setECN(true); // TODO how to avoid using Packet here?
            }
            else {
                // emit(congestionSignal, 0);
            }
            // We are currently busy, so just queue up the packet.
            if (capacity && getQueueBytes() >= capacity) {
                delete msg;
                throw cRuntimeError("Not ready for dealing with packet loss.");
                EV_TRACE << "Received " << msg << " but transmitter busy and queue full: discarding\n";
                // emit(dropSignal, (intval_t)check_and_cast<cPacket *>(msg)->getByteLength());
                delete msg;
            }
            else {
                EV_TRACE << "Received " << msg << " but transmitter busy: queueing up\n";
                msg->setTimestamp();
                // queue.insert(msg);
                insertQueue(msg);
//                emit(qlenSignal, queue.getLength());
            }

        }
        else {
            // We are idle, so we can start transmitting right away.
            EV_TRACE << "Received " << msg << endl;
            // emit(queueingTimeSignal, SIMTIME_ZERO); // TODO: what is the signal used for?
            startTransmitting(msg);
            // emit(busySignal, true);
        }
    }
}

void L2Queue::refreshDisplay() const
{
    if (!getEnvir()->isExpressMode()) {
        getDisplayString().setTagArg("t", 0, isBusy ? "transmitting" : "idle");
        getDisplayString().setTagArg("i", 1, isBusy ? (getQueueBytes() >= ecnThreshold * frameSize ? "red" : "yellow") : "");
    }
}

