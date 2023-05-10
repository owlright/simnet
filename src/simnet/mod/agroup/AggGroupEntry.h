#pragma once
#include <unordered_map>
#include "simnet/common/Defs.h"
#include "simnet/mod/Packet_m.h"
using namespace omnetpp;

class GlobalGroupManager;

struct AggGroupEntry
{
public:
    friend GlobalGroupManager;
    // const Packet* getAggPacket(SeqNumber seq) const;
    explicit AggGroupEntry(B size, int indegree);
    Packet* agg(Packet* pk);
    void release(const Packet* pk);

    B getUsedBufferSize() const;
    simtime_t getUsedTime() const { return accumulatedTime;};
    int getComputationCount() const {return computationCount;};
    const std::unordered_set<int>& getIncomingPortIndexes(SeqNumber seq) const {
        return packetTable.at(seq)->incomingPortIndexes;
    };


private:
    struct AggPacketEntry {
    public:
        Packet* agg(Packet *pk);
        explicit AggPacketEntry(SeqNumber seq, int indegree);

        SeqNumber seq{INVALID_ID};
        Packet* packet{nullptr};
        int fanIndegree{0};
        int counter{0};
        std::unordered_set<int> incomingPortIndexes;
        int computationCount{0};
        simtime_t startTime;
    };

private:
    int computationCount{0};
    simtime_t accumulatedTime;
    IntAddress groupAddr{INVALID_ADDRESS};
    B bufferSize{0};
    int indegree;
    // * store packets of the same seq
    std::unordered_map<SeqNumber, AggPacketEntry*> packetTable;
    std::unordered_set<SeqNumber> markNotAgg;
};

