#pragma once
#include <unordered_map>
#include "simnet/common/Defs.h"
#include "simnet/mod/AggPacket_m.h"
using namespace omnetpp;

class Aggregator
{
public:
    AggPacket* doAggregation(AggPacket* pk);

    bool checkAdmission(const AggPacket* pk) const;
    int getJobId() const {return jobId;}
    int getRound() const {return jobRound;}
    SeqNumber getSeqNumber() const {return seqNumber;}
    Aggregator(const AggPacket* pk);
    void recordIncomingPorts(const AggPacket* pk);
    const std::unordered_set<int>& getOutGateIndexes() {
        return incomingPortIndexes;
    }
    int multicastCount{0};

public:
    bool fullAggregation{false};
    B usedBuffer{0};

protected:
    void reset();
    void checkThenAddWorkerId(const AggPacket* pk);

protected:
    int jobRound{0};
    int counter{0};
    int jobId{-1};
    int psAddr{-1};
    bool ecn{false};
    SeqNumber seqNumber{-1};
    SeqNumber ackNumber{-1};
    simtime_t timestamp{0}; // arrived time
    std::vector<int64_t> workerRecord; // cheating record all worker's address

private:
    simtime_t accumulatedTime;
    std::unordered_set<int> incomingPortIndexes;
};