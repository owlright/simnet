#include <omnetpp.h>
#include "Controller.h"
#include "..\common\Print.h"
Define_Module(Controller);

int Controller::getRoute(cModule* from, int to)
{
    cTopology::Node *fromNode = topo->getNodeFor(from);


    for (int i = 0; i < topo->getNumNodes(); i++) {
        if (topo->getNode(i) == fromNode)
            continue;  // skip ourselves
        int address = topo->getNode(i)->getModule()->par("address");
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

int Controller::getGroupAggrNum(int groupid, int routerid)
{
    EV << COLOR(bgB::green) << "Query from router " << routerid << " for group " << groupid << END;
    if (aggrNumberOnRouter.find(groupid)!=aggrNumberOnRouter.end()) {
        auto numOnRouter = aggrNumberOnRouter[groupid];
        if (numOnRouter.find(routerid)!=numOnRouter.end()) {
            return numOnRouter[routerid];
        }
        return -1; // groupid exists but not on routerid
    }
    return -1; // groupid not exists
}

Controller::Controller()
{
  // TODO Auto-generated constructor stub
}

Controller::~Controller() {
    // TODO Auto-generated destructor stub
    delete topo;
}

void Controller::initialize(int stage)
{
    if (stage == 0) {
        EV_INFO << "network intialization." << endl;
        if (getParentModule()->getSubmoduleVectorSize("terminal") <= 0) {
            throw cRuntimeError("The network has no nodes");
        }
        this->topo = new cTopology("topo");
        std::vector<std::string> nedTypes;
        nedTypes.push_back(getParentModule()->getSubmodule("terminal", 0)->getNedTypeName());//todo nodename should not be fixed
        topo->extractByNedTypeName(nedTypes);
        EV << "cTopology found " << topo->getNumNodes() << " nodes\n";
        // ! parse aggr group info
        auto groupAggRouter = check_and_cast<cValueArray*>(par("aggrouter").objectValue());
        auto numberOfGroup = groupAggRouter->size();
        EV << "There are " << numberOfGroup << " groups." << endl;
        auto groupAggrNumber = check_and_cast<cValueArray*>(par("aggrnumber").objectValue());
        auto targets = check_and_cast<cValueArray*>(par("targets").objectValue())->asIntVector();

        if (groupAggrNumber->size()!=numberOfGroup||targets.size()!=numberOfGroup) {
            throw cRuntimeError("number of aggr group is not equal.");
        }

        for (int i = 0; i < numberOfGroup; i++) {
            auto aggrRouter = check_and_cast<cValueArray*>( (groupAggRouter->get(i)).objectValue() )->asIntVector(); //todo this is too tedious!
            auto aggrNumber = check_and_cast<cValueArray*>( (groupAggrNumber->get(i)).objectValue() )->asIntVector();
            auto root = targets[i];
            aggrNumberOnRouter[root].insert(std::make_pair(aggrRouter[i], aggrNumber[i]));
            EV << "group "<< root << " aggregate on "<< aggrRouter << endl;
        }

    }
}
