#include <iostream>
#include <fstream>
#include <algorithm> // std::shuffle
#include <numeric>   // std::iota
#include <random>    // std::default_random_engine
#include "GlobalGroupManager.h"

std::ostream& operator<<(std::ostream& os, const std::vector<cTopology::Node*>& array)
{
    os << "[";
    for (auto &elem : array) {
        os << " " << elem->getModule()->getFullPath();
    }
    os << " ] ";
    return os;
}

Define_Module(GlobalGroupManager);

void GlobalGroupManager::initialize(int stage)
{
    GlobalManager::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        placementPolicy = par("placementPolicy");

        aggTreeType = par("aggTreeType");
        if (strcmp(aggTreeType, "") != 0)
            useInc = true;
    }
    else if (stage == INITSTAGE_ASSIGN) {
        placeJobs(placementPolicy);
        for (auto& [jobid, info] : jobInfodb) {
            EV << "job " << jobid << endl;
            EV << "workers: " << info->workers << endl;
            EV << "pses: " << info->PSes << endl;
        }

        if (useInc) {
            calcAggTree(aggTreeType);
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

void GlobalGroupManager::addShortestPath(cTopology& tree, cTopology::Node* start, cTopology::Node* stop)
{
    topo->calculateWeightedSingleShortestPathsTo(stop);
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

void GlobalGroupManager::buildSteinerTree(cTopology& tree, const std::vector<cTopology::Node*>& leaves, cTopology::Node* root)
{
    tree.addNode(new cTopology::Node(root->getModuleId())); // ! must new a node
    for (auto& n:leaves) {
        double dist = INFINITY;
        // * find the joint node to the tree
        cTopology::Node* jointNode = nullptr;
        for (auto i = 0; i < tree.getNumNodes(); i++) {
            // ! Node* of the same Module in tree and topo are different
            auto nodeInTree = topo->getNodeFor(tree.getNode(i)->getModule());
            if (nodeInTree == root ||
                    nodeInTree->getModule()->getProperties()->get("switch")!=nullptr)
            { // ! ignore hosts
                topo->calculateWeightedSingleShortestPathsTo(nodeInTree);
                if (n->getDistanceToTarget() < dist) {
                    dist = n->getDistanceToTarget();
                    jointNode = nodeInTree;
                }
            }
        }
        ASSERT(jointNode);
        // * add the node into tree using the shortest path
        addShortestPath(tree, n, jointNode);
    }
}

void GlobalGroupManager::placeJobs(const char *policyName)
{
    if (strcmp(policyName, "manual") == 0) {
        readHostConfig(par("groupHostFile").stringValue());
    }
    else if (strcmp(policyName, "random") == 0) {
        int numUsedHosts = hostIds.size();
        auto hostscopy = hostIds;

        std::shuffle(hostscopy.begin(), hostscopy.end(), std::default_random_engine(intrand(3245)));
        std::vector<int> hostIds(hostscopy.begin(), hostscopy.begin()+numUsedHosts);

        int numGroups = par("numGroups").intValue();
        int numWorkers = par("numWorkers").intValue();
        int numPSes = par("numPSes").intValue();

        if (numGroups*(numWorkers+numPSes) > numUsedHosts) {
            throw cRuntimeError("used too many hosts, only %d can use.", numUsedHosts);
        }

        // TODO if allow two workers or pses on the same host ?
        // ! a host can only be use once
        int count = 0;
        for (auto i = 0; i < numGroups; i++) {
            std::vector<int> workers;
            std::vector<int> pses;
            for (auto j = 0; j < numWorkers + numPSes; j++) {
                int nodeId = hostIds.at(count++);
                int address = getAddr(nodeId);
                if (j < numWorkers)
                    workers.push_back(address);
                else
                    pses.push_back(address);
            }
            insertJobInfodb(workers, pses);
            createJobApps(getCurrentJobId());
        }
        // ! different job's worker or worker and ps can be on the same host
        // for (auto i = 0; i < numGroups; i++) {
        //     std::vector<int> workers, workerPorts;
        //     std::vector<int> pses, psPorts;
        //     std::unordered_set<int> visited;
        //     for (auto j = 0; j < numWorkers + numPSes; j++) {
        //         int index = numUsedHosts; // this is invalid afterall
        //         do {
        //             index = intrand(hostIds.size());
        //         } while (visited.find(index) != visited.end());
        //         visited.insert(index);
        //         int nodeId = hostNodes.at(index);
        //         int address = node2addr.at(nodeId);
        //         if (j < numWorkers) {
        //             workers.push_back(address);
        //         }
        //         else {
        //             pses.push_back(address);
        //         }
        //     }
        //     insertJobInfodb(workers, pses);
        //     createJobApps(getCurrentJobId());
        // }
    }
    else if (strcmp(policyName, "") == 0) {
        EV_WARN << "You may forget to set a placement policy for agg groups, make sure you manually set them." << endl;
    }
    else {
        throw cRuntimeError("invalid placement policy.");
    }
}

void GlobalGroupManager::createJobApps(int jobId)
{
    auto job = jobInfodb.at(jobId);
    auto workers = job->workers;
    auto nWorkers = job->numWorkers;
    auto nPSes = job->numPSes;

    if (nPSes > 1)
        throw cRuntimeError("Not ready for multiple servers.");

    for (auto i = 0; i < nPSes; i++) {
        auto nodeAddress =  job->PSes[i];
        auto node = getMod(nodeAddress);
        // * find an idle worker
        auto appExistSize = node->getSubmoduleVectorSize("pses");

        node->setSubmoduleVectorSize("pses", appExistSize + 1);
        auto appType = "simnet.app.ParameterServerApp";
        auto app = cModuleType::get(appType)->create("pses", node, appExistSize);
        // ! port number can only be decided here because we don't know how many pses will
        // ! be setup, but workers need this to create connection, so PSes must be setup before workers
        job->PSPorts[i] = 3000 + appExistSize;
        app->par("jobId") = jobId;
        app->par("numWorkers") = nWorkers;
        app->par("port") = job->PSPorts[i];
        app->par("groupAddress") = job->multicastAddresses[i];
        // only new apps can set these fields
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
        app->callInitialize(INITSTAGE_LOCAL);
     }


    for (auto i = 0; i < nWorkers; i++) {
        auto nodeAddress =  job->workers[i];
        auto node = getMod(nodeAddress);
        // * find an idle worker
        auto appExistSize = node->getSubmoduleVectorSize("workers");

        node->setSubmoduleVectorSize("workers", appExistSize + 1);
        auto appType = useInc ? "simnet.app.SRWorker" : "simnet.app.WorkerApp";
        auto app = cModuleType::get(appType)->create("workers", node, appExistSize);
        job->workerPorts[i] = 2000 + appExistSize; // ! port number can only be decided here
        // only new apps can set these fields
        app->par("port") = job->workerPorts[i];
        app->par("jobId") = jobId;
        app->par("workerId") = i;
        app->par("numWorkers") = nWorkers;
        app->par("destAddress") = job->PSes[0];
        app->par("destPort") = job->PSPorts[0];
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
        app->callInitialize(INITSTAGE_LOCAL);
    }

}

void GlobalGroupManager::calcAggTree(const char *policyName)
{
    if (strcmp(policyName, "manual") == 0)
    {
        // readSwitchConfig(par("groupSwitchFile").stringValue());
        // TODO manually set segments for each host
    }
    else if (strcmp(policyName, "sptree") == 0)
    {
        for (auto it : jobInfodb) {
            auto jobid = it.first;
            EV_TRACE << "job " << jobid << endl;
            auto group = it.second;
            auto senders = group->workers;
            auto ps = group->PSes.at(0);
            cTopology tree = cTopology("steiner");

            std::vector<cTopology::Node*> senderNodes;
            for (auto& s:senders) {
                senderNodes.push_back(getNode(s));
            }
            buildSteinerTree(tree, senderNodes, getNode(ps)); //  TODO multiple PSes

            std::vector<cModule*> senderMods;
            for (auto& s:senders) {
                senderMods.push_back(getMod(s));
            }

            // * HACK the simplest ecmp tree
            // * iterate over all tree's edges and nodes, add weight 1 onto them in topo
            // * prepare segments
            for (auto i = 0; i < senders.size(); i++) {
                auto addr = senders[i];
                auto m = senderMods[i];
                auto path = getShortestPath(tree, tree.getNodeFor(m), tree.getNodeFor(getMod(ps)));
                ASSERT(path.size() >= 3);
                EV_DEBUG << path.front() << "->" << path.back() << ":" << path << endl;
                auto segments = std::vector<int>(path.begin()+1, path.end()-1);
                std::unordered_set<cTopology::Node*> visitedNodes;
                std::unordered_set<cTopology::Link*> visitedLinks;
                for (auto j = 0; j < segments.size() - 1; j++) {
                    auto u = getNode(segments[j]);
                    auto v = getNode(segments[j+1]);
                    if (visitedNodes.find(u) == visitedNodes.end()) {
                        visitedNodes.insert(u);
                        u->setWeight(u->getWeight() + 1);
                    }
                    // * get the edge u->v
                    auto nOutEdges = u->getNumOutLinks();
                    for (auto k = 0; k < nOutEdges; k++) {
                        auto outEdge = u->getLinkOut(k);
                        if (outEdge->getRemoteNode() == v && visitedLinks.find(outEdge) == visitedLinks.end()) {
                            outEdge->setWeight(outEdge->getWeight() + 1); // ! avoid always use the same link
                        }
                    }

                }

                // * prepare args(indegree) at each segment
                std::vector<int> indegrees;
                for (auto& seg:segments) {
                    indegrees.push_back(tree.getNodeFor(getMod(seg))->getNumInLinks());
                }
                EV_TRACE << indegrees << endl;
                segmentInfodb[jobid][addr][ps] = new JobSegmentsRoute();
                segmentInfodb[jobid][addr][ps]->segmentAddrs = segments;
                segmentInfodb[jobid][addr][ps]->fanIndegrees = indegrees;
                for (auto i = 0; i < m->getSubmoduleVectorSize("workers"); i++) {
                    auto app = m->getSubmodule("workers", i);
                    if (app->hasPar("segmentAddrs") && ps == app->par("destAddress").intValue()) {
                        app->par("segmentAddrs") = vectorToString(segments);
                        app->par("fanIndegrees") = vectorToString(indegrees);
                    }
                }
            }
        }
    }
    else if (strcmp(policyName, "edge") == 0) {
        for (auto it : jobInfodb)
        {
            EV_DEBUG << "job " <<  it.first<< endl;
            auto group = it.second;
            auto senders = group->workers;
            auto ps = group->PSes.at(0);
            auto psNode = getNode(ps);
            // EV_TRACE << senders << " " << ps << endl;
            cTopology tree = cTopology("steiner");
            std::vector<cTopology::Node*> senderNodes;
            for (auto& s:senders) {
                senderNodes.push_back(getNode(s));
            }
            buildSteinerTree(tree, senderNodes, psNode);
            // ! if only do aggregation at edge, the indegree calculation is a litte harder
            std::unordered_map<IntAddress, int> indegrees;
            std::unordered_map<IntAddress, std::vector<IntAddress>> addrSegments;
            for (auto& s:senders) {
                auto srcNode = getNode(s);
                auto edge0Switch = srcNode->getLinkOut(0)->getRemoteNode();
                auto edge1Switch = psNode->getLinkIn(0)->getRemoteNode();
                // * prepare segments
                std::vector<IntAddress> segments;
                segments.push_back(getAddr(edge0Switch->getModule()));
                if (edge1Switch!=edge0Switch)
                    segments.push_back(getAddr(edge1Switch->getModule()));
                addrSegments[s] = segments;
                EV_TRACE << segments << endl;
                // std::cout << segments << endl;
                for (int i = segments.size() - 1; i >= 0; i--) {
                    auto seg = segments[i];
                    if (indegrees.find(seg) == indegrees.end()) {
                        indegrees[seg] = 1;
                    }
                    else {
                        if ((i == 0) || (i == 1 && indegrees.find(segments[i-1]) == indegrees.end())) {
                            // * this is still a new flow
                            indegrees[seg] += 1;
                        }
                    }
                }

                // for (auto i = 0; i < srcMod->getSubmoduleVectorSize("workers"); i++) {
                //     auto app = srcMod->getSubmodule("workers", i);
                //     if (app->hasPar("segmentAddrs") && ps == app->par("destAddress").intValue()) {
                //         app->par("segmentAddrs") = vectorToString(segments);
                //         std::vector<int> tmp;
                //         for (auto s:segments) {
                //             tmp.push_back(indegrees.at(s));
                //         }
                //         app->par("fanIndegrees") = vectorToString(tmp);
                //     }
                // }
            }
            for (auto& s:senders) {
                auto srcMod = getMod(s);
                auto segments = addrSegments[s];
                for (auto i = 0; i < srcMod->getSubmoduleVectorSize("workers"); i++) {
                    auto app = srcMod->getSubmodule("workers", i);
                    if (app->hasPar("segmentAddrs") && ps == app->par("destAddress").intValue()) {
                        app->par("segmentAddrs") = vectorToString(segments);
                        std::vector<int> tmp;
                        for (auto s:segments) {
                            tmp.push_back(indegrees.at(s));
                        }
                        app->par("fanIndegrees") = vectorToString(tmp);
                    }
                }
            }
            // for (auto& it:indegrees) {
            //     std::cout << it.first << " " << it.second << endl;
            // }
            // std::cout << endl;
        }
    }
    else if (strcmp(policyName, "") == 0) {
        EV_WARN << "You may forget to set the aggTreeType. No AggTree will be built!" << endl;
    }
    else {
        throw cRuntimeError("invalid aggTreeType");
    }
}

void GlobalGroupManager::insertJobInfodb(const std::vector<int>& workers, const std::vector<int>& pses)
{
    std::vector<int> workerPorts(workers.size(), INVALID_PORT);

    std::vector<int> PSPorts(pses.size(), INVALID_PORT);
    std::vector<int> multicastAddrs;
    for (auto i = 0; i < pses.size(); i++) {
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
