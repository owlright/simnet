#ifndef NODE_CONTROLLER_H_
#define NODE_CONTROLLER_H_

#include <omnetpp/csimplemodule.h>
#include "../common/Defs.h"
using namespace omnetpp;

class Controller: public omnetpp::cSimpleModule {
private:
    typedef std::map<int, std::map<int,int> > aggrGroupOnRouterTable;
public:
    int getRoute(cModule* from, int to) const;
    int getGroupInfo(int groupid, int routerid, const aggrGroupOnRouterTable& table) const;
    int getGroupAggrNum(int groupid, int routerid) const;
    int getGroupAggrBuffer(int groupid, int routerid) const;
    int getAggrSendersNum(int groupid) const;
    void updateAggrGroup(int groupid, int senderAddr);
    bool isAggrGroupOnRouter(int groupid, int routerid) const;
    bool isGroupTarget(int myAddress) const;
    Controller();
    virtual ~Controller();
private:
    cTopology *topo;
    std::vector<int> nodeMap;
    std::map<int, std::vector<int>> aggrgroup;
    aggrGroupOnRouterTable aggrNumberOnRouter; // groupaddr-routeraddr-numberOfAggr
    aggrGroupOnRouterTable aggrBufferOnRouter; // groupaddr-routeraddr-numberOfAggr
private:
    void setNodes(const cTopology *topo);
protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override { throw cRuntimeError("this module doesn't handle messages, it runs only in initialize()"); }
};

#endif /* NODE_CONTROLLER_H_ */
