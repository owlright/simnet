#include "GlobalGroupManager.h"
#include <fstream>

Define_Module(GlobalGroupManager);

// IntAddress GlobalGroupManager::getGroupAddress(IntAddress fromNode) const
// {
//     if (hostGroupInfo.find(fromNode) == hostGroupInfo.end())
//     {
//         return INVALID_ADDRESS;
//     }
//     return hostGroupInfo.at(fromNode).at(0);
// }

// IntAddress GlobalGroupManager::getGroupRootAddress(IntAddress groupAddr) const
// {
//     auto it = groupRoot.find(std::make_pair(groupAddr, 0));
//     if (it == groupRoot.end())
//     {
//         return INVALID_ADDRESS;
//     }
//     return it->second;
// }

// int GlobalGroupManager::getSendersNumber(IntAddress groupAddr) const
// {
//     return groupSources.at(std::make_pair(groupAddr, 0)).size(); // minus the parameter server
// }

// int GlobalGroupManager::getTreeIndex(IntAddress fromNode) const
// {
//     if (hostGroupInfo.find(fromNode) == hostGroupInfo.end())
//     {
//         return INVALID_ADDRESS;
//     }
//     return hostGroupInfo.at(fromNode).at(1);
// }

// int GlobalGroupManager::getFanIndegree(IntAddress group, int treeIndex, IntAddress switchAddress) const
// {
//     auto it = switchFanIndegree.find(std::make_tuple(group, treeIndex, switchAddress));
//     if (it == switchFanIndegree.end())
//         return -1;
//     else
//         return it->second;

// }

// int GlobalGroupManager::getBufferSize(IntAddress group, IntAddress switchAddress) const
// {
//     auto it = switchBufferSize.find(std::make_pair(group, switchAddress));
//     if (it == switchBufferSize.end())
//         throw cRuntimeError("bufferSize not found.");
//     return it->second;
// }

const GroupInfoWithIndex*
GlobalGroupManager::getGroupHostInfo(IntAddress hostAddr) const
{
    auto it = hostGroupInfo.find(hostAddr);
    if (it != hostGroupInfo.end())
        return it->second;
    return nullptr;
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

    if (roundMeter->counter == groupHostInfodb.at(groupAddr)->numWorkers) {
        emit(roundMeter->roundFctSignal, simTime() - roundMeter->startTime);
        roundMeter->startTime = simTime();
        roundMeter->counter = 0;
    }
}

void GlobalGroupManager::initialize(int stage)
{
    GlobalView::initialize(stage);
    if (stage == INITSTAGE_COLLECT) {
        prepareAggGroup(par("groupPolicy").stringValue());
    }
    if (stage == INITSTAGE_LAST)
        ASSERT(topo);
}

