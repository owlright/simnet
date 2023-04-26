#include "Controller.h"
#include <omnetpp.h>
#include "../common/Print.h"
#include <unordered_set>

Define_Module(Controller);

std::vector<int> Controller::getRoutes(cModule* from, int to) const // TODO just pass the src address is ok
{
    Enter_Method_Silent();
    cTopology::Node *fromNode = topo->getNodeFor(from);

    auto destNode = topo->getNode(addr2node.at(to));
    // ! HACK find multiple next hops used for ecmp
    topo->calculateUnweightedSingleShortestPathsTo(destNode);
    auto distance = fromNode->getDistanceToTarget();
    std::vector<int> gateIndexes;
    std::vector<cTopology::LinkOut *> linkrecord;
    // HACK: find ecmp paths
    while (fromNode->getNumPaths() > 0) {
        if (fromNode->getDistanceToTarget() <= distance) {
            auto outLink = fromNode->getPath(0);

//            fromNode->getPath(0)->disable();
            int gateIndex = outLink->getLocalGate()->getIndex();
//            outLink->disable();
            gateIndexes.push_back(gateIndex);
            EV_DETAIL << "router: " << fromNode->getModule()->par("address").intValue()
                    <<"  towards address " << to
                    << " gateIndex is " << gateIndex << endl;
        }
        // ! if we get a longer path just disable it and try next one
        fromNode->getPath(0)->disable(); // * disable this outLink and recalc the shortetpath algorithm
        linkrecord.push_back(fromNode->getPath(0));
        topo->calculateUnweightedSingleShortestPathsTo(destNode);
    }
    // ! remember to reset the links
    for (auto link:linkrecord) {
        link->enable();
    }
    return gateIndexes;
}

int Controller::getGroupInfo(int groupid, int routerid, const aggrGroupOnRouterTable& table) const
{
    EV << COLOR(bgB::green) << "Router " << routerid << " for group " << groupid << END;
    if (table.find(groupid)!=table.end()) {
        auto infoOnRouter = table.at(groupid);
        if (infoOnRouter.find(routerid)!=infoOnRouter.end()) {
            return infoOnRouter.at(routerid);
        }
        return -1; // groupid exists but not on routerid
    }
    return -1; // groupid exists but not on routerid
}

int Controller::getGroupAggrNum(int groupid, int routerid) const
{
    return getGroupInfo(groupid, routerid, aggrNumberOnRouter);
}

int Controller::getGroupAggrBuffer(int groupid, int routerid) const
{
    return getGroupInfo(groupid, routerid, aggrBufferOnRouter);
}

int Controller::getAggrSendersNum(int groupid) const
{
    return aggrgroup.at(groupid).size();
}

void Controller::updateAggrGroup(int groupid, int senderAddr)
{
    EV << COLOR(bgB::green) << "Register " << groupid << " by node " << senderAddr << END;
    aggrgroup[groupid].push_back(senderAddr);
}

bool Controller::isAggrGroupOnRouter(int groupid, int routerid) const
{
    if (aggrBufferOnRouter.find(groupid)==aggrBufferOnRouter.end()) return false;
    if (aggrBufferOnRouter.at(groupid).find(routerid)==aggrBufferOnRouter.at(groupid).end()) return false;
    return true;
}

bool Controller::isGroupTarget(int myAddress) const
{
    return aggrgroup.find(myAddress) != aggrgroup.end();
}

Controller::Controller()
{
  // TODO Auto-generated constructor stub
}

Controller::~Controller() {
    // TODO Auto-generated destructor stub
    delete topo;
}

void Controller::setNodes(const cTopology *topo)
{
    node2addr.reserve(topo->getNumNodes());
    for (int i = 0; i < topo->getNumNodes(); i++) {
        auto node = topo->getNode(i)->getModule();
        int address = node->par("address");
        auto isHost = node->getProperties()->get("host") != nullptr;
        if (isHost) {
            hosts.push_back(i);
        }
        node2addr.push_back(address);
        addr2node[address] = i;
        EV_DETAIL << "node: " << i << " address: " << address << endl;
    }
}

