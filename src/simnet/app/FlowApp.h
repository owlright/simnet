#pragma once
#include "CongApp.h"

enum AppState_t
{
    Idle,
    Scheduled,
    Sending,
    AllDataSended,
    Finished
};

class FlowApp : public CongApp
{
public:
    const AppState_t getAppState() const {return appState;};
    void scheduleNextFlowAt(simtime_t_cref time);
    ~FlowApp();

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;

protected:
    virtual void onReceivedAck(const Packet* pk) override;

protected:
    virtual void onFlowStart();
    virtual void onFlowStop();
    virtual void prepareTxBuffer();
    virtual Packet* createDataPacket(B packetBytes);


protected:
    // ! ned parameters
    AppState_t appState{Idle};
    B flowSize{0};
    double flowStartTime;
    bool useJitter{false};
    cPar* jitterBeforeSending = nullptr;
    cMessage *flowStartTimer = nullptr;
    cMessage *jitterTimeout = nullptr;

};