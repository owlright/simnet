#ifndef NODE_CONTROLLER_H_
#define NODE_CONTROLLER_H_

#include <omnetpp/csimplemodule.h>
using namespace omnetpp;

class Controller: public omnetpp::cSimpleModule {
private:
    typedef std::map<int, std::map<int,int> > aggrGroupOnRouterTable;
public:
    int getRoute(cModule* from, int to);
    int getGroupInfo(int groupid, int routerid, const aggrGroupOnRouterTable& table) const;
    int getGroupAggrNum(int groupid, int routerid) const;
    int getGroupAggrBuffer(int groupid, int routerid) const;
    bool isAggrGroupOnRouter(int groupid, int routerid) const;
    Controller();
    virtual ~Controller();
private:
    cTopology *topo;

    aggrGroupOnRouterTable aggrNumberOnRouter; // groupaddr-routeraddr-numberOfAggr
    aggrGroupOnRouterTable aggrBufferOnRouter; // groupaddr-routeraddr-numberOfAggr

protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override { throw cRuntimeError("this module doesn't handle messages, it runs only in initialize()"); }
};

#endif /* NODE_CONTROLLER_H_ */
