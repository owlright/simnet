#include "GlobalMetricCollector.h"

Define_Module(GlobalMetricCollector);
simsignal_t GlobalMetricCollector::jobRCT = registerSignal("jobRCT");

inline void printNiceTime(time_t& now)
{
    std::tm* localTime = std::localtime(&now);
    int hours = localTime->tm_hour;
    int minutes = localTime->tm_min;
    int seconds = localTime->tm_sec;
    char buffer[10];
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "[%d:%d:%d]", hours, minutes, seconds);
    std::cout << GRAY << std::left << std::setw(11) << buffer << ESC;
}

void GlobalMetricCollector::reportFlowStart(int jobid, int numWorkers, int workerId, simtime_t roundStartTime)
{
    // * only create once for each job
    if (jobRoundMetric.find(jobid) == jobRoundMetric.end()) {
        jobRoundMetric[jobid] = new JobRoundMetric();
        jobRoundMetric[jobid]->numWorkers = numWorkers;
        jobRoundMetric[jobid]->roundFctSignal = createSignalForGroup(jobid);
    } else {
        ASSERT(jobRoundMetric[jobid]->numWorkers = numWorkers);
    }
    if (roundStartTime < jobRoundMetric[jobid]->startTime) {
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
    if (flowMetric.find(nodeId) == flowMetric.end()) {
        flowMetric[nodeId] = new FlowMetric();
        flowMetric[nodeId]->currentRound += 1;
        totalFlowsNumber += numFlows;
    } else {
        flowMetric[nodeId]->currentRound += 1;
    }
}

void GlobalMetricCollector::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        showProgressInfo = par("showProgressInfo").boolValue();
        progressInterval = par("progressInterval").doubleValue();
    } else if (stage == INITSTAGE_COLLECT) {
        globalView = findModuleFromTopLevel<GlobalView>("globalView", this);
        if (globalView != nullptr) {
            // ! HACK: get the flows number every host will send
            auto globalConfig = getEnvir()->getConfigEx();
            auto numFlows_cfg = globalConfig->getPerObjectConfigValue("**", "numFlows");
            numFlows = cStringTokenizer(numFlows_cfg).asIntVector().at(0);
            // ! prepare for progress display
            lastSimTime = simTime();
            lastRealTime = std::time(nullptr);
            stopWatch = new cMessage("stopWatch");
            scheduleAfter(0.001, stopWatch); // the first time is choosed at random
        } else
            throw cRuntimeError("Fail to get globalView");
    }
}

void GlobalMetricCollector::handleMessage(cMessage* msg)
{
    // * display progress every 10 seconds.
    ASSERT(msg == stopWatch);
    if (showProgressInfo) {
        auto now = std::time(nullptr);
        auto duration = now - lastRealTime > 1 ? now - lastRealTime : 1;
        durationInRealTime = .2 * duration + .8 * duration;
        simsecPerSecond = (simTime() - lastSimTime).dbl() / durationInRealTime;
        lastSimTime = simTime();
        lastRealTime = now;
        printNiceTime(now);
        char flow_info[50];
        memset(flow_info, 0, sizeof(flow_info));
        sprintf(flow_info, "flows: %ld/%ld", flow_counter, totalFlowsNumber);
        std::cout << CYAN << std::left << std::setw(20) << flow_info;
        auto group_num = jobRoundMetric.size() > 0 ? jobRoundMetric.size() : 1;
        char group_info[20 * group_num];
        memset(group_info, 0, sizeof(group_info));
        strcat(group_info, "groups: ");
        char _tmp[20];
        for (auto& [jobid, met] : jobRoundMetric) {
            sprintf(_tmp, "%d:%d, ", jobid, met->currentRound);
            strcat(group_info, _tmp);
        }
        std::cout << CYAN << std::left << std::setw(50) << group_info;
        auto leftFlows = totalFlowsNumber - flow_counter;
        auto esplasedFlows = flow_counter - last_flow_counter;
        double estimatedLeftTime = 0;
        if (leftFlows > 0 && esplasedFlows > 0)
            estimatedLeftTime = (leftFlows / esplasedFlows) * durationInRealTime;
        last_flow_counter = flow_counter;
        std::cout << "ETA(min): " << estimatedLeftTime / 60.0 << ENDC;
        scheduleAfter(progressInterval * simsecPerSecond, stopWatch);
    }
}

void GlobalMetricCollector::finish()
{
    // * help to set the sim-time-limit value so that progress percentile can be displayed correctly
    if (flow_counter != totalFlowsNumber) {
        std::cout << RED << flow_counter << "/" << totalFlowsNumber << " flows completed." << ENDC;
    } else {
        std::cout << GREEN << totalFlowsNumber << " flows stop at " << allFlowsFinishedTime << ENDC;
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
    cProperty* statisticTemplate = getProperties()->get("statisticTemplate", "jobRoundCompleteTime");
    getEnvir()->addResultRecorders(this, signal, statisticName, statisticTemplate);
    return signal;
}
