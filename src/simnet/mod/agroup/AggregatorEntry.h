#pragma once
#include <unordered_map>
#include "simnet/common/Defs.h"
#include "simnet/mod/Packet_m.h"
using namespace omnetpp;

class AggregatorEntry
{
public:
    virtual Packet* doAggregation(Packet* pk);
    virtual ~AggregatorEntry();
    bool checkAdmission(Packet* pk) const;
    const int getJobId() const {return jobId;}
    const int getSeqNumber() const {return seqNumber;}

protected:
    virtual void reset();
    void checkThenAddWorkerId(const Packet* pk);

protected:
    bool isIdle{true};
    int counter{0};
    int jobId{-1};
    bool ecn{false};
    SeqNumber seqNumber{-1};
    simtime_t timestamp{0}; // arrived time
    std::vector<int64_t> workerRecord; // cheating record all worker's address

private:
    simtime_t accumulatedTime;
};

class ATPEntry : public  AggregatorEntry
{
public:
    ATPEntry(const Packet* pk);
    virtual Packet* doAggregation(Packet* pk) override;

protected:
    virtual void reset() override;
private:
    struct Temp
    {
        uint32_t bitmap{0};
        int fanIndegree{0};
    } temp;
    uint32_t bitmap{0};

};

class MTATPEntry: public AggregatorEntry
{

};
