#include "GlobalGroupManager.h"
#include "simnet/graph/algorithms.h"
#include <algorithm> // std::shuffle
#include <fstream>
#include <iostream>
#include <numeric> // std::iota
#include <queue>
#include <random> // std::default_random_engine
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

            // ! get an aggregation tree
            // TODO my own algorithm
            std::vector<IntAddress> aggNodes;
            std::unordered_map<IntAddress, vector<IntAddress>> equal_cost_aggnodes;
            vector<int> sources;
            for (auto s : senders) {
                sources.push_back(getNodeId(s));
            }
            auto root = getNodeId(ps);

            auto tree = takashami_tree(network, sources, root); //  TODO multiple PSes
            // network.draw("network");
            // tree.draw("sptree", "dot");
            vector<int> branch_nodes;
            auto branch_tree = extract_branch_tree(tree, sources, root, &branch_nodes);
            for (auto& b : branch_nodes) {
                aggNodes.push_back(getAddr(b));
            }
            // branch_tree.draw("brachtree", "dot");
            auto threshold = par("costThreshold").doubleValue();

            std::unordered_set<int> forbiddens(getHostIds().begin(), getHostIds().end());
            forbiddens.insert(branch_nodes.begin(), branch_nodes.end());
            for (auto n : branch_nodes) {
                auto temp = find_equal_nodes(network, branch_tree, n, forbiddens, threshold);
                if (!temp.empty()) {
                    vector<IntAddress> equalAddrs;
                    for (auto& t : temp) {
                        equalAddrs.push_back(getAddr(t));
                    }
                    equal_cost_aggnodes[getAddr(n)] = equalAddrs;
                }
            }

            EV_DEBUG << "agg_nodes: " << aggNodes << endl;
            EV_DEBUG << "equal_cost_agg_nodes: " << equal_cost_aggnodes << endl;
            // ! update graph edge's cost
            // TODO: how to decide the added cost
            // addCost(tree, 0.1, 0.1); // TODO: what about the equal cost aggnodes and their paths?

            // * prepare segments
            for (auto i = 0; i < sources.size(); i++) {
                auto addr = senders[i];
                vector<int> path;
                dijistra(tree, sources[i], root, &path);
                ASSERT(path.size() >= 3);
                vector<int> intermediates(path.begin() + 1, path.end() - 1); // exclude the paths ends
                // * prepare args(indegree) at each segment
                std::vector<int> indegrees;
                vector<vector<IntAddress>> segment_addrs;
                for (auto& node : intermediates) {
                    if (branch_tree.has_node(node)) {
                        auto nodeAddr = getAddr(node);
                        auto indegree = branch_tree.indegree(node);
                        ASSERT(indegree >= 2);
                        indegrees.push_back(indegree); // find the aggregation nodes
                        vector<IntAddress> tmp { nodeAddr };
                        if (equal_cost_aggnodes.find(node) != equal_cost_aggnodes.end()) {
                            tmp.insert(tmp.end(), equal_cost_aggnodes[node].begin(), equal_cost_aggnodes[node].end());
                        }
                        segment_addrs.push_back(tmp);
                    }
                }
                ASSERT(segment_addrs.size() == indegrees.size());
                EV_TRACE << addr << endl;
                for (auto i = 0; i < segment_addrs.size(); i++) {
                    EV_TRACE << segment_addrs[i] << " " << indegrees[i] << endl;
                }

                segmentInfodb[jobid][addr][ps] = make_shared<JobSegmentsRoute>();
                segmentInfodb[jobid][addr][ps]->segmentAddrs = segment_addrs;
                segmentInfodb[jobid][addr][ps]->fanIndegrees = indegrees;
                auto mod = getMod(senders[i]);
                for (auto i = 0; i < mod->getSubmoduleVectorSize("workers"); i++) {
                    auto app = mod->getSubmodule("workers", i);
                    if (app->hasPar("segmentAddrs") && ps == app->par("destAddress").intValue()) {
                        app->par("segmentAddrs") = vectorToString(segment_addrs);
                        app->par("fanIndegrees") = vectorToString(indegrees);
                    }
                }
            }
        }
    } else if (strcmp(policyName, "edge") == 0) {
        for (auto it : jobInfodb) {
            EV_DEBUG << "job " << it.first << endl;
            auto group = it.second;
            auto senders = group->workers;
            auto ps = group->PSes.at(0);
            auto psNode = getNode(ps);
            // EV_TRACE << senders << " " << ps << endl;
            std::vector<cTopology::Node*> senderNodes;
            for (auto& s : senders) {
                senderNodes.push_back(getNode(s));
            }
            // ! if only do aggregation at edge, the indegree calculation is a litte harder
            std::unordered_map<IntAddress, int> indegrees;
            std::unordered_map<IntAddress, std::vector<IntAddress>> addrSegments;
            for (auto& s : senders) {
                auto srcNode = getNode(s);
                auto edge0Switch = srcNode->getLinkOut(0)->getRemoteNode();
                auto edge1Switch = psNode->getLinkIn(0)->getRemoteNode();
                // * prepare segments
                std::vector<IntAddress> segments;
                segments.push_back(getAddr(edge0Switch->getModule()));
                if (edge1Switch != edge0Switch)
                    segments.push_back(getAddr(edge1Switch->getModule()));
                addrSegments[s] = segments;
                EV_TRACE << segments << endl;
                // std::cout << segments << endl;
                for (int i = segments.size() - 1; i >= 0; i--) {
                    auto seg = segments[i];
                    if (indegrees.find(seg) == indegrees.end()) {
                        indegrees[seg] = 1;
                    } else {
                        if ((i == 0) || (i == 1 && indegrees.find(segments[i - 1]) == indegrees.end())) {
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
            for (auto& s : senders) {
                auto srcMod = getMod(s);
                auto segments = addrSegments[s];
                for (auto i = 0; i < srcMod->getSubmoduleVectorSize("workers"); i++) {
                    auto app = srcMod->getSubmodule("workers", i);
                    if (app->hasPar("segmentAddrs") && ps == app->par("destAddress").intValue()) {
                        app->par("segmentAddrs") = vectorToString(segments);
                        std::vector<int> tmp;
                        for (auto s : segments) {
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
