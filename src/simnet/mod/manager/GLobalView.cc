#include "GlobalView.h"

cTopology * GlobalView::topo = nullptr;
std::vector<int> GlobalView::hostNodes;
std::vector<IntAddress> GlobalView::jobUsedAddrs;
std::unordered_map<int, IntAddress> GlobalView::node2addr;
std::unordered_map<IntAddress, int> GlobalView::addr2node;
std::unordered_map<IntAddress, cModule*> GlobalView::addr2mod;

bool GlobalView::isInitialized = false;

GlobalView::~GlobalView()
{
    delete topo;
    topo = nullptr;
    // ! FIXME
    // ! it's important to reset the isInitialized to false,
    // ! otherwise topo will not load again, I don't know why
    isInitialized = false;
}

void GlobalView::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        if (!isInitialized) {
            EV << "network initialization." << endl;
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
    std::unordered_set<IntAddress> used;
    for (int i = 0; i < topo->getNumNodes(); i++)
    {
        auto node = topo->getNode(i)->getModule();
        IntAddress address = node->par("address");
        if (address == -1) { // automanually set it by object id which is ensured unique
            address = node->getId();
            node->par("address") = address;
        }
        if (used.find(address) != used.end()){
            throw cRuntimeError("GlobalView::collectNodes: duplicate address %" PRId64, address);
        }
        used.insert(address);
        auto isHost = node->getProperties()->get("host") != nullptr;
        if (isHost) {
            hostNodes.push_back(i);
        }
        node2addr[i] = address;
        addr2node[address] = i;
        addr2mod[address] = node;
        EV_TRACE << "node: " << i << " address: " << address << " isHost:"<< (isHost ? "true":"false")<< endl;
    }
}

