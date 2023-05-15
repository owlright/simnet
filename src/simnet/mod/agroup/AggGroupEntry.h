#pragma once
#include <unordered_map>
#include "simnet/common/Defs.h"
#include "simnet/mod/Packet_m.h"
using namespace omnetpp;

class GlobalGroupManager;

struct AggGroupEntry
{
public:
    // friend GlobalGroupManager;
    explicit AggGroupEntry(B size, int indegree);
    Packet* agg(Packet* pk);
    B release(const Packet* pk);

    simtime_t getUsedTime() const { return accumulatedTime;};
    int getComputationCount() const {return computationCount;};
    B getLeftBuffer() const {return bufferSize - usedBuffer;};

private:
    struct AggPacketEntry {
    public:
        Packet* agg(Packet *pk);
        explicit AggPacketEntry(SeqNumber seq);

        SeqNumber seq{INVALID_ID};
        Packet* packet{nullptr};
        B usedBytes{0};
        int fanIndegree{0};
        int counter{0};
        int32_t timer{0};
        int computationCount{0};
        simtime_t startTime;
    };

private:
    int computationCount{0};
    simtime_t accumulatedTime;
    IntAddress groupAddr{INVALID_ADDRESS};
    B bufferSize{0};
    B usedBuffer{0};
    int indegree;
    // * store packets of the same seq
    std::unordered_map<SeqNumber, AggPacketEntry*> packetTable;
};

