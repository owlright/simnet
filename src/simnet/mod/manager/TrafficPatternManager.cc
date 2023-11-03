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
            auto randNum = this->intrand(hostIds.size());
            auto nodeIndex = hostIds.at(randNum);
            dst = getAddr(nodeIndex);
        }
        while(dst == srcAddr);
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
                auto randNum = this->intrand(idleHosts.size());
                tmpdest = idleHosts[randNum];
            } while(tmpdest == srcAddr);
            dests.push_back(tmpdest);
        }
    }
    return dests;
}

void TrafficPatternManager::initialize(int stage)
{
    GlobalManager::initialize(stage);
    if (stage == INITSTAGE_ACCEPT) {
        trafficPattern = par("trafficPattern").stdstringValue();
        groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);
        if (groupManager==nullptr) {
            idleHosts.resize(hostIds.size());
            std::transform(hostIds.cbegin(), hostIds.cend(),
                        idleHosts.begin(),
                        [this](int id){ return getAddr(id);}); // * convert host index to address
        }
        else {
            idleHosts = groupManager->getUnicastHosts();
        }
        EV_INFO << idleHosts.size() << " hosts for unicast flows." << endl;
    }
}
