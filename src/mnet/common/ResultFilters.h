#include <omnetpp.h>
using namespace omnetpp;
/**
 * Filter that expects a Packet or a packet length and outputs the throughput as double.
 * Throughput is computed for the *past* interval every 0.1s or 100 packets,
 * whichever comes first. The filter reads the interval and numLengthLimit
 * parameters from the INI file configuration.
 *
 * Note that this filter is unsuitable for interactive use (with instrument figures,
 * for example), because zeroes for long silent periods are only emitted retroactively,
 * when the silent period (or the simulation) is over.
 *
 * Recommended interpolation mode: backward sample-hold.
 */
class ThroughputFilter : public cObjectResultFilter
{
  protected:
    simtime_t interval = -1;
    int numLengthLimit = -1;
    bool emitIntermediateZeros = true;

    simtime_t lastSignalTime;
    double totalLength = 0;
    int numLengths = 0;

  protected:
    virtual void init(Context *ctx) override;
    virtual ThroughputFilter *clone() const override;
    virtual void emitThroughput(simtime_t endInterval, cObject *details);

  public:
    virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, intval_t value, cObject *details) override;
    virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details) override;
    virtual void finish(cComponent *component, simsignal_t signalID) override;
};