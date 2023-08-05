#include <iostream>
#include <fstream>
#include <algorithm> // std::shuffle
#include <numeric>   // std::iota
#include <random>    // std::default_random_engine
#include <queue>
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
            auto tmp = cStringTokenizer(workerAddrsStr, "[,]").asIntVector();
            vector<IntAddress> workerAddrs(tmp.begin(), tmp.end());
            auto PSAddrsStr = tokens[1].c_str();
            tmp = cStringTokenizer(PSAddrsStr, "[,]").asIntVector();
            vector<IntAddress> PSAddrs(tmp.begin(), tmp.end());
            EV << std::setw(50) << workerAddrsStr << std::setw(30) << PSAddrsStr << endl;
            insertJobInfodb(workerAddrs, PSAddrs);
        }
    }
}

void GlobalGroupManager::addShortestPath(cTopology* tree, cTopology::Node* start, cTopology::Node* stop)
{
    topo->calculateWeightedSingleShortestPathsTo(stop);
    auto node = start;
    auto treeNode = new cTopology::Node(node->getModuleId()); // ! must new a node
    tree->addNode(treeNode);
    while (node != stop) {
        auto nextNode = node->getPath(0)->getRemoteNode();
        cTopology::Node* nextTreeNode = nullptr;
        if (nextNode != stop) {
            nextTreeNode = new cTopology::Node(nextNode->getModuleId());
            tree->addNode(nextTreeNode);
        }
        else { // avoid add a node twice into tree
            nextTreeNode = tree->getNodeFor(nextNode->getModule());
        }
        // the link is always a new one
        tree->addLink(new cTopology::Link(), treeNode, nextTreeNode);
        node = nextNode;
        treeNode = nextTreeNode;
    }
}

std::vector<IntAddress> GlobalGroupManager::getShortestPath(cTopology* tree, cTopology::Node *start, cTopology::Node *stop)
{
    // the end nodes must be host
    ASSERT(start->getModule()->getProperties()->get("host")!=nullptr &&
           stop->getModule()->getProperties()->get("host")!=nullptr);
    auto node2addr = [](cTopology::Node *node) -> IntAddress {
        return node->getModule()->par("address").intValue();
        };
    std::vector<IntAddress> path;
    auto node = start;
    tree->calculateWeightedSingleShortestPathsTo(stop);

    while (node != stop) {
        path.emplace_back(node2addr(node));
        auto nextNode = node->getPath(0)->getRemoteNode();
        node = nextNode;
    }
    path.emplace_back(node2addr(stop));
    return path;
}

void GlobalGroupManager::addCost(const cTopology* tree, double node_cost, double edge_cost)
{
    // const double node_cost = 0.1;
    // const double edge_cost = 0.1;
    vector<IntAddress> leaves, aggNodes;
    IntAddress root;
    vector<std::pair<IntAddress, IntAddress>> edges; // duplication is allowed
    for (auto i = 0; i < tree->getNumNodes(); i++) {
        auto node = tree->getNode(i);
        auto indegree = node->getNumInLinks();
        auto outdegree = node->getNumOutLinks();
        if (indegree >= 2) {
            aggNodes.push_back(getAddr(node));
        }
        else if (indegree == 0) {
            leaves.push_back(getAddr(node));
        }
        else if (outdegree == 0) {
            root = getAddr(node);
        }
    }

    for (auto n:aggNodes) {
        auto node = getNode(n);
        node->setWeight(node->getWeight() + node_cost);
    }
    std::queue<IntAddress, std::deque<IntAddress> > queue;
    std::for_each(leaves.cbegin(), leaves.cend(), [&queue](IntAddress n){queue.push(n);});
    std::unordered_set<IntAddress> visited;
    while (!queue.empty()) {
        auto addr = queue.front();
        queue.pop();
        auto u = tree->getNodeFor(getMod(addr));
        if (u->getNumOutLinks() == 0) {
            ASSERT(getAddr(u) == root);
        }
        else {
            auto v = u->getLinkOut(0)->getRemoteNode();
            edges.push_back(std::make_pair(getAddr(u), getAddr(v)));
            if (visited.find(getAddr(v)) != visited.end())
                queue.push(getAddr(v));
            visited.insert(getAddr(v));
        }
    }
    auto get_edge = [this](decltype(edges.front()) e, const cTopology* graph) -> cTopology::LinkOut* {
        auto u = graph->getNodeFor(getMod(e.first));
        auto v = graph->getNodeFor(getMod(e.second));
        auto nOutEdges = u->getNumOutLinks();
        for (auto k = 0; k < nOutEdges; k++) {
            auto outEdge = u->getLinkOut(k);
            if (outEdge->getRemoteNode() == v) {
                return outEdge;
            }
        }
        return nullptr;
    };
    for (auto e:edges) {
        auto edge = get_edge(e, topo);
        edge->setWeight(edge->getWeight() + edge_cost);
        edge = get_edge(e, tree);
        edge->setWeight(edge->getWeight() + edge_cost);
    }
    // auto print = [](const std::pair<IntAddress, IntAddress>& e) { std::cout << e.first << "->" << e.second << endl; };
    // std::for_each(edges.cbegin(), edges.cend(), print);
}

