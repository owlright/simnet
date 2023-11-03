#include "GlobalMetricCollector.h"

Define_Module(GlobalMetricCollector);
simsignal_t GlobalMetricCollector::jobRCT = registerSignal("jobRCT");

void GlobalMetricCollector::reportFlowStart(int jobid, int numWorkers, int workerId, simtime_t roundStartTime)
{
    // * only create once for each job
    if (jobRoundMetric.find(jobid) == jobRoundMetric.end()) {
        jobRoundMetric[jobid] = new JobRoundMetric();
        jobRoundMetric[jobid]->numWorkers = numWorkers;
        jobRoundMetric[jobid]->roundFctSignal = createSignalForGroup(jobid);
    }
    else {
        ASSERT(jobRoundMetric[jobid]->numWorkers = numWorkers);
    }
    if (roundStartTime  < jobRoundMetric[jobid]->startTime) {
        jobRoundMetric[jobid]->startTime = roundStartTime;
    }
}

void GlobalMetricCollector::reportFlowStop(int jobid, int numWorkers, int workerId, simtime_t roundStopTime)
{
    auto it = jobRoundMetric.find(jobid);
    if (it == jobRoundMetric.end()) {
        throw cRuntimeError("This group hasn't registered its signal");
    }
    if (roundStopTime > allJobsFinishedTime) {
        allJobsFinishedTime = roundStopTime;
    }
    auto roundMeter = it->second;
    roundMeter->counter++;

    // * wait for all workers to finish its round
    if (roundMeter->counter == jobRoundMetric[jobid]->numWorkers) {
        emit(roundMeter->roundFctSignal, simTime() - roundMeter->startTime);
        emit(jobRCT, simTime() - roundMeter->startTime);
        roundMeter->currentRound++;
        roundMeter->reset();
    }
}

void GlobalMetricCollector::reportFlowStop(int nodeId, simtime_t finishTime)
{
    flow_counter += 1;
    if (finishTime > allFlowsFinishedTime) {
        allFlowsFinishedTime = finishTime;
    }
    // report progress in every 10 seconds
    std::time_t now = std::time(nullptr);
    if (std::difftime(now, last_time) > 10.0) {
        scheduleAt(simTime(), stopWatch);
        last_time = now;
    }
}

void GlobalMetricCollector::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        globalView = findModuleFromTopLevel<GlobalView>("globalView", this);
        if (globalView != nullptr) {
            auto n = globalView->gethostIds().size();
            auto globalConfig = getEnvir()->getConfigEx();
            auto numFlows_cfg = globalConfig->getPerObjectConfigValue("**","numFlows");
            auto numFlows = cStringTokenizer(numFlows_cfg).asIntVector().at(0);
            totalFlowsNumber = n*numFlows;
            last_time = std::time(nullptr);
            stopWatch = new cMessage("stopWatch");
        }
        else
            throw cRuntimeError("Fail to get globalView");
    }
}

void GlobalMetricCollector::handleMessage(cMessage *msg)
{
    ASSERT(msg == stopWatch);
    std::cout << CYAN << flow_counter << "/" << totalFlowsNumber <<" flows completed." << ENDC;
    for (auto& [jobid, met]:jobRoundMetric) {
        std::cout << CYAN << jobid << ": " << met->currentRound << ", ";
    }
    std::cout << ENDC;
}

void GlobalMetricCollector::finish() {
    // * help to set the sim-time-limit value so that progress percentile can be displayed correctly
    if (flow_counter != totalFlowsNumber) {
        std::cout << RED << flow_counter << "/" << totalFlowsNumber <<" flows completed." << ENDC;
    } else {
        std::cout << GREEN <<  totalFlowsNumber << " flows stop at " << allFlowsFinishedTime << ENDC;
    }
    std::cout << GREEN << "All jobs  stop at " << allJobsFinishedTime << ENDC;
}

simsignal_t GlobalMetricCollector::createSignalForGroup(int jobid)
{
    char signalName[32];
    sprintf(signalName, "job-%d-RoundFinishTime", jobid);
    simsignal_t signal = registerSignal(signalName);

    char statisticName[32];
    sprintf(statisticName, "job-%d-RoundFinishTime", jobid);
    cProperty *statisticTemplate =
        getProperties()->get("statisticTemplate", "jobRoundCompleteTime");
    getEnvir()->addResultRecorders(this, signal, statisticName, statisticTemplate);
    return signal;
}
