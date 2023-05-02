#include "GlobalGroupManager.h"
#include <fstream>

Define_Module(GlobalGroupManager);

AggGroupEntry *GlobalGroupManager::getGroupEntry(IntAddress group)
{
    auto entry = new AggGroupEntry(1, 1); // FIXME
    return entry;
}

void GlobalGroupManager::initialize(int stage)
{
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
            if (tokens.size() != 4)
                throw cRuntimeError("wrong line in module file: 3 items required, line: \"%s\"", line.c_str());
            // get fields from tokens
            long groupAddr = atol(tokens[0].c_str());
            long treeIndex = atol(tokens[1].c_str());
            long switchAddr = atol(tokens[2].c_str());
            long fanIndegree = atol(tokens[3].c_str());
            EV << "groupAddress:" << groupAddr
            << " treeIndex:" << treeIndex
            << " switchAddress:" << switchAddr
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
            EV << "groupAddress:" << groupAddr
            << " treeIndex:" << treeIndex
            << " hostAddress:" << hostAddr
            << " isHost:" << isRoot
            << endl;
        }
    }
}