cTopology*
GlobalGroupManager::buildSteinerTree(const std::vector<IntAddress>& leaves, const IntAddress& root, vector<IntAddress>& aggNodes)
{
    auto tree = new cTopology("steiner");
    tree->addNode(new cTopology::Node(getNode(root)->getModuleId())); // ! must new a node
    std::unordered_set<IntAddress> used{root};
    for (auto& leaf:leaves) {
        double dist = INFINITY;
        // * find the joint node to the tree
        cTopology::Node* jointNode = nullptr;
        for (auto i = 0; i < tree->getNumNodes(); i++) {
            // ! Node* of the same Module in tree and topo are different
            auto nodeInTree = topo->getNodeFor(tree->getNode(i)->getModule());
            if (getAddr(nodeInTree) == root ||
                    nodeInTree->getModule()->getProperties()->get("switch")!=nullptr)
            { // ! ignore hosts
                topo->calculateWeightedSingleShortestPathsTo(nodeInTree);
                if (getNode(leaf)->getDistanceToTarget() < dist) {
                    dist = getNode(leaf)->getDistanceToTarget();
                    jointNode = nodeInTree;
                }
            }
        }
        ASSERT(jointNode);
        auto addr = getAddr(jointNode);
        if (used.find(addr) == used.end())
            aggNodes.push_back(addr);
        used.insert(addr);
        // * add the node into tree using the shortest path
        addShortestPath(tree, getNode(leaf), jointNode); // TODO: add weight to avoid two path overlap;
    }
    return tree;
}

