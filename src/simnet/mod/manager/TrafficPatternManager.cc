#include "TrafficPatternManager.h"
#include "simnet/common/utils.h"
#include <random>
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
            if (destIndexes.at(i) > 0) {
                dst = idleHosts.at(i);
                break;
            }
        }
    } else {
        throw cRuntimeError("unknown trafficPattern.");
    }
    ASSERT(dst != INVALID_ADDRESS);
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
        idleHostsNumber = par("idleHostsNumber").intValue();
        groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);
        if (groupManager == nullptr) { // * no aggregation jobs, we can use all hosts
            auto& hostIds = getHostIds();
            idleHosts.resize(hostIds.size());
            std::transform(hostIds.cbegin(), hostIds.cend(), idleHosts.begin(),
                [this](int id) { return getAddr(id); }); // * convert host index to address
        } else { // * use the left hosts after jobs use
            idleHosts = groupManager->getUnicastHosts();
        }
        // std::cout << "idleHosts.size() = " << idleHosts.size() << std::endl;
        std::shuffle(idleHosts.begin(), idleHosts.end(), std::default_random_engine(intrand(1234)));
        int keepHostNum = (int)idleHosts.size();
        if (idleHostsNumber > 0 && idleHostsNumber < keepHostNum) {
            keepHostNum = idleHostsNumber;
        }
        if (keepHostNum < 2) {
            throw cRuntimeError("At least 2 hosts are required for traffic pattern.");
        }
        idleHosts.resize(keepHostNum);
        // std::cout << "idleHosts.size() = " << idleHosts.size() << std::endl;
        // ! You can only change the idleHosts before storing host2odIndex
        int index = 0;
        for (auto& addr:idleHosts) {
            host2odIndex[addr] = index++;
        }
        if (trafficPattern == "permutation") {
            odMatrix.resize(idleHosts.size(), std::vector<int>(idleHosts.size()));
            permutation(odMatrix, intrand(1234));
        }

        EV_INFO << idleHosts.size() << " hosts for unicast flows." << endl;
    }
}
