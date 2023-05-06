#include "GlobalGroupManager.h"
#include <fstream>

Define_Module(GlobalGroupManager);

IntAddress GlobalGroupManager::getGroupAddress(IntAddress fromNode) const
{
    if (hostGroupInfo.find(fromNode) == hostGroupInfo.end())
    {
        return INVALID_ADDRESS;
    }
    return hostGroupInfo.at(fromNode).at(0);
}

IntAddress GlobalGroupManager::getGroupRootAddress(IntAddress groupAddr) const
{
    auto it = groupRoot.find(std::make_pair(groupAddr, 0));
    if (it == groupRoot.end())
    {
        return INVALID_ADDRESS;
    }
    return it->second;
}

int GlobalGroupManager::getTreeIndex(IntAddress fromNode) const
{
    if (hostGroupInfo.find(fromNode) == hostGroupInfo.end())
    {
        return INVALID_ADDRESS;
    }
    return hostGroupInfo.at(fromNode).at(1);
}

int GlobalGroupManager::getFanIndegree(IntAddress group, int treeIndex, IntAddress switchAddress) const
{
    auto it = switchFanIndegree.find(std::make_tuple(group, treeIndex, switchAddress));
    if (it == switchFanIndegree.end())
        throw cRuntimeError("fanIndegree not found.");
    return it->second;

}

int GlobalGroupManager::getBufferSize(IntAddress group, IntAddress switchAddress) const
{
    auto it = switchBufferSize.find(std::make_pair(group, switchAddress));
    if (it == switchBufferSize.end())
        throw cRuntimeError("bufferSize not found.");
    return it->second;
}

void GlobalGroupManager::reportFlowStart(IntAddress groupAddr, simtime_t roundStartTime)
{
    if (groupRoundStartTime.find(groupAddr) == groupRoundStartTime.end()) {
        groupRoundStartTime[groupAddr] = new groupRoundFinishInfo();
        groupRoundStartTime[groupAddr]->roundFctSignal = createSignalForGroup(groupAddr);
        groupRoundStartTime[groupAddr]->startTime = roundStartTime;
    } else {
        if (roundStartTime  < groupRoundStartTime[groupAddr]->startTime) {
            groupRoundStartTime[groupAddr]->startTime = roundStartTime;
        }
    }
}

void GlobalGroupManager::reportFlowStop(IntAddress groupAddr, simtime_t roundStopTime)
{
    auto it = groupRoundStartTime.find(groupAddr);
    if (it == groupRoundStartTime.end()) {
        throw cRuntimeError("This group hasn't registered its signal");
    }

    auto roundMeter = it->second;
    roundMeter->counter++;
    if (roundMeter->counter == groupSources.at(std::make_pair(groupAddr, 0)).size()) {
        emit(roundMeter->roundFctSignal, simTime() - roundMeter->startTime);
        roundMeter->startTime = simTime();
        roundMeter->counter = 0;
    }
}

void GlobalGroupManager::initialize(int stage)
{
    GlobalView::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        readSwitchConfig(par("groupSwitchFile").stringValue());
        readHostConfig(par("groupHostFile").stringValue());
    }

    if (stage == INITSTAGE_ASSIGN) {

    }
}

void GlobalGroupManager::readSwitchConfig(const char * fileName)
{
    std::fstream switchConfig(fileName, std::ios::in);
    if (!switchConfig) {
        throw cRuntimeError("%s not found!", fileName);
    } else {
        std::string line;
        while (getline(switchConfig, line, '\n')) {
            if (line.empty() || line[0] == '#')
                continue;
            std::vector<std::string> tokens = cStringTokenizer(line.c_str()).asVector();
            if (tokens.size() != 5)
                throw cRuntimeError("wrong line in module file: 5 items required, line: \"%s\"", line.c_str());
            // get fields from tokens
            long groupAddr = atol(tokens[0].c_str());
            long treeIndex = atol(tokens[1].c_str());
            long switchAddr = atol(tokens[2].c_str());
            long bufferSize = atol(tokens[3].c_str());
            long fanIndegree = atol(tokens[4].c_str());
            switchFanIndegree[std::make_tuple(groupAddr, treeIndex, switchAddr)] = fanIndegree;
            switchBufferSize[std::make_pair(groupAddr, switchAddr)] = bufferSize;
            EV << "groupAddress:" << groupAddr
            << " treeIndex:" << treeIndex
            << " switchAddress:" << switchAddr
            << " bufferSize:" << bufferSize
            << " fanIndegree:" << fanIndegree
            << endl;
        }
    }
}

void GlobalGroupManager::readHostConfig(const char * fileName)
{
    std::fstream hostConfig(fileName, std::ios::in);
    if (!hostConfig) {
        throw cRuntimeError("%s not found!", fileName);
    } else {
        std::string line;
        while (getline(hostConfig, line, '\n')) {
            if (line.empty() || line[0] == '#')
                continue;
            std::vector<std::string> tokens = cStringTokenizer(line.c_str()).asVector();
            if (tokens.size() != 4)
                throw cRuntimeError("wrong line in module file: 3 items required, line: \"%s\"", line.c_str());
            // get fields from tokens
            long groupAddr = atol(tokens[0].c_str());
            long treeIndex = atol(tokens[1].c_str());
            long hostAddr = atol(tokens[2].c_str());
            long isRoot = atol(tokens[3].c_str());
            hostGroupInfo[hostAddr].push_back(groupAddr);
            hostGroupInfo[hostAddr].push_back(treeIndex);
            if (isRoot == 0) {
                groupSources[std::make_pair(groupAddr, treeIndex)].push_back(hostAddr);
            } else {
                groupRoot[std::make_pair(groupAddr, treeIndex)] = hostAddr;
            }

            EV << "groupAddress:" << groupAddr
            << " treeIndex:" << treeIndex
            << " hostAddress:" << hostAddr
            << " isHost:" << isRoot
            << endl;
        }
    }
}

simsignal_t GlobalGroupManager::createSignalForGroup(IntAddress group)
{
    char signalName[32];
    sprintf(signalName, "group%d-RoundFinishTime", group);
    simsignal_t signal = registerSignal(signalName);

    char statisticName[32];
    sprintf(statisticName, "group%d-RoundFinishTime", group);
    cProperty *statisticTemplate =
        getProperties()->get("statisticTemplate", "groupRoundFinishTime");
    getEnvir()->addResultRecorders(this, signal, statisticName, statisticTemplate);
    return signal;
}