std::unordered_map<IntAddress, vector<IntAddress>>
GlobalGroupManager::findEqualCostAggNodes(const cTopology *tree, vector<IntAddress> &aggNodes, double costThreshold)
{

    std::unordered_map<IntAddress, vector<IntAddress>> equal_cost_aggs;
    if (costThreshold < 0) // * save the time
        return equal_cost_aggs;
    std::vector<IntAddress> children;
    IntAddress parent;
    for (auto agg : aggNodes) {
        double local_cost = 0.0;
        // ! find nodes around the aggnodes
        auto agg_node = tree->getNodeFor(getMod(agg));
        for (auto i = 0; i < agg_node->getNumInLinks(); i++) {
            auto inedge = agg_node->getLinkIn(i);
            local_cost += inedge->getWeight();
            auto u = inedge->getRemoteNode();
            while (u->getNumInLinks() == 1) {
                inedge = u->getLinkIn(0);
                local_cost += inedge->getWeight();
                u = inedge->getRemoteNode();
            }
            children.push_back(getAddr(u));
        }
        ASSERT(agg_node->getNumOutLinks() == 1); // rember that root cannot be a aggregation node
        auto outedge = agg_node->getLinkOut(0);
        local_cost += outedge->getWeight();
        auto u = outedge->getRemoteNode();
        while(u->getNumOutLinks() && u->getNumInLinks() == 1) {
            outedge = u->getLinkOut(0);
            local_cost += outedge->getWeight();
            u = outedge->getRemoteNode();
        }
        parent = getAddr(u);

        std::unordered_set<int> excludes;
        std::for_each(hostIds.cbegin(), hostIds.cend(), [&excludes](int nodeId){excludes.insert(nodeId);});
        std::for_each(children.cbegin(), children.cend(), [this, &excludes](IntAddress n){excludes.insert(getNodeId(n));});
        excludes.insert(getNodeId(parent));
        excludes.insert(getNodeId(agg));
        auto N = topo->getNumNodes();
        double dist[N][N];
        for (auto i = 0; i < N; i++) {
            for (auto j = 0; j < N; j++)
                dist[i][j] = INFINITY;
        }
        topo->calculateWeightedSingleShortestPathsTo(getNode(parent));
        for (auto i = 0; i < N; i++) {
            dist[i][getNodeId(parent)] = topo->getNode(i)->getDistanceToTarget();
        }
//        topo->calculateWeightedSingleShortestPathsTo(getNode(agg));
//        for (auto i = 0; i < N; i++) {
//            dist[i][getNodeId(parent)] = topo->getNode(i)->getDistanceToTarget();
//        }
        for(auto i = 0; i < N; i++) {
            if (excludes.find(i) == excludes.end()) {
                topo->calculateWeightedSingleShortestPathsTo(topo->getNode(i));
                auto tmp_cost = 0.0;
                for (const auto& c:children) {
                    tmp_cost += getNode(c)->getDistanceToTarget();
                }
                tmp_cost += dist[i][getNodeId(parent)];
                if (tmp_cost - local_cost <= costThreshold) // TODO: how to decide the threshold, at least it should not be more than the spt!
                    equal_cost_aggs[agg].push_back(getAddr(i));
            }
        }
    }
    return equal_cost_aggs;
}

