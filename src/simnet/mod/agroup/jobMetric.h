#pragma once
#include <unordered_map>
#include "simnet/common/Defs.h"
#include "simnet/mod/Packet_m.h"
using namespace omnetpp;

// this class for collecting agg group's metrics
class jobMetric
{
public:
    jobMetric(cModule* module, IntAddress group);
    simtime_t getUsedTime() const { return accumulatedTime;};
    int getComputationCount() const {return compCount;};
    B getUsedBuffer() const {return usedBuffer;};
    void addUsedBuffer(B size) {usedBuffer += size;}
    void releaseUsedBuffer(B size) {usedBuffer -= size;}
    // create signals for the router
    void createBufferSignalForGroup(IntAddress group);
    // emit signals for the router
    void emitAllSignals();

private:
    cModule* module;
    simsignal_t usedBufferSignal;
    simtime_t accumulatedTime;
    int compCount{0};
    B usedBuffer{0};
    int jobId;
};

