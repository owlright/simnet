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
        std::fstream groupConfigFile(par("groupConfigFile").stringValue(), std::ios::in);
        if (!groupConfigFile)
            throw cRuntimeError("%s not found!", par("groupConfigFile").stringValue());
        std::string line;
        while (getline(groupConfigFile, line, '\n')) {
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
            EV << "group_address=" << groupAddr
               << " tree_index=" << treeIndex
               << " switch_address=" << switchAddr
               << " fanIndegree=" << fanIndegree
               << endl;
        }
    }
}
