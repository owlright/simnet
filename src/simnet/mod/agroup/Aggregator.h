#pragma once
#include <unordered_map>
#include "simnet/common/Defs.h"
#include "simnet/mod/AggPacket_m.h"
using namespace omnetpp;

class Aggregator
{
public:
    virtual Packet* doAggregation(AggPacket* pk);
    virtual ~Aggregator();

    // 1. aggregator is in use and jobid, seq is the same
    // 2. aggregator is empty
    // ! when the packet is a resend packet, leave it to doAggregation
    bool checkAdmission(const AggPacket* pk) const;
    const int getJobId() const {return jobId;}
    const int getSeqNumber() const {return seqNumber;}

protected:
    virtual void reset();
    void checkThenAddWorkerId(const AggPacket* pk);

protected:
    int counter{0};
    int jobId{-1};
    bool ecn{false};
    SeqNumber seqNumber{-1};
    simtime_t timestamp{0}; // arrived time
    std::vector<int64_t> workerRecord; // cheating record all worker's address

private:
    simtime_t accumulatedTime;
};