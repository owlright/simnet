#include <map>
#include <unordered_set>
#include <omnetpp.h>
#include "../mod/Packet_m.h"
#include "simnet/mod/AggPacket_m.h"
#include "../common/Defs.h"
#include "../common/Print.h"
#include "../common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalRouteMangager.h"
#include "simnet/mod/manager/GlobalGroupManager.h"
#include "simnet/mod/agroup/jobMetric.h"
#include "simnet/mod/agroup/Aggregator.h"
using namespace omnetpp;

/**
 * Demonstrates static routing, utilizing the cTopology class.
 */
class Routing : public cSimpleModule
{
    // * a key for lookup reverse MACK
    struct AgtrID {
        IntAddress addr;
        SeqNumber seq;
        int jobId; // ! the future packet will arrive
        AgtrID(IntAddress addr, SeqNumber seq, int jobId) {
            this->addr = addr;
            this->seq = seq;
            this->jobId = jobId;
        }
        bool operator==(const AgtrID& key) const {
            return addr == key.addr && jobId == key.jobId && seq == key.seq;
        }
        struct hash_fn
        {
            std::size_t operator() (const AgtrID& key) const
            {
                auto hashfn = std::hash<size_t>();
                return hashfn(key.addr) ^ hashfn(key.seq) ^ hashfn(key.jobId);
            }
        };
    };

    // * a key for lookup reverse MACK
    struct AddrGate {
        IntAddress addr;
        SeqNumber seq;
        int inGateIndex; // ! the future packet will arrive
        AddrGate(IntAddress addr, SeqNumber seq, int inGateIndex) {
            this->addr = addr;
            this->seq = seq;
            this->inGateIndex = inGateIndex;
        }
        bool operator==(const AddrGate& key) const {
            return addr == key.addr && inGateIndex == key.inGateIndex && seq == key.seq;
        }
        struct hash_fn
        {
            std::size_t operator() (const AddrGate& key) const
            {
                auto hashfn = std::hash<size_t>();
                return hashfn(key.addr) ^ hashfn(key.seq) ^ hashfn(key.inGateIndex);
            }
        };
    };

public:
    virtual ~Routing();

private:
    bool isSwitch;
    IntAddress myAddress{INVALID_ADDRESS};
    [[deprecated]] int position{-1};

    bool ecmpFlow = false;
    double collectionPeriod;
    // TODO improve the code
    [[deprecated]] std::string aggPolicy;
    [[deprecated]] bool isTimerPolicy{false};

    typedef std::map<int, std::vector<int>> RoutingTable;  // destaddr -> gateindex
    RoutingTable rtable;
    std::unordered_map<AddrGate, std::unordered_set<int>, AddrGate::hash_fn> groupUnicastTable;
    GlobalRouteManager* routeManager{nullptr};

    B bufferSize{0};
    int agtrCount{0};
    B usedBuffer{0};
    B agtrSize;
    bool useAgtrIndex{false};

    // [[deprecated]] std::unordered_set<MulticastID, hash_fn> markNotAgg;
    // std::unordered_map<MulticastID, std::vector<int>, hash_fn> incomingPortIndexes;
    std::unordered_map<IntAddress, jobMetric*> groupMetricTable;
    std::map<size_t, Aggregator*> aggregators;
    std::unordered_map<AgtrID, size_t, AgtrID::hash_fn> agtrIndexes;
    // [[deprecated]]std::unordered_map<MulticastID, int64_t, hash_fn> seqDeadline;

private:
    Aggregator* tryGetAgtr(const AggPacket* apk);
    void tryReleaseAgtr(const AggPacket* apk);

private:
    // ! self messages
    // cMessage* aggTimeOut{nullptr};
    cMessage* dataCollectTimer{nullptr};
    static simsignal_t bufferInUseSignal;
    // ! common router functions

    // Ask global routeManager for the first seen destAddr
    // and store it in rtable for next time search
    int getRouteGateIndex(int srcAddr, int destAddr);
    [[deprecated]] bool isGroupAddr(IntAddress addr) const { return (GROUPADDR_START <= addr && addr < GROUPADDR_END);};
    [[deprecated]] bool isUnicastAddr(IntAddress addr) const {return !isGroupAddr(addr);};

    // ! common forwarding functions
    void broadcast(Packet* pk, const std::vector<int>& outGateIndexes);
    void broadcast(Packet* pk, const std::unordered_set<int>& outGateIndexes);
    void forwardIncoming(Packet* pk);
    void processAggPacket(AggPacket*& pk);
    int getForwardGateIndex(const Packet* pk, IntAddress nextAddr=-1);

    // ! for data collection
    simsignal_t createBufferSignalForGroup(IntAddress group);
    int getComputationCount() const;
    simtime_t getUsedTime() const;

    // ! for aggregation
    // std::vector<int> getReversePortIndexes(const MulticastID& groupSeqKey) const;
    [[deprecated]] Packet* doAggregation(Packet* pk);
    [[deprecated]] Packet* aggregate(AggPacket* pk);
    [[deprecated]] bool addGroupEntry(IntAddress group, B bufferCanUsed, B firstDataSize, int indegree);
    [[deprecated]] bool tryAddSeqEntry(const Packet* pk);
    // [[deprecated]] void recordIncomingPorts(MulticastID& groupSeqKey, int port);

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void finish() override;
    virtual void handleParameterChange(const char *parameterName) override;
};
