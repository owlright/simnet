#include "ResultFilters.h"
Register_ResultFilter("throughput", ThroughputFilter);

void ThroughputFilter::init(Context *ctx)
{
    cObjectResultFilter::init(ctx);
    std::string fullPath = ctx->component->getFullPath() + "." + ctx->attrsProperty->getIndex() + ".throughput";
    auto intervalValue = getEnvir()->getConfig()->getPerObjectConfigValue(fullPath.c_str(), "interval");
    interval = cConfiguration::parseDouble(intervalValue, "s", nullptr, 0.1);
    auto numLengthLimitValue = getEnvir()->getConfig()->getPerObjectConfigValue(fullPath.c_str(), "numLengthLimit");
    numLengthLimit = cConfiguration::parseLong(numLengthLimitValue, nullptr, 100);
    lastSignalTime = simTime();
}

ThroughputFilter *ThroughputFilter::clone() const
{
    auto clone = new ThroughputFilter();
    clone->interval = interval;
    clone->numLengthLimit = numLengthLimit;
    return clone;
}

void ThroughputFilter::emitThroughput(simtime_t endInterval, cObject *details)
{
    double throughput = endInterval == lastSignalTime ? 0 : totalLength / (endInterval - lastSignalTime).dbl();
    fire(this, endInterval, throughput, details);
    lastSignalTime = endInterval;
    totalLength = 0;
    numLengths = 0;
}

void ThroughputFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, intval_t length, cObject *details)
{
    const simtime_t now = simTime();
    numLengths++;
    ASSERT(numLengths <= numLengthLimit);
    if (numLengthLimit > 0 && numLengths == numLengthLimit) {
        totalLength += length;
        emitThroughput(now, details);
    }
    else if (lastSignalTime + interval <= now) {
        emitThroughput(lastSignalTime + interval, details);
        if (emitIntermediateZeros) {
            while (lastSignalTime + interval <= now)
                emitThroughput(lastSignalTime + interval, details);
        }
        else {
            if (lastSignalTime + interval <= now) { // no packets arrived for a long period
                // zero should have been signaled at the beginning of this packet (approximation)
                emitThroughput(now - interval, details);
            }
        }
        totalLength += length;
    }
    else
        totalLength += length;
}

void ThroughputFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details)
{
    if (auto packet = dynamic_cast<cPacket *>(object))
        receiveSignal(prev, t, packet->getBitLength(), details);
}

void ThroughputFilter::finish(cComponent *component, simsignal_t signalID)
{
    const simtime_t now = simTime();
    if (lastSignalTime < now) {
        cObject *details = nullptr;
        if (lastSignalTime + interval < now) {
            emitThroughput(lastSignalTime + interval, details);
            if (emitIntermediateZeros) {
                while (lastSignalTime + interval < now)
                    emitThroughput(lastSignalTime + interval, details);
            }
        }
        emitThroughput(now, details);
    }
}
