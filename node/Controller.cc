#include <omnetpp.h>
#include "Controller.h"
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
    }
}
