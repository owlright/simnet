#pragma once
#include "simnet/common/Defs.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalView.h"
#include <ctime>
#include <omnetpp.h>
#include <unordered_map>
using namespace omnetpp;

class GlobalMetricCollector final : public cSimpleModule {
public:
    void registerGroupMetric(int jobuid, int numWorkers, int numRounds);
    void registerFlowMetric(int nodeId, int numFlows);
    // for group flows
    void reportFlowStart(int jobuid, int workerId, simtime_t roundStartTime);
    void reportFlowStop(int jobuid, int workerId, simtime_t roundStopTime);
    // for unicast flows
    void reportFlowStop(int nodeId, simtime_t finishTime);
    ~GlobalMetricCollector() { cancelAndDelete(stopWatch); };
    static std::ostream& getStream() { return stream; }

protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage* msg) override;
    void finish() override;

private:
    double progressInterval { 10 };
    bool showProgressInfo { false };
    static std::ostream stream; // this singleton is used to avoid allocating a new stream each time a log statement executes

private:
    simsignal_t createSignalForGroup(int jobuid);
    static simsignal_t jobRCT;
    simtime_t allFlowsFinishedTime { 0 };
    simtime_t allJobsFinishedTime { 0 };

    opp_component_ptr<GlobalView> globalView;

    int64_t totalFlowsNumber { 0 };
    int64_t flow_counter { 0 };
    int64_t last_flow_counter { 0 };
    int flow_nochange_counter { 0 };
    bool flow_is_over { false };

    int64_t totalRoundsNumber { 0 };
    int64_t last_round_counter { 0 };
    int64_t round_counter { 0 };
    int round_nochange_counter { 0 };
    bool job_is_over { false };

    double durationInRealTime;
    double simsecPerSecond; // ! 1 realworld second = # simsecs
    double estimatedLeftTime { 0 };
    simtime_t lastSimTime { 0 };
    std::time_t lastRealTime { 0 };

    std::time_t simStartRealTime { 0 };

private:
    struct JobRoundMetric {
        int currentRound { 0 };
        int counter { 0 };
        int numWorkers;
        int numRounds;
        simtime_t startTime;
        simsignal_t roundFctSignal;
        void reset()
        {
            startTime = SimTime::getMaxTime();
            counter = 0;
        }
    };

    std::map<int, JobRoundMetric*> jobRoundMetric;
    cMessage* stopWatch { nullptr };
};