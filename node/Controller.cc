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
        nedTypes.push_back(getParentModule()->getSubmodule("terminal", 0)->getNedTypeName());
        topo->extractByNedTypeName(nedTypes);
        EV << "cTopology found " << topo->getNumNodes() << " nodes\n";
        // ! parse aggr group info
        auto aggrgroup = check_and_cast<cValueArray*>(par("aggrgroup").objectValue());
        auto targets = check_and_cast<cValueArray*>(par("targets").objectValue())->asIntVector();
        if (aggrgroup->size() != targets.size()) {
            throw cRuntimeError("sender group number is not equal to targets");
        }
        for (int i = 0; i < aggrgroup->size(); i++) {
            auto senders = check_and_cast<cValueArray*> (aggrgroup->get(i).objectValue())->asIntVector();
            aggrSendersTable[i] = std::move(senders); // ? if this right?
            EV << "Group "<< i << " Senders:" << aggrSendersTable[i] << " Target:"<<targets.at(i) << endl;
        }

    }
}
