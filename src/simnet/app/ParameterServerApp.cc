#include "UnicastEchoApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"

class ParameterServerApp : public UnicastEchoApp
{
protected:
    void initialize(int stage) override;
private:
    GlobalGroupManager* groupManager;
    IntAddress groupAddr{INVALID_ADDRESS};
    int treeIndex{INVALID_ID};
};

Define_Module(ParameterServerApp);

void ParameterServerApp::initialize(int stage)
{
    UnicastEchoApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        groupManager = getModuleFromPar<GlobalGroupManager>(par("groupManager"), this);
        if (groupManager==nullptr)
            throw cRuntimeError("ParameterServerApp::initialize: groupManager not found!");
    }
    if (stage == INITSTAGE_ASSIGN) {
        groupAddr = groupManager->getGroupAddress(myAddr);
        treeIndex = groupManager->getTreeIndex(myAddr);
        EV << "groupAddr: " << groupAddr << endl;
    }
}