void GlobalGroupManager::placeJobs(const char *policyName)
{
    if (strcmp(policyName, "manual") == 0) {
        readHostConfig(par("groupHostFile").stringValue());
    }
    else if (strcmp(policyName, "random") == 0) {
        int numUsedHosts = hostIds.size();
        auto worker_left_hosts = hostIds;
        std::transform(worker_left_hosts.cbegin(), worker_left_hosts.cend(),
                        worker_left_hosts.begin(),
                        [this](int id){ return getAddr(id);}); // convert host index to address
        auto ps_left_hosts = worker_left_hosts;
        auto seed1 = intrand(3245);
        auto seed2 = intrand(4148);
        auto shuffle = [](decltype(worker_left_hosts) &vec, decltype(seed1) seed) {
            std::shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
        };
        shuffle(worker_left_hosts, seed1);
        shuffle(ps_left_hosts, seed2);
        // std::cout << "workers can use: " << worker_left_hosts << endl;
        // std::cout << "ps can use: " << ps_left_hosts << endl;
        // std::vector<int> hostIds(hostscopy.begin(), hostscopy.begin()+numUsedHosts);

        int numGroups = par("numGroups").intValue();
        int numWorkers = par("numWorkers").intValue();
        int numPSes = par("numPSes").intValue();

        if (numGroups*numWorkers > numUsedHosts || numGroups*numPSes > numUsedHosts) {
            throw cRuntimeError("used too many hosts, only %d can use.", numUsedHosts);
        }

        for (auto i = 0; i < numGroups; i++) {
            std::vector<IntAddress> workers(worker_left_hosts.begin(), worker_left_hosts.begin() + numWorkers);
            // std::cout << workers << endl;
            worker_left_hosts.erase(worker_left_hosts.begin(), worker_left_hosts.begin() + numWorkers);
            shuffle(worker_left_hosts, seed1);
            // std::cout << worker_left_hosts << endl;
            // ! s1 and s2 must be sorted
            auto s1 = std::set<IntAddress>(ps_left_hosts.begin(), ps_left_hosts.end());
            auto s2 = std::set<IntAddress>(workers.begin(), workers.end());
            decltype(s1) res;
            std::set_difference(s1.begin(), s1.end(),
                                s2.begin(), s2.end(), std::inserter(res, res.end()));

            decltype(ps_left_hosts) tmp(res.begin(), res.end());
            // std::cout << "PS can use: " << tmp << endl;
            std::vector<IntAddress> pses {tmp.back()}; // just pick the last element
            tmp.pop_back();
            // std::cout << "PS: "<< pses << endl;
            ps_left_hosts.resize(ps_left_hosts.size() - pses.size());
            std::merge(tmp.begin(), tmp.end(), workers.begin(), workers.end(), ps_left_hosts.begin());
            shuffle(ps_left_hosts, seed2);
            insertJobInfodb(workers, pses);
            createJobApps(getCurrentJobId());
        }
        // TODO multiple parameter servers
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
        app->par("workers") = vectorToString(job->workers);
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
    else if (strcmp(policyName, "sptree") == 0) // TODO my own algorithms
    {
        for (auto it : jobInfodb) {
            auto jobid = it.first;
            EV_TRACE << "job " << jobid << endl;
            auto group = it.second;
            auto senders = group->workers;
            auto ps = group->PSes.at(0);

            // ! get an aggregation tree
            // TODO my own algorithm
            std::vector<IntAddress> aggNodes;
            auto tree = buildSteinerTree(senders, ps, aggNodes); //  TODO multiple PSes
            auto threshold = par("costThreshold").doubleValue();
            auto equal_cost_aggnodes = findEqualCostAggNodes(tree, aggNodes, threshold);
            EV_DEBUG << "equal_cost_agg_nodes: " << equal_cost_aggnodes << endl;
            // ! update graph edge's cost
            // TODO: how to decide the added cost
            addCost(tree, 0.1, 0.1); // TODO: what about the equal cost aggnodes and their paths?

            std::vector<cModule*> senderMods;
            for (auto& s:senders) {
                senderMods.push_back(getMod(s));
            }

            // * prepare segments
            for (auto i = 0; i < senders.size(); i++) {
                auto addr = senders[i];
                auto m = senderMods[i];
                auto path = getShortestPath(tree, tree->getNodeFor(m), tree->getNodeFor(getMod(ps)));
                ASSERT(path.size() >= 3);
                EV_DEBUG << path.front() << "->" << path.back() << ":" << path << endl;
                auto intermediates = std::vector<IntAddress>(path.begin()+1, path.end()-1); // exclude the paths ends
                // * prepare args(indegree) at each segment
                std::vector<int> indegrees;
                vector<vector<IntAddress>> segment_addrs;
                for (auto& node:intermediates) {
                    auto indegree = tree->getNodeFor(getMod(node))->getNumInLinks();
                    if (indegree >= 2) {
                        indegrees.push_back(indegree);
                        std::vector<IntAddress> tmp{node};
                        if (equal_cost_aggnodes.find(node) != equal_cost_aggnodes.end()) {
                            tmp.insert(tmp.end(), equal_cost_aggnodes[node].begin(), equal_cost_aggnodes[node].end());
                        }
                        segment_addrs.push_back(tmp);
                    }
                }

                EV_TRACE << indegrees << endl;
                segmentInfodb[jobid][addr][ps] = new JobSegmentsRoute();
                segmentInfodb[jobid][addr][ps]->segmentAddrs = segment_addrs;
                segmentInfodb[jobid][addr][ps]->fanIndegrees = indegrees;
                for (auto i = 0; i < m->getSubmoduleVectorSize("workers"); i++) {
                    auto app = m->getSubmodule("workers", i);
                    if (app->hasPar("segmentAddrs") && ps == app->par("destAddress").intValue()) {
                        app->par("segmentAddrs") = vectorToString(segment_addrs);
                        app->par("fanIndegrees") = vectorToString(indegrees);
                    }
                }
            }
            delete tree;
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
            std::vector<cTopology::Node*> senderNodes;
            for (auto& s:senders) {
                senderNodes.push_back(getNode(s));
            }
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

void GlobalGroupManager::insertJobInfodb(const std::vector<IntAddress>& workers, const std::vector<IntAddress>& pses)
{
    std::vector<PortNumber> workerPorts(workers.size(), INVALID_PORT);

    std::vector<PortNumber> PSPorts(pses.size(), INVALID_PORT);
    std::vector<IntAddress> multicastAddrs;
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
