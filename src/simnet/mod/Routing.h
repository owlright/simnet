#include <map>
#include <omnetpp.h>
#include "../mod/Packet_m.h"
#include "../common/Defs.h"
#include "../common/Print.h"
#include "../common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalRouteMangager.h"
#include "simnet/mod/manager/GlobalGroupManager.h"
#include "simnet/mod/agroup/AggGroupEntry.h"

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
    bool isSwitch;
    IntAddress myAddress{INVALID_ADDRESS};
    IntAddress myGroupAddress{INVALID_ADDRESS};
    bool ecmpFlow = false;
    typedef std::map<int, std::vector<int>> RoutingTable;  // destaddr -> gateindex
    RoutingTable rtable;

    GlobalRouteManager* routeManager{nullptr};
    GlobalGroupManager* groupManager{nullptr};
    // GroupPacketHandler* gpkHandler{nullptr};
    simsignal_t dropSignal;
    simsignal_t outputIfSignal;
    simsignal_t outputPacketSignal;

    B bufferSize{0};
    B usedBuffer{0};
    std::unordered_set< std::pair<IntAddress, SeqNumber>, hashFunction > markNotAgg;
    std::unordered_map<IntAddress, AggGroupEntry*> groupTable;

private:
    // AggPacketHandler aggPacketHandler;
    int getRouteGateIndex(int srcAddr, int destAddr);
    bool isGroupAddr(IntAddress addr) const { return (GROUPADDR_START <= addr && addr < GROUPADDR_END);};
    bool isUnicastAddr(IntAddress addr) const {return !isGroupAddr(addr);};
    void broadcast(Packet* pk, const std::unordered_set<int>& outGateIndexes);
    std::unordered_set<int> getReversePortIndexes(Packet *pk) const;
    int getComputationCount() const;
    simtime_t getUsedTime() const;

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void refreshDisplay() const override;
    virtual void finish() override;
};
