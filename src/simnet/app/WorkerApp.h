#pragma once
#include "FlowApp.h"
#include "simnet/mod/AggPacket_m.h"
#include "simnet/mod/manager/GlobalMetricCollector.h"
class WorkerApp : public CongApp
{
protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

protected:
    virtual void onReceivedAck(const Packet* pk) override;
    virtual void onReceivedData(const Packet* pk) override;
    virtual Packet* createDataPacket(B packetBytes) override; // ! ACK to PS's FIN will use this

protected:
    virtual void onRoundStart();
    virtual void onRoundStop();

protected:
    void setField(AggPacket* pk);
    void prepareTxBuffer();

protected:
    cMessage *roundStartTimer = nullptr;
    SeqNumber nextAggSeq{1};
    SeqNumber roundStartSeq{0};
    int jobId{-1};
    int workerId{-1};
    int numWorkers{-1};
    int numRounds;
    double roundInterval;
    B flowSize{0};

    opp_component_ptr<GlobalMetricCollector> jobMetricCollector;
};
