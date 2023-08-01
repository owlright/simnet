#pragma once
#include "UnicastSenderApp.h"
#include "simnet/mod/manager/GlobalMetricCollector.h"
class WorkerApp : public UnicastSenderApp
{
protected:
    void initialize(int stage) override;
    virtual void onFlowStart() override;
    virtual void onFlowStop() override;
    virtual Packet* createDataPacket(SeqNumber seq, B packetBytes) override;
    virtual void finish() override;
    virtual void connectionDataArrived(Connection *connection, cMessage *msg) override;


protected:
    int jobId{-1};
    int workerId{-1};
    int numWorkers{-1};
    int numRounds;
    double roundInterval;

    opp_component_ptr<GlobalMetricCollector> jobMetricCollector;
};
