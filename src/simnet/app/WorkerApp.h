#pragma once
#include "FlowApp.h"
#include "simnet/mod/AggPacket_m.h"
#include "simnet/mod/manager/GlobalMetricCollector.h"
class WorkerApp : public FlowApp
{
protected:
    void setField(AggPacket* pk);

protected:
    virtual void onReceivedAck(const Packet* pk) override;

protected:
    virtual void onFlowStart() override;
    virtual void onFlowStop() override;
    virtual Packet* createDataPacket(B packetBytes) override;

protected:
    void initialize(int stage) override;
    virtual void finish() override;

protected:
    SeqNumber nextAggSeq{1};
    int jobId{-1};
    int workerId{-1};
    int numWorkers{-1};
    int numRounds;
    double roundInterval;

    opp_component_ptr<GlobalMetricCollector> jobMetricCollector;
};
