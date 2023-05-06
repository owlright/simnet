#include "GlobalView.h"

cTopology * GlobalView::topo = nullptr;
std::vector<int> GlobalView::hostNodes;
std::unordered_map<int, IntAddress> GlobalView::node2addr;
std::unordered_map<IntAddress, int> GlobalView::addr2node;
bool GlobalView::isInitialized = false;

void GlobalView::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        if (!isInitialized) {
            EV_INFO << "network initialization." << endl;
            topo = new cTopology("topo");
            topo->extractByProperty("node");
            EV << "cTopology found " << topo->getNumNodes() << " nodes\n";
            collectNodes(topo);
            isInitialized = true;
        }
    }
}

void GlobalView::collectNodes(cTopology *topo)
{
    node2addr.reserve(topo->getNumNodes());
    for (int i = 0; i < topo->getNumNodes(); i++)
    {
        auto node = topo->getNode(i)->getModule();
        int address = node->par("address");
        auto isHost = node->getProperties()->get("host") != nullptr;
        if (isHost) {
            hostNodes.push_back(i);
        }
        node2addr[i] = address;
        addr2node[address] = i;
        EV_DEBUG<< "node: " << i << " address: " << address << endl;
    }
}

