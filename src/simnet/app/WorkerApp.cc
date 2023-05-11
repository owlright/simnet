#include "UnicastSenderApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"

class WorkerApp : public UnicastSenderApp
{
protected:
    void initialize(int stage) override;
    void onFlowStart() override;
    void onFlowStop() override;

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
        groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);
        if (groupManager==nullptr)
            throw cRuntimeError("WorkerApp::initialize: groupManager not found!");
    }
    if (stage == INITSTAGE_ASSIGN) {
        groupAddr = groupManager->getGroupAddress(localAddr);
        if (groupAddr > 0 && groupManager->getGroupRootAddress(groupAddr) != localAddr)
        {
            destAddr = groupAddr;
    //        connection->bindRemote(destAddr, destPort); // no need to do this, parent will do this
            treeIndex = groupManager->getTreeIndex(localAddr);

            EV << "(sender) groupAddr: " << groupAddr <<" localAddr:" << localAddr  << endl;
        }
    }
}

void WorkerApp::onFlowStart()
{
    UnicastSenderApp::onFlowStart();
    groupManager->reportFlowStart(groupAddr, simTime());
}

void WorkerApp::onFlowStop()
{
    UnicastSenderApp::onFlowStop();
    groupManager->reportFlowStop(groupAddr, simTime());
}
