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

#include "App.h"
#include "../common/Defs.h"
#include "Controller.h"
#include "../common/ModuleAccess.h"

Define_Module(App);

App::~App()
{
    cancelAndDelete(generatePacket);
}

void App::initialize(int stage)
{
    if (stage == Stage::INITSTAGE_LOCAL) {
        myAddress = par("address");
        destAddress = par("destAddress");
        groupAddress = par("groupAddress");
        packetTotalCount = par("packetTotalCount");
        packetLengthBytes = par("packetLength");
        startTime = par("startTime");
        // flowInterval = &par("flowInterval"); // volatile parameter
        disableSending = (destAddress==-1);
        socket = (Socket*)(getSubmodule("socket"));

        WATCH(myAddress);
        WATCH(destAddress);

        endToEndDelaySignal = registerSignal("endToEndDelay");

        if (disableSending) {
            EV << myAddress << " don't send packets." << endl;
        }
        if (!disableSending) {
            generatePacket = new cMessage("nextPacket");
            scheduleAt(startTime, generatePacket);
        }
    }
    if (stage == Stage::INITSTAGE_REPORT) {
        if (!disableSending && groupAddress != -1) {
            auto controller = getModuleFromPar<Controller>(this->getParentModule()->par("globalController"), this->getParentModule());
            controller->updateAggrGroup(groupAddress, myAddress); // report this node is a sender
        }
    }
    if (stage == Stage::INITSTAGE_CONTROLL) {
        auto controller = getModuleFromPar<Controller>(this->getParentModule()->par("globalController"), this->getParentModule());
        if (controller->isGroupTarget(myAddress)) {
            groupSenders = controller->getAggrSendersNum(myAddress);
            groupAddress = myAddress;
            EV << "node "<< myAddress << " is group target and have " << groupSenders << " senders" << endl;
        };

        socket->Bind(myAddress, destAddress, groupAddress);
        socket->SetSendersNum(groupSenders);
    }

}

void App::handleMessage(cMessage *msg)
{
    if (msg == generatePacket) {
        // Sending packet
        ASSERT(socket!=nullptr);
        socket->Send(packetTotalCount, packetLengthBytes);
        return;
    }
    if (msg->arrivedOn("socketIn")) {
        // Handle incoming packet
        Packet *pk = check_and_cast<Packet *>(msg);

        // if (hasGUI())
        //     getParentModule()->bubble("Arrived!");

        delete pk; // do nothing
    }
}
