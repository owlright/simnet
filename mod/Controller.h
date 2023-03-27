#ifndef MOD_CONTROLLER_H_
#define MOD_CONTROLLER_H_

#include <omnetpp.h>
#include "../common/Defs.h"
#include <unordered_map>

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

public:
    int askForDest(int srcAddr) const;

private:
    cTopology *topo;
    std::vector<int> nodeMap; // nodeMap[node-index-in-topo] = node's address
    std::vector<int> hosts; // store host's index-in-topo not address!
    std::unordered_map<int, int> odMap; // odMap[node's address] = dest's address for assign traffic pattern
    std::map<int, std::vector<int>> aggrgroup;
    aggrGroupOnRouterTable aggrNumberOnRouter; // groupaddr-routeraddr-numberOfAggr
    aggrGroupOnRouterTable aggrBufferOnRouter; // groupaddr-routeraddr-numberOfAggr
private:
    void setNodes(const cTopology *topo);
    void prepareTrafficPattern(const std::string& name);
    void prepareAggrGroup(const std::string& name);

protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override { throw cRuntimeError("this module doesn't handle messages, it runs only in initialize()"); }
};

#endif /* MOD_CONTROLLER_H_ */
