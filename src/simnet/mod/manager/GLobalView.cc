#include "GlobalView.h"

cTopology * GlobalView::topo = nullptr;
std::vector<int> GlobalView::hostNodes;
std::vector<IntAddress> GlobalView::jobUsedAddrs;
std::unordered_map<int, IntAddress> GlobalView::nodeId2addr;
std::unordered_map<IntAddress, int> GlobalView::addr2nodeId;
std::unordered_map<IntAddress, cModule*> GlobalView::addr2mod;
std::unordered_map<cModule*, IntAddress> GlobalView::mod2addr;
std::unordered_map<IntAddress, cTopology::Node*> GlobalView::addr2node;
std::unordered_map<cTopology::Node*, IntAddress> GlobalView::node2addr;
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
    nodeId2addr.reserve(topo->getNumNodes());
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
        nodeId2addr[i] = address;
        addr2nodeId[address] = i;
        addr2mod[address] = nodeMod;
        mod2addr[nodeMod] = address;
        node2addr[node] = address;
        addr2node[address] = node;
        EV_TRACE << "node: " << i << " address: " << address << " isHost:"<< (isHost ? "true":"false")<< endl;
    }
}

