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

void TrafficPatternManager::initialize(int stage)
{
    GlobalView::initialize(stage);
    if (stage == INITSTAGE_LOCAL)
    {
        trafficPattern = par("trafficPattern").stdstringValue();
    }
}
