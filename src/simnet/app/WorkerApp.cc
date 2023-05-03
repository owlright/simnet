#include "UnicastSenderApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"

class WorkerApp : public UnicastSenderApp
{
protected:
    void initialize(int stage) override;
    cMessage* makeDataPacket(Connection *connection, Packet *pk) override;

private:
    IntAddress groupAddr{INVALID_ADDRESS};
    int treeIndex{INVALID_ID};
    GlobalGroupManager* groupManager;
};

Define_Module(WorkerApp);

void WorkerApp::initialize(int stage)
{
    UnicastSenderApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        groupManager = getModuleFromPar<GlobalGroupManager>(par("groupManager"), this);
        // if (groupManager==nullptr)
        //     throw cRuntimeError("WorkerApp::initialize: groupManager not found!");
    }
    if (stage == INITSTAGE_ASSIGN) {
        groupAddr = groupManager->getGroupAddress(myAddr);
        destAddr = groupAddr;
        treeIndex = groupManager->getTreeIndex(myAddr);
        EV << "groupAddr: " << groupAddr << endl;
    }
}

cMessage* WorkerApp::makeDataPacket(Connection *connection, Packet *pk)
{
    char pkname[40];
    sprintf(pkname, "Group-%" PRId64 "-to-%" PRId64 "-seq%" PRId64, myAddr, destAddr, sentBytes);
    pk->setName(pkname);
    pk->setECN(false);
    return pk;
}