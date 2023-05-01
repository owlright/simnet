#pragma once

#include <omnetpp.h>
#include <unordered_map>
#include "simnet/common/Defs.h"
#include "simnet/mod/Packet_m.h"
#include "simnet/mod/agroup/AggGroupEntry.h"
#include "simnet/mod/manager/GlobalGroupManager.h"
using namespace omnetpp;

// Hash function
struct hashFunction
{
    size_t operator()(const std::pair<IntAddress , SeqNumber> &x) const{
        return x.first ^ x.second;
    }
};


class GroupPacketHandler : public cSimpleModule {
public:
    Packet* agg(Packet* pk);
    B getUsedBufferSize() const;
    void releaseGroupOnSeq(IntAddress group, SeqNumber seq);
    const std::vector<int>& getReversePortIndexes(Packet* pk) const;

protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override { throw cRuntimeError("this module doesn't handle messages, it runs only in initialize()"); }

private:
    void registerGroup(IntAddress group, B bufferSize);

private:
    GlobalGroupManager* groupManager{nullptr};
    std::unordered_map<IntAddress, AggGroupEntry*> groupTable;
    B bufferSize{0}; // the max buffer size that in network computation/aggregation can use
    std::unordered_set< std::pair<IntAddress, SeqNumber>, hashFunction > markNotAgg;
};

