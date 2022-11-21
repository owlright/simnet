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
struct flowItem {
    bool isDirectionIn; // in or out
    long seq; // the packet sequence has been confirmed by now
} ;
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
    typedef std::map<int, flowItem*> FlowTable;
    FlowTable ftable;
    // state
    cMessage *generatePacket = nullptr;
    long pkCounter;
    long ack;
    long seq;
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
    seq = 0;

    WATCH(pkCounter);
    WATCH(myAddress);
    WATCH_MAP(ftable);
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
        sprintf(pkname, "pk-%d-to-%d-#%ld", myAddress, destAddress, seq);
        EV << "generating packet " << pkname << endl;

        Packet *pk = new Packet(pkname);
        pk->setByteLength(packetLengthBytes->intValue());
        pk->setKind(DATA);
        pk->setSeq(pkCounter-1);
        pk->setSrcAddr(myAddress);
        pk->setDestAddr(destAddress);
        send(pk, "out");
        ftable[myAddress] = new flowItem();
        ftable[myAddress]->isDirectionIn = false;
        ftable[myAddress]->seq = 0;
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
        if (senderAddr==myAddress) { // the packet send from myself
            delete pk;
            return;
        }
        if (hasGUI())
            getParentModule()->bubble("Arrived!");

        char pkname[40];
        if (packetKind == DATA) {
            int outPortIndex = pk->par("outGateIndex");
            double rate = getParentModule()->gate("port$o", outPortIndex)->getChannel()->par("datarate");
            EV << pk->getName() <<"comes from port " << outPortIndex << " channelrate is " << rate <<endl;
            auto pkseq = pk->getSeq();
            auto it = ftable.find(senderAddr);
            if (it != ftable.end()) {
                if (it->second->seq <= pkseq)
                    it->second->seq = pkseq + 1; // ask for next packet
            }
            else {
                // deal with the first data packet
                ftable[senderAddr] = new flowItem();
                ftable[senderAddr]->isDirectionIn = true;
                ftable[senderAddr]->seq = pkseq + 1;
            }

            sprintf(pkname, "Ack-%d-to-%d-#%ld", myAddress, senderAddr, ftable[senderAddr]->seq);
            Packet *ackpk = new Packet(pkname);
            ackpk->setByteLength(packetLengthBytes->intValue());
            ackpk->setKind(ACK);
            ackpk->setAckSeq(ftable[senderAddr]->seq);
            ackpk->setSrcAddr(myAddress);
            ackpk->setDestAddr(senderAddr);

            if (hasGUI())
                getParentModule()->bubble("Generating ack packet...");
            send(ackpk, "out");
            delete pk;
       }
       else if (packetKind == ACK) {
            auto pkseq = pk->getSeq();
            auto it = ftable.find(myAddress);
            if (it->second->seq <= pkseq) {
                it->second->seq = pkseq + 1; // ask for next packet
            }
            else {

            }
       }
    }
}