int Controller::askForDest(int srcAddr) const {
    return odMap.at(srcAddr);
}

void Controller::prepareTrafficPattern(const std::string& name) {
    if (name=="random") {
        for (auto h:hosts) {
            int src = node2addr[h];
            int destAddr;
            do {
                auto destNode = hosts.at(intrand(hosts.size()));
                destAddr = node2addr.at(destNode);
            } while (destAddr == src); // avoid send packet to itself
            odMap[src] = destAddr;
        }
    }

}

int Controller::askForGroup(int srcAddr) const {
    // TODO store aggrgroup infomation in a struct, now for compatibility with App
    for (auto& entry: aggrgroup) {
        if (entry.first==srcAddr)
            return srcAddr;
        for (auto sender: entry.second) {
            if (sender == srcAddr) {
                return entry.first;
            }
        }
    }
    return -1;
}

void Controller::prepareAggrGroup(const std::string& name) {
    if (name=="random") {
        int groups = 2;
        int groupmembers = 10;
        std::vector<std::vector<int>> aggrgroups;
        std::unordered_set<int> visited;
        for (auto i = 0; i < groups; i++) {
            std::vector<int> g;
            for (auto j = 0; j < groupmembers ; j++) {
                auto node = hosts.size(); // which is not possible
                do {
                    node = intrand(hosts.size());
                } while (visited.find(node) != visited.end());
                g.push_back(node);
                visited.insert(node);
            }
            aggrgroups.emplace_back(g);
        }
        //get steiner tree for each group
        cTopology *tree = new cTopology("steiner");
        for (auto& g:aggrgroups) {
            auto root = g.back();
            g.pop_back();
            auto rootNode = topo->getNode(root);
            auto rootAddress = rootNode->getModule()->par("address").intValue();

            // std::cout << "root: "<< rootNode->getModule()->par("address") << endl;
            tree->addNode(new cTopology::Node(rootNode->getModuleId())); // ! must duplicate the node
            for (auto& n:g) {
                auto currentHost = topo->getNode(n);
                // regester this aggrgoup's senders
                aggrgroup[rootAddress].push_back(currentHost->getModule()->par("address").intValue());
                double dist = INFINITY;
                cTopology::Node* destNode = nullptr;
                // * find the closest node in the tree
                for (auto i = 0; i < tree->getNumNodes(); i++) {
                    auto nodeInTree = topo->getNodeFor(tree->getNode(i)->getModule()); // ! the node must in topo
                    if (nodeInTree == rootNode ||
                            nodeInTree->getModule()->getProperties()->get("switch")!=nullptr) { // ! remember ignore the hosts
                        topo->calculateUnweightedSingleShortestPathsTo(nodeInTree);
                        if (currentHost->getDistanceToTarget() < dist) {
                            dist = currentHost->getDistanceToTarget();
                            destNode = nodeInTree;
                        }
                    }
                }
                ASSERT(destNode);
                // * add the node into tree using the shortest path
                topo->calculateUnweightedSingleShortestPathsTo(destNode);
                auto node = currentHost;
                auto treenode = new cTopology::Node(node->getModuleId());
                tree->addNode(treenode);
                while (node != destNode) {
                    // std::cout << node->getModule()->par("address") << endl;
                    auto nextNode = node->getPath(0)->getRemoteNode();
                    cTopology::Node* nexttreenode = nullptr;
                    if (nextNode!=destNode) {
                        nexttreenode = new cTopology::Node(nextNode->getModuleId());
                        tree->addNode(nexttreenode);
                    }
                    else {
                        nexttreenode = tree->getNodeFor(nextNode->getModule());
                    }
                    tree->addLink(new cTopology::Link(), treenode, nexttreenode);

                    node = nextNode;
                    treenode = nexttreenode;
                }
            }
            // * tree construction is finished
            // * now assign tree to routers

            for (auto i = 0; i < tree->getNumNodes(); i++) {
                auto node = tree->getNode(i);
                auto mod = node->getModule();
                auto indegree = node->getNumInLinks();
                EV_DETAIL << mod->par("address").intValue() << " indegree:" << indegree << endl;
                if (indegree >= 2) {
                    auto routerAddress = mod->par("address").intValue();
                    aggrNumberOnRouter[rootAddress][routerAddress] = indegree;
                    aggrBufferOnRouter[rootAddress][routerAddress] = -1; // -1 means unlimited use
                }
            }

            tree->clear();
        }
        delete tree;
    } else if (name=="manual") {
        // ! parse aggr group info
        auto groupAggSenders = check_and_cast<cValueArray*>(par("aggSenders").objectValue());
        auto groupAggRouters = check_and_cast<cValueArray*>(par("aggRouters").objectValue());
        auto numberOfGroup = groupAggRouters->size();

        auto groupAggFanInDegree = check_and_cast<cValueArray*>(par("aggFanInDegree").objectValue());
        auto groupAggMaxBufferSize = check_and_cast<cValueArray*>(par("aggMaxBufferSize").objectValue());
        // ! only targets is single
        auto groupTargets = check_and_cast<cValueArray*>(par("targets").objectValue())->asIntVector();

        if (groupAggFanInDegree->size()!=numberOfGroup
        ||groupTargets.size()!=numberOfGroup
        ||groupAggMaxBufferSize->size()!=numberOfGroup
        ||groupAggSenders->size()!=numberOfGroup) {
            throw cRuntimeError("number of aggr group is not equal.");
        }

        for (int i = 0; i < numberOfGroup; i++) {
            auto aggSenders = check_and_cast<cValueArray*>( (groupAggSenders->get(i)).objectValue() )->asIntVector();

            auto aggRouter = check_and_cast<cValueArray*>( (groupAggRouters->get(i)).objectValue() )->asIntVector(); //todo this is too tedious!
            auto aggFanInDegree = check_and_cast<cValueArray*>( (groupAggFanInDegree->get(i)).objectValue() )->asIntVector();
            auto aggBuffer = check_and_cast<cValueArray*>( (groupAggMaxBufferSize->get(i)).objectValue() )->asIntVector();
            auto root = groupTargets[i];
            for (auto s:aggSenders) {
                aggrgroup[root].push_back(s);
            }
            EV << "group "<< root << " aggregate on "<< aggRouter << " with number: " << aggFanInDegree << endl;
            auto numberOfRouter = aggRouter.size();
            if (numberOfRouter != aggFanInDegree.size())
                throw cRuntimeError("router number and aggr number not match!");
            for (int j = 0; j < numberOfRouter; j++) {
                aggrNumberOnRouter[root][aggRouter[j]] = aggFanInDegree[j];
                aggrBufferOnRouter[root][aggRouter[j]] = aggBuffer[j];
            }
        }
    }
}

