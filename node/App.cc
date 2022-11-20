//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <vector>
#include <omnetpp.h>
#include "Packet_m.h"

using namespace omnetpp;

/**
 * Generates traffic for the network.
 */
class App : public cSimpleModule
{
  private:
    // configuration
    int myAddress;
    long packetCount;
    std::vector<int> destAddresses;
    cPar *sendIATime;
    cPar *packetLengthBytes;
    int ACK = 0;
    int DATA = 1;
private:
    typedef std::map<int, long> FlowSeqTable;
    FlowSeqTable fstable;
    // state
    cMessage *generatePacket = nullptr;
    long pkCounter;
    long ackCounter;
    // signals
    simsignal_t endToEndDelaySignal;
    simsignal_t hopCountSignal;
    simsignal_t sourceAddressSignal;

  public:
    virtual ~App();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(App);

App::~App()
{
    cancelAndDelete(generatePacket);
}

void App::initialize()
{
    myAddress = par("address");
    // packetCount = par("packetCount");
    packetLengthBytes = &par("packetLength");
    sendIATime = &par("sendIaTime");  // volatile parameter
    pkCounter = 0;
    ackCounter = 0;

    WATCH(pkCounter);
    WATCH(ackCounter);
    WATCH(myAddress);
    WATCH_MAP(fstable);
    const char *destAddressesPar = par("destAddresses");
    cStringTokenizer tokenizer(destAddressesPar);
    const char *token;
    while ((token = tokenizer.nextToken()) != nullptr)
        destAddresses.push_back(atoi(token));

    if (destAddresses.size() == 0)
        throw cRuntimeError("At least one address must be specified in the destAddresses parameter!");

    generatePacket = new cMessage("nextPacket");
    scheduleAt(sendIATime->doubleValue(), generatePacket);

    endToEndDelaySignal = registerSignal("endToEndDelay");
    hopCountSignal = registerSignal("hopCount");
    sourceAddressSignal = registerSignal("sourceAddress");
}

void App::handleMessage(cMessage *msg)
{
    if (msg == generatePacket) {
        // Sending packet
        int destAddress = destAddresses[intuniform(0, destAddresses.size()-1)];

        char pkname[40];
        sprintf(pkname, "pk-%d-to-%d-#%ld", myAddress, destAddress, pkCounter++);
        EV << "generating packet " << pkname << endl;

        Packet *pk = new Packet(pkname);
        pk->setByteLength(packetLengthBytes->intValue());
        pk->setKind(DATA);
        pk->setSeq(pkCounter-1);
        pk->setSrcAddr(myAddress);
        pk->setDestAddr(destAddress);
        send(pk, "out");

        // scheduleAt(simTime() + sendIATime->doubleValue(), generatePacket);
        // if (hasGUI())
        //     getParentModule()->bubble("Generating packet...");
    }
    else {
        // Handle incoming packet
        Packet *pk = check_and_cast<Packet *>(msg);

        EV << "received packet " << pk->getName() << " after " << pk->getHopCount() << "hops" << endl;
        emit(endToEndDelaySignal, simTime() - pk->getCreationTime());
        emit(hopCountSignal, pk->getHopCount());
        emit(sourceAddressSignal, pk->getSrcAddr());
        int senderAddr = pk->getSrcAddr();
        short packetKind = pk->getKind();

        if (hasGUI())
            getParentModule()->bubble("Arrived!");

        char pkname[40];
        if (packetKind == DATA & senderAddr != myAddress) {
            int outPortIndex = pk->par("outGateIndex");
            double rate = getParentModule()->gate("port$o", outPortIndex)->getChannel()->par("datarate");
            EV << pk->getName() <<"comes from port " <<outPortIndex << "channelrate is " << rate<<endl;
            sprintf(pkname, "ack-%d-to-%d-#%ld", myAddress, senderAddr, ackCounter++);
            Packet *ack = new Packet(pkname);
            ack->setByteLength(packetLengthBytes->intValue());
            ack->setKind(ACK);
            ack->setAckSeq(ackCounter-1);
            ack->setSrcAddr(myAddress);
            ack->setDestAddr(senderAddr);
            auto it = fstable.find(senderAddr);
            if (it != fstable.end()) {
                fstable[senderAddr] = it->second + 1;
            }
            else {
                fstable[senderAddr] = 1;
            }

            if (hasGUI())
                getParentModule()->bubble("Generating ack packet...");
            send(ack, "out");
            delete pk;
       }
    }
}

