#include <omnetpp.h>
#include "simnet/common/Defs.h"
#include "simnet/app/UnicastSenderApp.h"
using namespace omnetpp;

class Node : public cSimpleModule
{
public:
    virtual ~Node() {};

protected:
    IntAddress address;

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage * msg) override;
    virtual void handleParameterChange(const char *parameterName) override;
    virtual void refreshDisplay() const override;
    virtual int numInitStages() const override { return Stage::NUM_INIT_STAGES; }

};

class HostNode : public Node {
public:
    virtual ~HostNode();

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage * msg) override;

private:
    opp_component_ptr<UnicastSenderApp> createUnicastSenderApp();
    void startNewFlow();

protected:
    double load{0};
    B flowSizeMean{0};
    double flowInterval{0.0};
    double bandwidth{0.0}; // bps
    const char* trafficPattern;

    cMessage* newFlowTimer{nullptr};
};

class SwitchNode : public Node {

};
