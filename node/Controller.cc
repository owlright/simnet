#include <omnetpp.h>
#include "Controller.h"
#include "..\common\Print.h"
Define_Module(Controller);

int Controller::getRoute(cModule* from, int to) const
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
        int address = topo->getNode(i)->getModule()->par("address");
        nodeMap.push_back(address);
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
    if (stage == Stage::INITSTAGE_CONTROLL) {
        for (const auto& entry: aggrgroup) {
            EV << COLOR(bgB::green) << "group:" << entry.first <<" senders:" << entry.second << END;
        }

    }
}
