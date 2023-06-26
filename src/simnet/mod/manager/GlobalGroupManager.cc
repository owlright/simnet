#include "GlobalGroupManager.h"
#include <fstream>

Define_Module(GlobalGroupManager);

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

    if (roundMeter->counter == jobInfodb.at(groupAddr)->numWorkers) {
        emit(roundMeter->roundFctSignal, simTime() - roundMeter->startTime);
        roundMeter->startTime = SimTime::getMaxTime();
        roundMeter->counter = 0;
    }
}

void GlobalGroupManager::initialize(int stage)
{
    GlobalView::initialize(stage);
    if (stage == INITSTAGE_ASSIGN) {
        placeJobs(par("placementPolicy").stringValue());
        calcAggTree(par("aggTreeType").stringValue());
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
        EV << std::left
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
            if (tokens.size() != 7)
                throw cRuntimeError("wrong line in module file: 8 items required, line: \"%s\"", line.c_str());

            auto workerAddr   = atol(tokens[0].c_str());
            auto bitmap0Index = atol(tokens[1].c_str());
            auto switch0Addr  = atol(tokens[2].c_str());
            auto fanIndegree0 = atol(tokens[3].c_str());
            auto bitmap1Index = atol(tokens[4].c_str());
            auto switch1Addr  = atol(tokens[5].c_str());
            auto fanIndegree1 = atol(tokens[6].c_str());
            EV << std::left
                << std::setw(10) << workerAddr
                << std::setw(10) << bitmap0Index
                << std::setw(10) << switch0Addr
                << std::setw(15) << fanIndegree0
                << std::setw(10) << bitmap1Index
                << std::setw(10) << switch1Addr
                << std::setw(15) << fanIndegree1
                << endl;
            std::shared_ptr<JobSwitchInfo> entry(new JobSwitchInfo());
            entry->switch0 = switch0Addr;
            entry->switch1 = switch1Addr;
            entry->fanIndegree0 = fanIndegree0;
            entry->fanIndegree1 = fanIndegree1;
            entry->bitmap0 = bitmap0Index > 0 ? (1 << (bitmap0Index-1)) : 0;
            entry->bitmap1 = bitmap1Index > 0 ? (1 << (bitmap1Index-1)) : 0;
            auto it = jobInfo.find(workerAddr);
            ASSERT(it != jobInfo.end());
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
        EV << std::left << std::setw(50) << "workers" << std::setw(30) << "PSes" << endl;
        while (getline(hostConfig, line, '\n')) {
            if (line.empty() || line[0] == '#')
                continue;
            std::vector<std::string> tokens = cStringTokenizer(line.c_str()).asVector();
            if (tokens.size() != 2)
                throw cRuntimeError("wrong line in module file: 2 items required, line: \"%s\"", line.c_str());
            // get fields from tokens
            auto workerAddrsStr = tokens[0].c_str();
            auto workerAddrs = cStringTokenizer(workerAddrsStr, "[,]").asIntVector();
            auto PSAddrsStr = tokens[1].c_str();
            auto PSAddrs = cStringTokenizer(PSAddrsStr, "[,]").asIntVector();
            EV << std::setw(50) << workerAddrsStr << std::setw(30) << PSAddrsStr << endl;
            insertJobInfodb(workerAddrs, PSAddrs);
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

std::vector<IntAddress> GlobalGroupManager::getShortestPath(cTopology &tree, cTopology::Node *start, cTopology::Node *stop)
{
    // the end nodes must be host
    ASSERT(start->getModule()->getProperties()->get("host")!=nullptr &&
           stop->getModule()->getProperties()->get("host")!=nullptr);
    auto node2addr = [](cTopology::Node *node) -> IntAddress {
        return node->getModule()->par("address").intValue();
        };
    std::vector<IntAddress> path;
    auto node = start;
    tree.calculateWeightedSingleShortestPathsTo(stop);

    while (node != stop) {
        path.emplace_back(node2addr(node));
        auto nextNode = node->getPath(0)->getRemoteNode();
        node = nextNode;
    }
    path.emplace_back(node2addr(stop));
    return path;
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

void GlobalGroupManager::placeJobs(const char *policyName)
{
    if (strcmp(policyName, "manual") == 0) {
        readHostConfig(par("groupHostFile").stringValue());
    } else if (strcmp(policyName, "random") == 0) {
        int numGroups = par("numGroups").intValue();
        int numWorkers = par("numWorkers").intValue();
        int numPSes = par("numPSes").intValue();
        for (auto i = 0; i < numGroups; i++) {
            std::vector<int> workers, workerPorts;
            std::vector<int> pses, psPorts;
            std::unordered_set<int> visited; // ! make a group member
            for (auto j = 0; j < numWorkers + numPSes; j++) {
                int index = hostNodes.size();
                do {
                    index = intrand(hostNodes.size());
                } while (visited.find(index) != visited.end());
                visited.insert(index);
                int nodeId = hostNodes.at(index);
                int address = node2addr.at(nodeId);
                if (j < numWorkers) {
                    workers.push_back(address);
                }
                else {
                    pses.push_back(address);
                }
            }
            insertJobInfodb(workers, pses);
            createJobApps(getCurrentJobId());
        }
    }
}

void GlobalGroupManager::createJobApps(int jobId)
{
    auto job = jobInfodb.at(jobId);
    auto workers = job->workers;
    auto nWorkers = job->numWorkers;
    auto nPSes = job->numPSes;
    for (auto i = 0; i < nWorkers + nPSes; i++) {
        auto isWorker = i < nWorkers;
        auto nodeAddress =  (isWorker ? job->workers[i] : job->PSes[i-nWorkers]);
        auto node = addr2mod.at(nodeAddress);
        auto appExistSize = node->getSubmoduleVectorSize("apps");
        node->setSubmoduleVectorSize("apps", appExistSize + 1);
        auto appType = (isWorker ? "simnet.app.SRWorker" : "simnet.app.ParameterServerApp");
        cModule *app = cModuleType::get(appType)->create("apps", node, appExistSize);
        app->par("port") = 2000 + appExistSize;
        app->par("jobId") = jobId;
        if (isWorker) {
            app->par("workerId") = i;
            app->par("numWorkers") = nWorkers;
            app->par("destAddress") = job->PSes[0];
        } else {
            app->par("groupAddress") = job->multicastAddresses[0];
            app->par("psId") = i - nWorkers;
        }
        app->finalizeParameters();
        app->buildInside();
        app->scheduleStart(simTime());
        auto inGate = app->gate("in");
        auto outGate = app->gate("out");
        auto at = node->getSubmodule("at");
        at->setGateSize("localIn", at->gateSize("localIn") + 1);
        at->setGateSize("localOut", at->gateSize("localOut") + 1);
        at->gate("localOut",  at->gateSize("localIn")-1)->connectTo(inGate);
        outGate->connectTo(at->gate("localIn", at->gateSize("localOut")-1));
    }
}

void GlobalGroupManager::insertJobInfodb(const std::vector<int>& workers, const std::vector<int>& pses)
{
    PortNumber port = 2001;
    std::vector<int> workerPorts;
    for (auto i = 0; i < workers.size(); i++) {
        workerPorts.push_back(port++);
    }
    std::vector<int> PSPorts;
    std::vector<int> multicastAddrs;
    for (auto i = 0; i < pses.size(); i++) {
        PSPorts.push_back(port++);
        multicastAddrs.push_back(getNextGroupAddr());
    }
    auto entry(new JobHostInfo());
    entry->jobId = getNextJobId();
    entry->workers = workers;
    entry->PSes = pses;
    entry->workerPorts = workerPorts;
    entry->PSPorts = PSPorts;
    entry->numWorkers = workers.size();
    entry->numPSes = pses.size();
    entry->multicastAddresses = multicastAddrs;
    jobInfodb[entry->jobId] = entry;
}
