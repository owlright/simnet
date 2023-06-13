#pragma once
#include <unordered_map>
#include "simnet/common/Defs.h"
#include "simnet/mod/Packet_m.h"
using namespace omnetpp;

class AggregatorEntry
{
public:
    virtual Packet* doAggregation(Packet* pk);
    bool checkAdmission(Packet* pk) const;

protected:
    virtual void reset();

protected:
    bool isIdle{true};
    int counter{0};
    int jobId{-1};
    bool ecn{false};
    SeqNumber seqNumber{-1};
    simtime_t timestamp{0}; // arrived time

private:
    simtime_t accumulatedTime;
};

class ATPEntry : public  AggregatorEntry
{
public:
    virtual Packet* doAggregation(Packet* pk) override;

protected:
    virtual void reset() override;
private:
    struct Temp
    {
        uint32_t bitmap{0};
        int fanIndegree{0};
    } temp;
    uint32_t bitmap;
};

class MTATPEntry: public AggregatorEntry
{

};