void Controller::initialize(int stage)
{
    if (stage == Stage::INITSTAGE_LOCAL) {
        EV_INFO << "network intialization." << endl;
//        if (getParentModule()->getSubmoduleVectorSize("terminal") <= 0) {
//            throw cRuntimeError("The network has no nodes");
//        }
        this->topo = new cTopology("topo");
//        std::vector<std::string> nedTypes;
//        nedTypes.push_back(getParentModule()->getSubmodule("terminal", 0)->getNedTypeName());//todo nodename should not be fixed
//        topo->extractByNedTypeName(nedTypes);
        topo->extractByProperty("node");
        EV << "cTopology found " << topo->getNumNodes() << " nodes\n";
        setNodes(topo);


    }
    if (stage == Stage::INITSTAGE_LOCAL) {
        if (!par("trafficPattern").stdstringValue().empty()) {
            prepareTrafficPattern(par("trafficPattern").stdstringValue());
        }
        if (!par("aggrGroupPlacePolicy").stdstringValue().empty()) {
            prepareAggrGroup(par("aggrGroupPlacePolicy").stdstringValue());
        }
    }
    if (stage == Stage::INITSTAGE_ASSIGN) {
        EV << "There are " << aggrgroup.size() << " groups." << endl;
        for (const auto& entry: aggrgroup) {
            EV << COLOR(bgB::green) << "group:" << entry.first <<" senders:" << entry.second << END;
        }
    }
}
