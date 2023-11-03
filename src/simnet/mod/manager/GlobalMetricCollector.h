#pragma once
#include <ctime>
#include <omnetpp.h>
#include <unordered_map>
#include "simnet/common/Defs.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalView.h"
using namespace omnetpp;

class GlobalMetricCollector final: public cSimpleModule
{
public:
    // for group coflows
    void reportFlowStart(int jobuid, int numWorkers, int workerId, simtime_t roundStartTime);
    void reportFlowStop(int jobuid, int numWorkers,  int workerId, simtime_t roundStopTime);
    // for unicast flows
    void reportFlowStop(int nodeId, simtime_t finishTime);
    ~GlobalMetricCollector() {
        cancelAndDelete(stopWatch);
    };
protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage* msg) override;
    void finish() override;

private:
    simsignal_t createSignalForGroup(int jobuid);
    static simsignal_t jobRCT;
    simtime_t allFlowsFinishedTime {0};
    simtime_t allJobsFinishedTime {0};

    opp_component_ptr<GlobalView> globalView;

    int numFlows{0};
    int64_t totalFlowsNumber{0};
    int64_t flow_counter{0};
    double timeRatio;
    simtime_t last_simtime{0};
    std::time_t last_time{0};

private:
    struct JobRoundMetric {
        int currentRound{0};
        int counter{0};
        int numWorkers;
        simtime_t startTime;
        simsignal_t roundFctSignal;
        void reset() {
            startTime = SimTime::getMaxTime();
            counter = 0;
        }
    };
    struct FlowMetric {
        int currentRound{0};
    };
    std::unordered_map<int, JobRoundMetric*> jobRoundMetric;
    std::unordered_map<int, FlowMetric*> flowMetric;
    cMessage* stopWatch{nullptr};
};