void GlobalGroupManager::readSwitchConfig(const char * fileName)
{
    std::fstream switchConfig(fileName, std::ios::in);
    if (!switchConfig) {
        throw cRuntimeError("%s not found!", fileName);
    } else {
        std::string line;
        EV << std::left << std::setw(15) << "groupAddress"
            << std::setw(10) << "worker"
            << std::setw(10) << "bitmap0"
            << std::setw(10) << "switch0"
            << std::setw(15) << "fanIndegree0"
            << std::setw(10) << "bitmap1"
            << std::setw(10) << "switch1"
            << std::setw(15) << "fanIndegree1"
            << endl;
        while (getline(switchConfig, line, '\n')) {
            if (line.empty() || line[0] == '#')
                continue;
            std::vector<std::string> tokens = cStringTokenizer(line.c_str()).asVector();
            if (tokens.size() != 8)
                throw cRuntimeError("wrong line in module file: 8 items required, line: \"%s\"", line.c_str());
            // // get fields from tokens
            auto groupAddr    = atol(tokens[0].c_str());
            auto workerAddr   = atol(tokens[1].c_str());
            auto bitmap0Index = atol(tokens[2].c_str());
            auto switch0Addr  = atol(tokens[3].c_str());
            auto fanIndegree0 = atol(tokens[4].c_str());
            auto bitmap1Index = atol(tokens[5].c_str());
            auto switch1Addr  = atol(tokens[6].c_str());
            auto fanIndegree1 = atol(tokens[7].c_str());
            EV << std::left << std::setw(15) << "groupAddress"
                << std::setw(10) << workerAddr
                << std::setw(10) << bitmap0Index
                << std::setw(10) << switch0Addr
                << std::setw(15) << fanIndegree0
                << std::setw(10) << bitmap1Index
                << std::setw(10) << switch1Addr
                << std::setw(15) << fanIndegree1
                << endl;
            std::shared_ptr<GroupSwitchInfo> entry(new GroupSwitchInfo());
            entry->switch0 = switch0Addr;
            entry->switch1 = switch1Addr;
            entry->fanIndegree0 = fanIndegree0;
            entry->fanIndegree1 = fanIndegree1;
            entry->bitmap0 = bitmap0Index > 0 ? (1 << bitmap0Index) : bitmap0Index;
            entry->bitmap1 = bitmap0Index > 0 ? (1 << bitmap1Index) : bitmap1Index;
            auto it = hostGroupInfo.find(workerAddr);
            ASSERT(it != hostGroupInfo.end());
            it->second->switchinfo = entry;
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
        int jobId = 0; // which is also the index in database
        EV << std::left << std::setw(20) << "groupAddress" << std::setw(50) << "workers" << std::setw(30) << "PSes" << endl;
        while (getline(hostConfig, line, '\n')) {
            if (line.empty() || line[0] == '#')
                continue;
            std::vector<std::string> tokens = cStringTokenizer(line.c_str()).asVector();
            if (tokens.size() != 3)
                throw cRuntimeError("wrong line in module file: 3 items required, line: \"%s\"", line.c_str());
            // get fields from tokens
            auto groupAddr = atol(tokens[0].c_str());
            auto workerAddrsStr = tokens[1].c_str();
            auto workerAddrs = cStringTokenizer(workerAddrsStr, "[,]").asIntVector();
            auto PSAddrsStr = tokens[2].c_str();
            auto PSAddrs = cStringTokenizer(PSAddrsStr, "[,]").asIntVector();
            std::shared_ptr<GroupHostInfo> entry(new GroupHostInfo());
            entry->jobId = jobId++;
            entry->groupAddress = groupAddr;
            entry->workers = workerAddrs;
            entry->PSes = PSAddrs;
            entry->numWorkers = workerAddrs.size();
            entry->numPSes = PSAddrs.size();
            groupHostInfodb.push_back(entry);
            EV << std::setw(20) << groupAddr << std::setw(50) << workerAddrsStr << std::setw(30) << PSAddrsStr << endl;
            for (auto i = 0; i < workerAddrs.size(); i++)
            {
                auto hostEntry = new GroupInfoWithIndex();
                hostEntry->hostinfo = entry;
                hostEntry->isWorker = true;
                hostEntry->index = i;
                auto addr = workerAddrs.at(i);
                ASSERT(hostGroupInfo.find(addr) == hostGroupInfo.end()); // one host only in one group
                hostGroupInfo[addr] = hostEntry;
            }
            for (auto i = 0; i < PSAddrs.size(); i++)
            {
                auto hostEntry = new GroupInfoWithIndex();
                hostEntry->hostinfo = entry;
                hostEntry->isWorker = false;
                hostEntry->index = i;
                auto addr = PSAddrs.at(i);
                ASSERT(hostGroupInfo.find(addr) == hostGroupInfo.end()); // ! server and worker can't be together
                hostGroupInfo[addr] = hostEntry;
            }
        }
    }
}

simsignal_t GlobalGroupManager::createSignalForGroup(IntAddress group)
{
    char signalName[32];
    sprintf(signalName, "group%lld-RoundFinishTime", group);
    simsignal_t signal = registerSignal(signalName);

    char statisticName[32];
    sprintf(statisticName, "group%lld-RoundFinishTime", group);
    cProperty *statisticTemplate =
        getProperties()->get("statisticTemplate", "groupRoundFinishTime");
    getEnvir()->addResultRecorders(this, signal, statisticName, statisticTemplate);
    return signal;
}

void GlobalGroupManager::addShortestPath(cTopology& tree, cTopology::Node* start, cTopology::Node* stop)
{
    topo->calculateUnweightedSingleShortestPathsTo(stop);
    auto node = start;
    auto treeNode = new cTopology::Node(node->getModuleId()); // ! must new a node
    tree.addNode(treeNode);
    while (node != stop) {
        auto nextNode = node->getPath(0)->getRemoteNode();
        cTopology::Node* nextTreeNode = nullptr;
        if (nextNode != stop) {
            nextTreeNode = new cTopology::Node(nextNode->getModuleId());
            tree.addNode(nextTreeNode);
        }
        else { // avoid add a node twice into tree
            nextTreeNode = tree.getNodeFor(nextNode->getModule());
        }
        // the link is always a new one
        tree.addLink(new cTopology::Link(), treeNode, nextTreeNode);
        node = nextNode;
        treeNode = nextTreeNode;
    }
}

void GlobalGroupManager::buildSteinerTree(cTopology& tree, const std::vector<int>& members, int root)
{
    auto rootNode = topo->getNode(root);
    tree.addNode(new cTopology::Node(rootNode->getModuleId())); // ! must new a node
    for (auto& n:members) {
        double dist = INFINITY;
        auto currentHost = topo->getNode(n);
        // * find the joint node to the tree
        cTopology::Node* jointNode = nullptr;
        for (auto i = 0; i < tree.getNumNodes(); i++) {
            // ! Node* of the same Module in tree and topo are different
            auto nodeInTree = topo->getNodeFor(tree.getNode(i)->getModule());
            if (nodeInTree == rootNode ||
                    nodeInTree->getModule()->getProperties()->get("switch")!=nullptr)
            { // ! ignore hosts
                topo->calculateUnweightedSingleShortestPathsTo(nodeInTree);
                if (currentHost->getDistanceToTarget() < dist) {
                    dist = currentHost->getDistanceToTarget();
                    jointNode = nodeInTree;
                }
            }
        }
        ASSERT(jointNode);
        // * add the node into tree using the shortest path
        addShortestPath(tree, currentHost, jointNode);
    }
}

void GlobalGroupManager::prepareAggGroup(const char* policyName)
{
    if (strcmp(policyName, "manual") == 0)
    {
        readHostConfig(par("groupHostFile").stringValue());
        readSwitchConfig(par("groupSwitchFile").stringValue());
    }
    else if (strcmp(policyName, "random") == 0)
    {
        // int groupNum = 2;
        // int groupMembers = 4;
        // IntAddress groupAddr = GROUPADDR_START - 1;
        // std::unordered_set<int> visited; // TODO may not necessary
        // for (auto i = 0; i < groupNum; i++)
        // {
        //     groupAddr++;
        //     auto gkey = std::make_pair(groupAddr, 0);
        //     std::vector<int> members;
        //     // * choose group members randomly without repetition
        //     for (auto j = 0; j < groupMembers ; j++)
        //     {
        //         int index = hostNodes.size();
        //         if (groupMembers > index)
        //             throw cRuntimeError("too many group members!");
        //         do {
        //             index = intrand(hostNodes.size());
        //         }
        //         while (visited.find(index) != visited.end());
        //         auto node = hostNodes.at(index);
        //         members.push_back(node);
        //         visited.insert(node);
        //         auto addr = node2addr.at(node);
        //         hostGroupInfo[addr].push_back(groupAddr);
        //         hostGroupInfo[addr].push_back(0); // TODO treeIndex is always 0 so far
        //     }
        //     auto root = members.back();
        //     members.pop_back();
        //     groupRoot[gkey] = node2addr.at(root);
        //     for (auto& m:members)
        //         groupSources[gkey].push_back(node2addr.at(m));

        //     // * get steiner tree for each group
        //     cTopology tree = cTopology("steiner");
        //     buildSteinerTree(tree, members, root);
        //     // * tree construction is finished
        //     // * now assign tree to routers
        //     for (auto i = 0; i < tree.getNumNodes(); i++) {
        //         auto node = tree.getNode(i);
        //         auto mod = node->getModule();
        //         if (mod->getProperties()->get("switch")!=nullptr) {
        //             auto indegree = node->getNumInLinks();
        //             auto switchAddr = mod->par("address").intValue();
        //             EV_DEBUG <<"switchAddr:" << std::setw(6) << switchAddr << " indegree:" << indegree << endl;
        //             if (indegree >= 2) {
        //                 switchFanIndegree[std::make_tuple(groupAddr, 0, switchAddr)] = indegree;
        //                 switchBufferSize[std::make_pair(groupAddr, switchAddr)] = 10000;
        //             }
        //         }
        //     }
        // }
    }
    else
    {
        throw cRuntimeError("you must specify a policy.");
    }
}
