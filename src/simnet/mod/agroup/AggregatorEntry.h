#pragma once
#include <unordered_map>
#include "simnet/common/Defs.h"
#include "simnet/mod/AggPacket_m.h"
#include "Aggregator.h"
using namespace omnetpp;

class ATPEntry : public  Aggregator
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

class MTATPEntry: public Aggregator
{

};
