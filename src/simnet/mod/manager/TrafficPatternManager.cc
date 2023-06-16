#include "TrafficPatternManager.h"

Define_Module(TrafficPatternManager);

IntAddress TrafficPatternManager::getDestAddr(IntAddress srcAddr) const
{
    IntAddress dst{INVALID_ADDRESS};
    if (trafficPattern == "uniform")
    {
        do { // ! avoid send to itself
            auto randNum = this->intrand(hostNodes.size());
            auto nodeIndex = hostNodes.at(randNum);
            dst = node2addr.at(nodeIndex);
        }
        while(dst == srcAddr);
    }
    if (dst == INVALID_ADDRESS)
    {
        throw cRuntimeError("TrafficPatternManager::getDestAddr(%lld): invalid address!", srcAddr);
    }
    return dst;
}

std::vector<IntAddress> TrafficPatternManager::getDestAddrs(IntAddress srcAddr, int n) const
{
    std::vector<IntAddress> dests(INVALID_ADDRESS);
    int count = 0;
    IntAddress tmpdest;
    for (auto i = 0; i < n; i++) {
        if (trafficPattern == "uniform")
        {
            do { // ! avoid send to itself
                auto randNum = this->intrand(hostNodes.size());
                auto nodeIndex = hostNodes.at(randNum);
                tmpdest = node2addr.at(nodeIndex);
            } while(tmpdest == srcAddr);
            dests.push_back(tmpdest);
        }
    }
    return dests;
}

void TrafficPatternManager::initialize(int stage)
{
    GlobalView::initialize(stage);
    if (stage == INITSTAGE_LOCAL)
    {
        trafficPattern = par("trafficPattern").stdstringValue();
    }
    if (stage == INITSTAGE_LAST)
        ASSERT(topo);
}
