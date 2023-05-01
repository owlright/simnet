#include <map>
#include <omnetpp.h>
#include "../mod/Packet_m.h"
#include "../common/Defs.h"
#include "../common/Print.h"
#include "../common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalRouteMangager.h"
#include "simnet/mod/manager/GlobalGroupManager.h"
#include "simnet/mod/agroup/AggGroupEntry.h"
// #include "simnet/mod/agroup/GroupPacketHandler.h"
// #include "../mod/AggrGroupInfo.h"
using namespace omnetpp;
struct hashFunction
{
    size_t operator()(const std::pair<IntAddress , SeqNumber> &x) const{
        return x.first ^ x.second;
    }
};

/**
 * Demonstrates static routing, utilizing the cTopology class.
 */
class Routing : public cSimpleModule
{
private:
    IntAddress myAddress;
    bool ecmpFlow = false;
    typedef std::map<int, std::vector<int>> RoutingTable;  // destaddr -> gateindex
    RoutingTable rtable;
    typedef std::map<int, std::vector<int> > AggrRoutingTable;
    AggrRoutingTable aggrChildren;
    // std::map<int, AggrGroupInfo*> aggrGroupTable;
    GlobalRouteManager* routeManager{nullptr};
    GlobalGroupManager* groupManager{nullptr};
    // GroupPacketHandler* gpkHandler{nullptr};
    simsignal_t dropSignal;
    simsignal_t outputIfSignal;
    simsignal_t outputPacketSignal;
    // std::map<int, Packet*> aggrPacket;
    // std::map<int, int> aggrCounter;
    // std::map<int, int> aggrNumber;

private:
    //! for dealing with agg groups
    class AggPacketHandler {
    public:
        Packet* agg(Packet* pk);
        B getUsedBufferSize() const;
        void releaseGroupOnSeq(IntAddress group, SeqNumber seq);
        const std::vector<int>& getReversePortIndexes(Packet* pk) const;

    private:
        void registerGroup(IntAddress group, B bufferSize);
    public:
        GlobalGroupManager* groupManager{nullptr};
        std::unordered_map<IntAddress, AggGroupEntry*> groupTable;
        B bufferSize{0}; // the max buffer size that in network computation/aggregation can use
        std::unordered_set< std::pair<IntAddress, SeqNumber>, hashFunction > markNotAgg;
    };
    AggPacketHandler aggPacketHandler;
    // bool isAggrGroupAdded(int address) const;
    // bool isAggrGroup(int address) const;
    // AggrGroupInfo* getAggrGroup(int address) const;
    // AggrGroupInfo* getOrAddGroup(int address);
    int getRouteGateIndex(int srcAddr, int destAddr);
    bool isGroupAddr(IntAddress addr) const { return (GROUPADDR_START <= addr && addr < GROUPADDR_END);};
    bool isUnicastAddr(IntAddress addr) const {return !isGroupAddr(addr);};
    void broadcast(Packet* pk, const std::vector<int>& outGateIndexes);

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void refreshDisplay() const override;
};
