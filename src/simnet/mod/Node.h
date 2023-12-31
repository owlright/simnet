#include "simnet/app/FlowApp.h"
#include "simnet/common/Defs.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/TrafficPatternManager.h"
#include <omnetpp.h>

using namespace omnetpp;

class Node : public cSimpleModule {
public:
    virtual ~Node() {};

protected:
    IntAddress address;

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage* msg) override;
    virtual void handleParameterChange(const char* parameterName) override;
    virtual void refreshDisplay() const override;
    virtual int numInitStages() const override { return Stage::NUM_INIT_STAGES; }
    virtual void finish() override {};
};

class HostNode : public Node {
public:
    virtual ~HostNode();

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage* msg) override;
    virtual void finish() override;

protected:
    double load { 0 };
    B flowSizeMean { 0 };
    double flowInterval { 0.0 };
    double bandwidth { 0.0 }; // bps
    cMessage* newFlowTimer { nullptr };

private:
    FlowApp* createCongApp();
    void startNewFlow();
    IntAddress generateDestAddr();

private:
    std::vector<opp_component_ptr<FlowApp>> unicastSenders;
    opp_component_ptr<TrafficPatternManager> tpManager;
    opp_component_ptr<GlobalMetricCollector> metricCollector;
    bool loadMode { false };
    int numFlows { 0 };
    int flowCount { 0 };
};

class SwitchNode : public Node { };
