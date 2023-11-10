#include "TrafficPatternManager.h"
#include "simnet/common/utils.h"
Define_Module(TrafficPatternManager);

IntAddress TrafficPatternManager::getDestAddr(IntAddress srcAddr) const
{
    if (srcAddr == INVALID_ADDRESS) {
        throw cRuntimeError("TrafficPatternManager::getDestAddr(%" PRId64 "): invalid address!", srcAddr);
    }
    IntAddress dst { INVALID_ADDRESS };
    if (trafficPattern == "uniform") {
        do { // ! avoid send to itself
            auto randNum = this->intrand(idleHosts.size());
            dst = idleHosts.at(randNum);
        } while (dst == srcAddr);
    } else if (trafficPattern == "permutation") {
        auto srcIndex = host2odIndex.at(srcAddr);
        auto destIndexes = odMatrix.at(srcIndex);
        for (auto i = 0; i < destIndexes.size(); i++) {
            dst = idleHosts.at(i);
            break;
        }
    } else {
        throw cRuntimeError("unknown trafficPattern.");
    }
    return dst;
}

std::vector<IntAddress> TrafficPatternManager::getDestAddrs(IntAddress srcAddr, int n) const
{
    std::vector<IntAddress> dests(INVALID_ADDRESS);
    IntAddress tmpdest;
    for (auto i = 0; i < n; i++) {
        if (trafficPattern == "uniform") {
            do { // ! avoid send to itself
                auto randNum = this->intrand(idleHosts.size());
                tmpdest = idleHosts[randNum];
            } while (tmpdest == srcAddr);
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
        if (groupManager == nullptr) { // * no aggregation jobs, we can use all hosts
            auto& hostIds = getHostIds();
            idleHosts.resize(hostIds.size());
            std::transform(hostIds.cbegin(), hostIds.cend(), idleHosts.begin(),
                [this](int id) { return getAddr(id); }); // * convert host index to address
        } else { // * use the left hosts after jobs use
            idleHosts = groupManager->getUnicastHosts();
        }
        int index = 0;
        for (auto& addr:idleHosts) {
            host2odIndex[addr] = index++;
        }
        odMatrix.resize(idleHosts.size(), std::vector<int>(idleHosts.size()));
        permutation(odMatrix, intrand(1234));

        EV_INFO << idleHosts.size() << " hosts for unicast flows." << endl;
    }
}
