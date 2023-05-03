#include "UnicastSenderApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"

class WorkerApp : public UnicastSenderApp
{
protected:
    void initialize(int stage) override;
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
