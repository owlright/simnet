#include "TrafficPatternManager.h"

Define_Module(TrafficPatternManager);

IntAddress TrafficPatternManager::getDestAddr(IntAddress srcAddr) const
{
    if (srcAddr == INVALID_ADDRESS)
    {
        throw cRuntimeError("TrafficPatternManager::getDestAddr(%" PRId64 "): invalid address!", srcAddr);
    }
    IntAddress dst{INVALID_ADDRESS};
    if (trafficPattern == "uniform")
    {
        do { // ! avoid send to itself
            auto randNum = this->intrand(hostNodes.size());
            auto nodeIndex = hostNodes.at(randNum);
            dst = nodeId2addr.at(nodeIndex);
        }
        while(dst == srcAddr);
    }
    else if (trafficPattern == "") {
        EV_WARN << "Empty trafficPattern!" << endl;
    }
    else {
        throw cRuntimeError("unknown trafficPattern.");
    }
    return dst;
}

std::vector<IntAddress> TrafficPatternManager::getDestAddrs(IntAddress srcAddr, int n) const
{
    std::vector<IntAddress> dests(INVALID_ADDRESS);
    IntAddress tmpdest;
    for (auto i = 0; i < n; i++) {
        if (trafficPattern == "uniform")
        {
            do { // ! avoid send to itself
                auto randNum = this->intrand(hostNodes.size());
                auto nodeIndex = hostNodes.at(randNum);
                tmpdest = nodeId2addr.at(nodeIndex);
            } while(tmpdest == srcAddr);
            dests.push_back(tmpdest);
        }
    }
    return dests;
}

void TrafficPatternManager::initialize(int stage)
{
    GlobalView::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        trafficPattern = par("trafficPattern").stdstringValue();
    }
}
