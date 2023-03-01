#ifndef NODE_CONTROLLER_H_
#define NODE_CONTROLLER_H_

#include <omnetpp/csimplemodule.h>
using namespace omnetpp;

class Controller: public omnetpp::cSimpleModule {
public:
    int getRoute(cModule* from, int to);
    Controller();
    virtual ~Controller();
private:
    cTopology *topo;

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override { throw cRuntimeError("this module doesn't handle messages, it runs only in initialize()"); }
};

#endif /* NODE_CONTROLLER_H_ */
