#include "GlobalRouteMangager.h"

Define_Module(GlobalRouteManager);

void GlobalRouteManager::initialize(int stage)
{
    GlobalView::initialize(stage);
    if (stage == INITSTAGE_LAST)
        ASSERT(topo);
}

cTopology::Node* GlobalRouteManager::getNode(IntAddress address) const
{
    auto its = addr2node.find(address);
    if (its == addr2node.end()) {
       throw cRuntimeError("address %lld does not exist!", address);
    }
    return topo->getNode(its->second);
}

std::vector<int> GlobalRouteManager::getRoutes(IntAddress switchAddr, IntAddress dest) const // TODO just pass the src address is ok
{
    Enter_Method("GlobalRouteManager::getRoutes");
    if (switchAddr == dest) {
        throw cRuntimeError("switchAddr and dest are the same.");
    }
    auto srcNode = getNode(switchAddr);
    auto destNode = getNode(dest);

    // ! HACK find multiple next hops used for ecmp
    // * try to get one shortest path
    topo->calculateUnweightedSingleShortestPathsTo(destNode);
    auto distance = srcNode->getDistanceToTarget();
    if (distance == INFINITY) {
        throw cRuntimeError("there is no path from %" PRId64 " to %" PRId64, switchAddr, dest);
    }
    // * find path whose length <= distance
    std::vector<int> gateIndexes;
    std::vector<cTopology::LinkOut *> linkrecord; // for reset use
    // HACK: find ecmp paths
    while (srcNode->getNumPaths() > 0) {
        if (srcNode->getDistanceToTarget() <= distance) {
            auto outLink = srcNode->getPath(0);
            int gateIndex = outLink->getLocalGate()->getIndex();
            gateIndexes.push_back(gateIndex);
            EV_DEBUG << "src address: " << srcNode->getModule()->par("address").intValue()
                    <<"  towards address " << dest
                    << " gateIndex is " << gateIndex << endl;
        }
        // ! disable this link and recalculate shortest paths
        srcNode->getPath(0)->disable();
        linkrecord.push_back(srcNode->getPath(0));
        topo->calculateUnweightedSingleShortestPathsTo(destNode);
    }
    // reset the links
    for (auto link:linkrecord) {
        link->enable();
    }
    return gateIndexes;
}
