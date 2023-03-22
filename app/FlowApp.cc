#include "FlowApp.h"

Define_Module(FlowApp);
FlowApp::~FlowApp()
{
    cancelAndDelete(selfMsg);
}

void FlowApp::initialize(int stage)
{
    if (stage == Stage::INITSTAGE_LOCAL) {
        socket = (Socket*)(getSubmodule("socket"));
        selfMsg = new cMessage("SelfMsg-FlowApp");
        double load = par("load");
        if (load != 0) { // load mode
            assert(0 < load && load < 1);
            auto flowLengthMean = par("flowLengthMean").doubleValueInUnit("b");
            // !hacky
            auto bandwidth = check_and_cast<cDatarateChannel *>(getParentModule()->gateHalf("port", cGate::Type::OUTPUT, 0)->getChannel())->getDatarate();
            loadModeEnabled = true;
            interval = flowLengthMean / (bandwidth * load);
            EV << "load: " << load << " flowLengthMean: " << flowLengthMean <<" bandwidth: " << bandwidth << endl;
        }
        if (loadModeEnabled) {
            scheduleAfter(exponential(interval), selfMsg);
        } else {
            scheduleAfter(par("arrivalInterval"), selfMsg);
        }
    }
}

void FlowApp::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {

    }
}
