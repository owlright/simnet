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
#include "simnet/mod/agroup/Aggregator.h"
using namespace omnetpp;

/**
 * Demonstrates static routing, utilizing the cTopology class.
 */
class Routing : public cSimpleModule
{
    // struct hashFunction
    // {
    //     size_t operator()(const std::pair<IntAddress , SeqNumber> &x) const{
    //         return x.first ^ x.second;
    //     }
    // };
    typedef struct MulticastID
    {
        IntAddress addr;
        PortNumber port;
        SeqNumber seq;
        MulticastID(IntAddress PSAddr, PortNumber PSport, SeqNumber seq)
        {
            this->addr = PSAddr;
            this->port = PSport;
            this->seq = seq;
        }
        // `operator==` is required to compare keys in case of a hash collision
        bool operator==(const MulticastID &key) const {
            return addr == key.addr && port == key.port && seq == key.seq;
        }
    } MulticastID;

    struct hash_fn
    {
        std::size_t operator() (const MulticastID& key) const
        {
            auto hashfn = std::hash<int64_t>();
            std::size_t h1 = hashfn(key.addr);
            std::size_t h2 = hashfn((int64_t)key.port);
            std::size_t h3 = hashfn(key.seq);
            return h1 ^ h2 ^ h3;
        }
    };

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
    std::unordered_set<MulticastID, hash_fn> markNotAgg;
    std::unordered_map<MulticastID, std::vector<int>, hash_fn> incomingPortIndexes;
    std::unordered_map<IntAddress, jobMetric*> groupMetricTable;
    std::vector<Aggregator*> aggregators;
    std::unordered_map<MulticastID, int64_t, hash_fn> seqDeadline;

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
    std::vector<int> getReversePortIndexes(const MulticastID& groupSeqKey) const;
    [[deprecated]] Packet* doAggregation(Packet* pk);
    Packet* aggregate(AggPacket* pk);
    [[deprecated]] bool addGroupEntry(IntAddress group, B bufferCanUsed, B firstDataSize, int indegree);
    [[deprecated]] bool tryAddSeqEntry(const Packet* pk);
    void recordIncomingPorts(MulticastID& groupSeqKey, int port);

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void finish() override;
};
