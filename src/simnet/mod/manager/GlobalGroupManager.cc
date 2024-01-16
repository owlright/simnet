#include "GlobalGroupManager.h"
#include "simnet/graph/algorithms.h"
#include <algorithm> // std::shuffle
#include <fstream>
#include <iostream>
#include <numeric> // std::iota
#include <queue>
#include <random> // std::default_random_engine
#include <tuple>
std::ostream& operator<<(std::ostream& os, const std::vector<cTopology::Node*>& array)
{
    os << "[";
    for (auto& elem : array) {
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
    } else if (stage == INITSTAGE_ASSIGN) {
        network = globalView->getNetworkCopy();
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

void GlobalGroupManager::readHostConfig(const char* fileName)
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

void GlobalGroupManager::placeJobs(const char* policyName)
{
    if (strcmp(policyName, "manual") == 0) {
        readHostConfig(par("groupHostFile").stringValue());
    } else if (strcmp(policyName, "random") == 0) {
        int numGroups = par("numGroups").intValue();
        int numWorkers = par("numWorkers").intValue();
        int numPSes = par("numPSes").intValue();
        int numHosts = getHostIds().size();
        std::vector<int> left_hosts = getHostIds();
        std::transform(left_hosts.cbegin(), left_hosts.cend(), left_hosts.begin(),
            [this](int id) { return getAddr(id); }); // * convert host index to address
        auto seed = intrand(3245);
        auto shuffle = [](decltype(left_hosts)& vec, decltype(seed) seed) {
            std::shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
        };
        if (numGroups * (numWorkers + numPSes) > numHosts) {
            throw cRuntimeError("used too many hosts, only %d can use.", numHosts);
        }
        for (auto i = 0; i < numGroups; i++) {
            shuffle(left_hosts, seed);
            std::vector<IntAddress> workers(left_hosts.begin(), left_hosts.begin() + numWorkers + 1);
            auto ps = std::vector<IntAddress> { workers.back() };
            workers.pop_back();
            left_hosts.erase(left_hosts.begin(), left_hosts.begin() + numWorkers + 1);
            std::sort(workers.begin(), workers.end()); // easy debug
            insertJobInfodb(workers, ps);
            createJobApps(getCurrentJobId());
        }
        unicastHosts.insert(unicastHosts.begin(), left_hosts.begin(), left_hosts.end());
        std::sort(unicastHosts.begin(), unicastHosts.end()); // easy debug
        /* -------- one job's PS can overlap other's worker, too complicated -------- */
        // auto ps_left_hosts = worker_left_hosts;
        // auto seed1 = intrand(3245);
        // auto seed2 = intrand(4148);
        // auto shuffle = [](decltype(worker_left_hosts) &vec, decltype(seed1) seed) {
        //     std::shuffle(vec.begin(), vec.end(), std::default_random_engine(seed));
        // };

        // if (numGroups*numWorkers > numUsedHosts || numGroups*numPSes > numUsedHosts) {
        //     throw cRuntimeError("used too many hosts, only %d can use.", numUsedHosts);
        // }

        // for (auto i = 0; i < numGroups; i++) {
        //     shuffle(worker_left_hosts, seed1);
        //     std::vector<IntAddress> workers(worker_left_hosts.begin(), worker_left_hosts.begin() + numWorkers);
        //     // std::cout << workers << endl;
        //     worker_left_hosts.erase(worker_left_hosts.begin(), worker_left_hosts.begin() + numWorkers);
        //     // std::cout << worker_left_hosts << endl;
        //     // ! s1 and s2 must be sorted, so must use std::set here
        //     auto s1 = std::set<IntAddress>(ps_left_hosts.begin(), ps_left_hosts.end());
        //     auto s2 = std::set<IntAddress>(workers.begin(), workers.end());
        //     decltype(s1) res;
        //     std::set_difference(s1.begin(), s1.end(),
        //                         s2.begin(), s2.end(), std::inserter(res, res.end()));

        //     decltype(ps_left_hosts) tmp(res.begin(), res.end());
        //     // std::cout << "PS can use: " << tmp << endl;
        //     shuffle(tmp, seed2); // ! tmp is sorted, must shuffle here
        //     std::vector<IntAddress> pses {tmp.back()}; // just pick the last element
        //     tmp.pop_back();
        //     // std::cout << "PS: "<< pses << endl;
        //     ps_left_hosts.resize(ps_left_hosts.size() - pses.size());
        //     std::merge(tmp.begin(), tmp.end(), workers.begin(), workers.end(), ps_left_hosts.begin());
        //     shuffle(ps_left_hosts, seed2);
        //     std::sort(workers.begin(), workers.end());
        //     insertJobInfodb(workers, pses);
        //     createJobApps(getCurrentJobId());
        // }
        /* -------------------------------------------------------------------------- */
        // TODO multiple parameter servers
    } else if (strcmp(policyName, "") == 0) {
        EV_WARN << "You may forget to set a placement policy for agg groups, make sure you manually set them." << endl;
    } else {
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
        auto nodeAddress = job->PSes[i];
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
        at->gate("localOut", at->gateSize("localIn") - 1)->connectTo(inGate);
        outGate->connectTo(at->gate("localIn", at->gateSize("localOut") - 1));
        app->callInitialize(INITSTAGE_LOCAL);
    }

    for (auto i = 0; i < nWorkers; i++) {
        auto nodeAddress = job->workers[i];
        auto node = getMod(nodeAddress);
        // * find an idle worker
        auto appExistSize = node->getSubmoduleVectorSize("workers");

        node->setSubmoduleVectorSize("workers", appExistSize + 1);
        auto appType = useInc ? "simnet.app.INCWorker" : "simnet.app.WorkerApp";
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
        auto inGate = app->gate("in");
        auto outGate = app->gate("out");
        auto at = node->getSubmodule("at");
        at->setGateSize("localIn", at->gateSize("localIn") + 1);
        at->setGateSize("localOut", at->gateSize("localOut") + 1);
        at->gate("localOut", at->gateSize("localIn") - 1)->connectTo(inGate);
        outGate->connectTo(at->gate("localIn", at->gateSize("localOut") - 1));
        app->callInitialize(INITSTAGE_LOCAL);
    }
}

void GlobalGroupManager::calcAggTree(const char* policyName)
{
    using namespace simnet::algorithms;
    using simnet::Graph;
    auto runId = getEnvir()->getConfigEx()->getActiveRunNumber();
    if (strcmp(policyName, "manual") == 0) {
        // readSwitchConfig(par("groupSwitchFile").stringValue());
        // TODO manually set segments for each host
    } else if (strcmp(policyName, "sptree") == 0) // TODO my own algorithms
    {
        for (auto it : jobInfodb) {
            auto jobid = it.first;
            EV_TRACE << "job " << jobid << endl;
            auto group = it.second;
            auto senders = group->workers;
            auto ps = group->PSes.at(0);

            vector<int> sources;
            for (auto s : senders) {
                sources.push_back(getNodeId(s));
            }
            auto root = getNodeId(ps);

            auto threshold = par("costThreshold").doubleValue();
            bool allowedKTree = threshold > 0;
            int numNCTrees = par("numECTrees").intValue();
            bool removeEqualBranchNodes = par("removeEqualBranchNodes").intValue() > 0;
            std::unordered_set<int> forbidden_hosts(getHostIds().begin(), getHostIds().end());

            // ! update graph edge's cost
            // TODO: how to decide the added cost
            // addCost(tree, 0.1, 0.1); // TODO: what about the equal cost aggnodes and their paths?
            decltype(aggTrees)::value_type jobTrees;
            decltype(aggNodes)::value_type jobEqualNodes, tmpJobEqualNodes;
            if (allowedKTree) {
                // ! I don't know which trees are better, so all of them should be calculated at first
                auto kTrees = takashami_trees(network, sources, root, forbidden_hosts, &tmpJobEqualNodes, removeEqualBranchNodes);
                vector<std::tuple<double, int, simnet::Graph>> sortedKTrees;
                for (int i = 0; i < kTrees.size(); i++) {
                    sortedKTrees.push_back({ kTrees[i].get_cost(), i, kTrees[i] });
                }
                std::sort(sortedKTrees.begin(), sortedKTrees.end(),
                    [](const auto& a, const auto& b) { return std::get<0>(a) < std::get<0>(b); });
                auto& [minCost, index, firstTree] = sortedKTrees[0];
                // firstTree.draw("firstTree.png");
                jobTrees.push_back(firstTree);
                jobEqualNodes.push_back(tmpJobEqualNodes[index]);
                numNCTrees = std::min(numNCTrees, int(sortedKTrees.size()));
                for (auto i = 1; i < numNCTrees; i++) {
                    auto& [currCost, index, t] = sortedKTrees[i];
                    // t.draw((std::string("tree")+std::to_string(i)).c_str());
                    // ASSERT(t.is_tree());
                    // ! 允许等价树超过一些代价，但也不能超过太多，否则会出现故意绕远路的现象，2是目前的经验值
                    if (t.get_cost() - minCost <= 2) {
                        for (auto s : sources) {
                            ASSERT(t.has_node(s));
                        }
                        jobTrees.push_back(t);
                        jobEqualNodes.push_back(tmpJobEqualNodes[index]);
                    } else {
                        break;
                    }
                }
                ASSERT(jobTrees.size() == jobEqualNodes.size());
            }
            else {
                auto tree = takashami_tree(network, sources, root);
                jobTrees.push_back(tree);
                vector<int> branch_nodes;
                auto branch_tree = extract_branch_tree(tree, sources, root, &branch_nodes);
                map<int, vector<int>> equalNodes;
                for (auto b: branch_nodes) {
                    equalNodes[b] = find_equal_nodes(network, tree, b);
                }
                jobEqualNodes.push_back(equalNodes);
            }

            std::cout << "runID " << runId << " job " << jobid << " " << jobTrees.size() << " trees prepared."
                      << std::endl;
            aggTrees.push_back(jobTrees);
            aggNodes.push_back(jobEqualNodes);
        }
    } else if (strcmp(policyName, "edge") == 0) {
        for (auto it : jobInfodb) {
            auto jobid = it.first;
            EV_TRACE << "job " << jobid << endl;
            auto group = it.second;
            auto senders = group->workers;
            auto ps = group->PSes.at(0);

            vector<int> sources;
            for (auto s : senders) {
                sources.push_back(getNodeId(s));
            }
            auto root = getNodeId(ps);
            ASSERT(network.outdegree(root) == 1);
            auto root_switch = network.in_neighbors(root)[0].first;
            Graph t;
            t.add_edge(root_switch, root);
            map<int, vector<int>> indegree_;
            vector<int> edge_switches;
            for (auto& s : sources) {
                ASSERT(network.outdegree(s) == 1);
                for (auto& [v, w] : network.out_neighbors(s)) {
                    t.add_edge(s, v);
                    if (indegree_.find(v) == indegree_.end()) {
                        indegree_[v] = { 1 };
                    } else {
                        indegree_[v][0] += 1;
                    }
                    if (v != root_switch)
                        t.add_edge(v, root_switch);
                }
            }
            decltype(indegree_) equalNodes;
            for (auto& [v, i] : indegree_) {
                if (i[0] > 1)
                    equalNodes[v] = {};
            }

            aggTrees.push_back({ t });
            aggNodes.push_back({ equalNodes });
        }
    } else if (strcmp(policyName, "") == 0) {
        EV_WARN << "You may forget to set the aggTreeType. No AggTree will be built!" << endl;
    } else {
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
    auto entry = make_shared<JobHostInfo>();
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
