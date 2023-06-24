#include <omnetpp.h>
#include "simnet/common/Defs.h"
#include "simnet/app/UnicastApp.h"
using namespace std;

class Node : public cSimpleModule
{
protected:
    IntAddress address;

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage * msg) override {};
    virtual void handleParameterChange(const char *parameterName) override;
    virtual void refreshDisplay() const override;
    virtual int numInitStages() const override { return Stage::NUM_INIT_STAGES; }
};
