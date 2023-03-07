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
#include "..\common\Defs.h"
#include "Controller.h"
#include "..\common\ModuleAccess.h"

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
        packetLengthBytes = &par("packetLength");
        sendIATime = &par("sendIaTime");  // volatile parameter

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
            scheduleAt(sendIATime->doubleValue(), generatePacket);
        }
    }
    if (stage == Stage::INITSTAGE_REPORT) {
        if (!disableSending && groupAddress != -1) {
            auto controller = getModuleFromPar<Controller>(this->getParentModule()->par("globalController"), this->getParentModule());
            controller->updateAggrGroup(groupAddress, myAddress);
        }
    }
    if (stage == Stage::INITSTAGE_CONTROLL) {
        auto controller = getModuleFromPar<Controller>(this->getParentModule()->par("globalController"), this->getParentModule());
        if (controller->isGroupTarget(myAddress)) {
            groupSenders = controller->getAggrSendersNum(myAddress);
            EV << "node "<< myAddress << " have " << groupSenders << " senders" << endl;
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
        socket->Send(packetTotalCount, packetLengthBytes->intValue());
        return;
    }
    if (msg->arrivedOn("socketIn")) {
        // Handle incoming packet
        Packet *pk = check_and_cast<Packet *>(msg);

        if (hasGUI())
            getParentModule()->bubble("Arrived!");

        delete pk; // do nothing
    }
}
