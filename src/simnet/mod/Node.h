#include <omnetpp.h>
#include "simnet/common/Defs.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/app/UnicastSenderApp.h"
#include "simnet/mod/manager/TrafficPatternManager.h"

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

protected:
    double load{0};
    B flowSizeMean{0};
    double flowInterval{0.0};
    double bandwidth{0.0}; // bps
    cMessage* newFlowTimer{nullptr};

private:
    UnicastSenderApp* createUnicastSenderApp();
    void startNewFlow();
    IntAddress generateDestAddr();

private:
    std::vector<opp_component_ptr<UnicastSenderApp>> unicastSenders;
    opp_component_ptr<TrafficPatternManager> tpManager;
    bool loadMode{false};
    PortNumber currPort{1010};
    int numFlows{0};
    int flowCount{0};
};

class SwitchNode : public Node {

};
