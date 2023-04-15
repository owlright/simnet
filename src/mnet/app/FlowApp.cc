#include "FlowApp.h"
#include "../common/ModuleAccess.h"
#include "../mod/Controller.h"
Define_Module(FlowApp);
FlowApp::~FlowApp()
{
    cancelAndDelete(startFlowTimer);
}

void FlowApp::initialize(int stage)
{
    if (stage == Stage::INITSTAGE_LOCAL) {
        myAddress = par("address");
        destAddress = par("destAddress");
        groupAddress = par("groupAddress");
        packetLengthBytes = par("packetLength");
        flowLength = &par("flowLength");
        socket = (Socket*)(getSubmodule("socket"));
        flowRealCompletionTimeSignal = registerSignal("flowRealCompletionTime");
        flowIdealCompletionTimeSignal = registerSignal("flowIdealCompletionTime");
        bandwidth = check_and_cast<cDatarateChannel *>(getParentModule()->gateHalf("port", cGate::Type::OUTPUT, 0)->getChannel())->getDatarate();
    }

    if (stage == Stage::INITSTAGE_CONTROLL) {
        double load = par("load");
        // HACK
        auto controller = getModuleFromPar<Controller>(this->getParentModule()->par("globalController"), this->getParentModule());
        if (destAddress == -2) { // means random pattern, ask for controller
            destAddress = controller->askForDest(myAddress);
            EV_DETAIL << "src:" << myAddress << " dest:" << destAddress << endl;
//            socket->Bind(myAddress, destAddress, groupAddress); // ! TODO the socket is unicast at first, it may change into a aggrsocket
        }
        if (groupAddress == -2) { // ask for controller to see if myself in a aggr group,-1 means not particapte in aggr group
            groupAddress = controller->askForGroup(myAddress);
            load = 0;
            if (groupAddress == myAddress) { // I'm the root node of the aggregation tree
                auto groupSenders = controller->getAggrSendersNum(myAddress);
                socket->SetSendersNum(groupSenders);
                EV << "node "<< myAddress << " is group target and has " << groupSenders << " senders" << endl;
            } else if (groupAddress > 0) { // I'm the sender
                destAddress = groupAddress;
                EV_DETAIL << "src:" << myAddress << " group:" << groupAddress << endl;
                flowLength = &par("aggFlowLength");
            }

        }
        socket->Bind(myAddress, destAddress, groupAddress);


        startFlowTimer = new cMessage("flowStart");

        if (load != 0) { // load mode
            assert(0 < load && load < 1);
            auto flowLengthMean = par("flowLengthMean").doubleValueInUnit("b");
            // !HACK

            loadModeEnabled = true;
            interval = flowLengthMean / (bandwidth * load);
            EV_DETAIL << "load: " << load << " flowLengthMean: " << flowLengthMean <<" bandwidth: " << bandwidth << endl;
        }
        if (myAddress != groupAddress) { // root node dont send packets
            if (loadModeEnabled) {
                scheduleAfter(exponential(interval), startFlowTimer);
            } else {
                scheduleAfter(par("arrivalInterval"), startFlowTimer);
            }
        }

    }
}

void FlowApp::handleMessage(cMessage *msg)
{
    if (msg == startFlowTimer) {
        if (destAddress != -1 || groupAddress != -1 ) {
            auto flowTotalBytes = flowLength->intValue();
            do {
                flowTotalBytes = flowLength->intValue();
            } while (flowTotalBytes <= 0);
            flowPackets = flowTotalBytes / packetLengthBytes;
            flowPackets += flowTotalBytes % packetLengthBytes > 0 ? 1 : 0;
            EV << "flow length in bytes: "<< flowTotalBytes << " packet size in bytes: "<< packetLengthBytes << endl;
            emit(flowIdealCompletionTimeSignal, (simtime_t)((flowTotalBytes*8)/bandwidth));
            flowStartTime = simTime();
            socket->Send(flowTotalBytes, packetLengthBytes);
        }
    }
    if (msg->arrivedOn("socketIn")) {
        // Handle incoming packet
        Packet *pk = check_and_cast<Packet *>(msg);
        if (pk->getKind() == PacketType::ACK) {
            flowPackets--;
            if (flowPackets == 0) { // flow is finished, start a new flow
                flowCounter++;
                emit(flowRealCompletionTimeSignal, simTime() - flowStartTime);
                if (loadModeEnabled) {
                    scheduleAfter(exponential(interval), startFlowTimer);
                } else {
                    scheduleAfter(par("arrivalInterval"), startFlowTimer);
                }
            }
        }
        delete pk;
    }
}
