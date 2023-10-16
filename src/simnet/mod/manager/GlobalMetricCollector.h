#pragma once
#include <omnetpp.h>
#include <unordered_map>
#include "simnet/common/Defs.h"
using namespace omnetpp;

class GlobalMetricCollector : public cModule
{
public:
    void reportFlowStart(int jobuid, int numWorkers, int workerId, simtime_t roundStartTime);
    void reportFlowStop(int jobuid, int numWorkers,  int workerId, simtime_t roundStopTime);
    void reportFlowStop(int nodeId, simtime_t finishTime);

protected:
    void finish() override;

private:
    simsignal_t createSignalForGroup(int jobuid);
    static simsignal_t jobRCT;
    simtime_t allFlowsFinishedTime {0};
    simtime_t allJobsFinishedTime {0};

private:
    struct JobRoundMetric {
        int counter{0};
        int numWorkers;
        simtime_t startTime;
        simsignal_t roundFctSignal;
        void reset() {
            startTime = SimTime::getMaxTime();
            counter = 0;
        }
    };
    std::unordered_map<int, JobRoundMetric*> jobRoundMetric;
};