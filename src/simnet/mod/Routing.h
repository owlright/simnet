#include <map>
#include <omnetpp.h>
#include "../mod/Packet_m.h"
#include "simnet/mod/AggPacket_m.h"
#include "../common/Defs.h"
#include "../common/Print.h"
#include "../common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalRouteMangager.h"
#include "simnet/mod/manager/GlobalGroupManager.h"
#include "simnet/mod/agroup/jobMetric.h"
#include "simnet/mod/agroup/AggregatorEntry.h"
using namespace omnetpp;

/**
 * Demonstrates static routing, utilizing the cTopology class.
 */
class Routing : public cSimpleModule
{
    struct hashFunction
    {
        size_t operator()(const std::pair<IntAddress , SeqNumber> &x) const{
            return x.first ^ x.second;
        }
    };
    typedef std::pair<IntAddress, SeqNumber> AddrSeqType;

public:
    virtual ~Routing();

private:
    bool isSwitch;
    IntAddress myAddress{INVALID_ADDRESS};
    int position{-1};
    // IntAddress myGroupAddress{INVALID_ADDRESS};
    bool ecmpFlow = false;
    double collectionPeriod;
    // TODO improve the code
    [[deprecated]] std::string aggPolicy;
    [[deprecated]] bool isTimerPolicy{false};

    typedef std::map<int, std::vector<int>> RoutingTable;  // destaddr -> gateindex
    RoutingTable rtable;

    GlobalRouteManager* routeManager{nullptr};
    // GlobalGroupManager* groupManager{nullptr};
    // GroupPacketHandler* gpkHandler{nullptr};
    simsignal_t dropSignal;
    simsignal_t outputIfSignal;

    B bufferSize{0};
    B usedBuffer{0};
    int numAggregators{0};
    B agtrSize;
    std::unordered_set<AddrSeqType, hashFunction> markNotAgg;
    std::unordered_map<AddrSeqType, std::vector<int>, hashFunction> incomingPortIndexes;
    std::unordered_map<IntAddress, jobMetric*> groupMetricTable;
    std::vector<AggregatorEntry*> aggregators;
    std::unordered_map<AddrSeqType, int64_t, hashFunction> seqDeadline;

private:
    // ! self messages
    cMessage* aggTimeOut{nullptr};
    cMessage* dataCollectTimer{nullptr};

    // ! common router functions

    // Ask global routeManager for the first seen destAddr
    // and store it in rtable for next time search
    int getRouteGateIndex(int srcAddr, int destAddr);
    [[deprecated]] bool isGroupAddr(IntAddress addr) const { return (GROUPADDR_START <= addr && addr < GROUPADDR_END);};
    [[deprecated]] bool isUnicastAddr(IntAddress addr) const {return !isGroupAddr(addr);};

    // ! common forwarding functions
    void broadcast(Packet* pk, const std::vector<int>& outGateIndexes);
    void forwardIncoming(Packet* pk);

    // ! for data collection
    simsignal_t createBufferSignalForGroup(IntAddress group);
    int getComputationCount() const;
    simtime_t getUsedTime() const;

    // ! for aggregation
    std::vector<int> getReversePortIndexes(const AddrSeqType& groupSeqKey) const;
    [[deprecated]] Packet* doAggregation(Packet* pk);
    Packet* aggregate(AggPacket* pk);
    [[deprecated]] bool addGroupEntry(IntAddress group, B bufferCanUsed, B firstDataSize, int indegree);
    [[deprecated]] bool tryAddSeqEntry(const Packet* pk);
    void recordIncomingPorts(AddrSeqType& groupSeqKey, int port);

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void refreshDisplay() const override;
    virtual void finish() override;
};
