#include "Controller.h"
#include <omnetpp.h>
#include "../common/Print.h"
#include <unordered_set>

Define_Module(Controller);

int Controller::getRoute(cModule* from, int to) const // TODO just pass the src address is ok
{
    Enter_Method_Silent();
    cTopology::Node *fromNode = topo->getNodeFor(from);

    for (int i = 0; i < topo->getNumNodes(); i++) {
        if (topo->getNode(i) == fromNode)
            continue;  // skip ourselves
        int address = nodeMap.at(i);
        if (address == to) {
            topo->calculateUnweightedSingleShortestPathsTo(topo->getNode(i));
            if (fromNode->getNumPaths() > 0) {
                int gateIndex = fromNode->getPath(0)->getLocalGate()->getIndex();
                EV << "  towards address " << address << " gateIndex is " << gateIndex << endl;
                return gateIndex;
            }
        }
    }
  return -1;
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
    nodeMap.reserve(topo->getNumNodes());
    for (int i = 0; i < topo->getNumNodes(); i++) {
        auto node = topo->getNode(i)->getModule();
        int address = node->par("address");
        auto isHost = node->getProperties()->get("host") != nullptr;
        if (isHost) {
            hosts.push_back(i);
        }
        nodeMap.push_back(address);
    }
}

int Controller::askForDest(int srcAddr) const {
    return odMap.at(srcAddr);
}

void Controller::prepareTrafficPattern(const std::string& name) {
    if (name=="random") {
        for (auto h:hosts) {
            int src = nodeMap[h];
            int destAddr;
            do {
                auto destNode = hosts.at(intrand(hosts.size()));
                destAddr = nodeMap.at(destNode);
            } while (destAddr == src); // avoid send packet to itself
            odMap[src] = destAddr;
        }
    }

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
            // std::cout << "root: "<< rootNode->getModule()->par("address") << endl;
            tree->addNode(new cTopology::Node(rootNode->getModuleId())); // ! must duplicate the node
            for (auto& n:g) {
                auto currentHost = topo->getNode(n);
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
            // * the tree is finished
            for (auto i = 0; i < tree->getNumNodes(); i++) {
                auto node = tree->getNode(i);
                auto mod = node->getModule();
                std::cout << mod->par("address") << " " << node->getNumInLinks() << endl;
            }
            tree->clear();
            std::cout<<endl;
        }
        delete tree;
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
        // ! parse aggr group info
        auto groupAggRouter = check_and_cast<cValueArray*>(par("aggrouter").objectValue());
        auto numberOfGroup = groupAggRouter->size();
        EV << "There are " << numberOfGroup << " groups." << endl;
        auto groupAggrNumber = check_and_cast<cValueArray*>(par("aggrnumber").objectValue());
        auto groupAggrBuffer = check_and_cast<cValueArray*>(par("aggrbuffer").objectValue());
        auto targets = check_and_cast<cValueArray*>(par("targets").objectValue())->asIntVector();

        if (groupAggrNumber->size()!=numberOfGroup||targets.size()!=numberOfGroup||groupAggrBuffer->size()!=numberOfGroup) {
            throw cRuntimeError("number of aggr group is not equal.");
        }

        for (int i = 0; i < numberOfGroup; i++) {
            auto aggrRouter = check_and_cast<cValueArray*>( (groupAggRouter->get(i)).objectValue() )->asIntVector(); //todo this is too tedious!
            auto aggrNumber = check_and_cast<cValueArray*>( (groupAggrNumber->get(i)).objectValue() )->asIntVector();
            auto aggrBuffer = check_and_cast<cValueArray*>( (groupAggrBuffer->get(i)).objectValue() )->asIntVector();
            auto root = targets[i];
//            aggrNumberOnRouter[root][aggrRouter[i]] = aggrNumber[i];
            EV << "group "<< root << " aggregate on "<< aggrRouter << " with number: " << aggrNumber << endl;
            auto numberOfRouter = aggrRouter.size();
            if (numberOfRouter != aggrNumber.size())
                throw cRuntimeError("router number and aggr number not match!");
            for (int j = 0; j < numberOfRouter; j++) {
                aggrNumberOnRouter[root][aggrRouter[j]] = aggrNumber[j];
                aggrBufferOnRouter[root][aggrRouter[j]] = aggrBuffer[j];
            }
        }

    }
    if (stage == Stage::INITSTAGE_LOCAL) {
        if (!par("trafficPattern").stdstringValue().empty()) {
            prepareTrafficPattern(par("trafficPattern").stdstringValue());
        }
        if (!par("aggrGroupPlacePolicy").stdstringValue().empty()) {
            prepareAggrGroup(par("aggrGroupPlacePolicy").stdstringValue());
        }
    }
    if (stage == Stage::INITSTAGE_CONTROLL) {
        for (const auto& entry: aggrgroup) {
            EV << COLOR(bgB::green) << "group:" << entry.first <<" senders:" << entry.second << END;
        }

    }
}
