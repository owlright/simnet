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
// utility function for printing elapsed time
static char* timeToStr(double t, char* buf = nullptr)
{
    static char buf2[64];
    char* b = buf ? buf : buf2;

    int sec = (int)floor(t);
    if (t < 3600)
        sprintf(b, "%lgs (%dm %02ds)", t, int(sec / 60L), int(sec % 60L));
    else if (t < 86400)
        sprintf(b, "%lgs (%dh %02dm)", t, int(sec / 3600L), int((sec % 3600L) / 60L));
    else
        sprintf(b, "%lgs (%dd %02dh)", t, int(sec / 86400L), int((sec % 86400L) / 3600L));

    return b;
}

void GlobalMetricCollector::reportFlowStart(int jobid, int workerId, simtime_t roundStartTime)
{
    // * only create once for each job
    if (jobRoundMetric.find(jobid) == jobRoundMetric.end()) {
        throw cRuntimeError("job not register");
    }
    if (roundStartTime < jobRoundMetric[jobid]->startTime) {
        jobRoundMetric[jobid]->startTime = roundStartTime;
    }
}

void GlobalMetricCollector::reportFlowStop(int jobid, int workerId, simtime_t roundStopTime)
{
    auto it = jobRoundMetric.find(jobid);
    if (it == jobRoundMetric.end()) {
        throw cRuntimeError("This group hasn't registered its signal");
    }

    allJobsFinishedTime = roundStopTime;

    auto roundMeter = it->second;
    roundMeter->counter++;

    // * wait for all workers to finish its round
    if (roundMeter->counter == jobRoundMetric[jobid]->numWorkers) {
        emit(roundMeter->roundFctSignal, simTime() - roundMeter->startTime);
        emit(jobRCT, simTime() - roundMeter->startTime);
        roundMeter->currentRound++;
        roundMeter->reset();
        round_counter += 1;
        if (round_counter == totalRoundsNumber) {
            auto runId = getEnvir()->getConfigEx()->getActiveRunNumber();
            std::cout << GREEN << "runID " << runId << " job is over" << ENDC;
            job_is_over = true;
        }
    }
}

void GlobalMetricCollector::registerGroupMetric(int jobid, int numWorkers, int numRounds)
{
    if (jobRoundMetric.find(jobid) == jobRoundMetric.end()) {
        jobRoundMetric[jobid] = new JobRoundMetric();
        jobRoundMetric[jobid]->numWorkers = numWorkers;
        jobRoundMetric[jobid]->roundFctSignal = createSignalForGroup(jobid);
        jobRoundMetric[jobid]->numRounds = numRounds;
        totalRoundsNumber += numRounds;
    }
}

void GlobalMetricCollector::registerFlowMetric(int nodeId, int numFlows) { totalFlowsNumber += numFlows; }

void GlobalMetricCollector::reportFlowStop(int nodeId, simtime_t finishTime)
{
    flow_counter += 1;
    allFlowsFinishedTime = finishTime;
    if (totalFlowsNumber == flow_counter) {
        auto runId = getEnvir()->getConfigEx()->getActiveRunNumber();
        std::cout << GREEN << "runID " << runId << " flow is over" << ENDC;
        flow_is_over = true;
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
            // ! prepare for progress display
            lastSimTime = simTime();
            lastRealTime = std::time(nullptr);
            simStartRealTime = std::time(nullptr);
            stopWatch = new cMessage("stopWatch");
            scheduleAfter(0.001, stopWatch); // the first time is choosed at random
            std::cout << totalFlowsNumber << std::endl;
            if (totalFlowsNumber == 0)
                flow_is_over = true;
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
        // printNiceTime(now);
        auto runId = getEnvir()->getConfigEx()->getActiveRunNumber();
        char flow_info[50];
        memset(flow_info, 0, sizeof(flow_info));
        sprintf(flow_info, "runID %d flows: %ld/%ld", runId, flow_counter, totalFlowsNumber);
        std::cout << std::left << std::setw(30) << flow_info;
        auto group_num = jobRoundMetric.size() > 0 ? jobRoundMetric.size() : 1;
        char group_info[20 * group_num];
        memset(group_info, 0, sizeof(group_info));
        strcat(group_info, "groups: ");
        char _tmp[20];
        for (auto& [jobid, met] : jobRoundMetric) {
            sprintf(_tmp, "%d: %d/%d, ", jobid, met->currentRound, met->numRounds);
            strcat(group_info, _tmp);
        }
        std::cout << std::left << std::setw(10 * group_num) << group_info;

        auto leftFlows = totalFlowsNumber - flow_counter;
        auto esplasedFlows = flow_counter - last_flow_counter;
        auto leftRounds = totalRoundsNumber - round_counter;
        auto esplasedRounds = round_counter - last_round_counter;

        double estimated1, estimated2 = 0;
        if (esplasedFlows > 0) {
            estimated1 = (leftFlows / esplasedFlows) * durationInRealTime;
            flow_nochange_counter = 0;
        } else {
            flow_nochange_counter += 1;
            estimated1 = flow_nochange_counter * durationInRealTime * leftFlows;
        }

        if (esplasedRounds > 0) {
            estimated2 = (leftRounds / esplasedRounds) * durationInRealTime;
            round_nochange_counter = 0;
        } else {
            round_nochange_counter += 1;
            estimated2 = round_nochange_counter * durationInRealTime * leftRounds;
        }

        auto estimated = estimated1 > estimated2 ? estimated1 : estimated2;
        estimatedLeftTime = .2 * estimatedLeftTime + .8 * estimated;
        last_flow_counter = flow_counter;
        last_round_counter = round_counter;
        std::cout << "Elapsed " << timeToStr(now - simStartRealTime) << " ";
        std::cout << "ETA: " << timeToStr(estimatedLeftTime) << endl;
        if (!flow_is_over || !job_is_over) {
            scheduleAfter(progressInterval * simsecPerSecond, stopWatch);
        }
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
