#include "GlobalView.h"

Define_Module(GlobalView);

void GlobalView::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        EV << "network initialization." << endl;
        topo = new cTopology("topo");
        topo->extractByProperty("node");
        int N = topo->getNumNodes();
        topoDist.resize(N, vector<double>(N));
        for (int i = 0; i < topo->getNumNodes(); i++) {
            topo->calculateWeightedSingleShortestPathsTo(topo->getNode(i));
            for (int j = 0; j < topo->getNumNodes(); j++) {
                topoDist[i][j] = topo->getNode(j)->getDistanceToTarget();
                // std::cout << i << " "
                //           << j << " "
                //           << topoDist[i][j] << endl;
                // std::cout << topo->getNode(i)->getModule()->getClassAndFullPath() << " "
                //           << topo->getNode(j)->getModule()->getClassAndFullPath() << " "
                //           << topoDist[i][j] << endl;
            }
        }
        EV << "cTopology found " << topo->getNumNodes() << " nodes\n";
        collectNodes(topo);
    }
}

void GlobalView::collectNodes(cTopology *topo)
{
    nodeId2Addr.reserve(topo->getNumNodes());
    std::unordered_set<IntAddress> used;
    for (int i = 0; i < topo->getNumNodes(); i++)
    {
        auto node = topo->getNode(i);
        auto nodeMod = node->getModule();
        IntAddress address = nodeMod->par("address");
        if (address == -1) { // automanually set it by object id which is ensured unique
            address = nodeMod->getId();
            nodeMod->par("address") = address;
        }
        if (used.find(address) != used.end()){
            throw cRuntimeError("GlobalView::collectNodes: duplicate address %" PRId64, address);
        }
        used.insert(address);
        auto isHost = nodeMod->getProperties()->get("host") != nullptr;
        if (isHost) {
            hostNodes.push_back(i);
        }
        nodeId2Addr[i] = address;
        addr2NodeId[address] = i;
        EV_TRACE << "node: " << i << " address: " << address << " isHost:"<< (isHost ? "true":"false")<< endl;
    }
}

