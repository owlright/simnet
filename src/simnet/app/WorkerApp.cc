#include "UnicastSenderApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"

class WorkerApp : public UnicastSenderApp
{
protected:
    void initialize(int stage) override;
    void onFlowStart() override;
    void onFlowStop() override;
    virtual Packet* createDataPacket(B packetBytes) override;

protected:
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

Packet *WorkerApp::createDataPacket(B packetBytes)
{
    char pkname[40];
    sprintf(pkname, " %lld-to-%lld-seq%lld",
            localAddr, destAddr, sentBytes);
    auto pk = UnicastSenderApp::createDataPacket(packetBytes);
    pk->setName(pkname);
    return pk;
}

class TimerWorkerApp : public WorkerApp
{
protected:
    virtual Packet* createDataPacket(B packetBytes) override;
    virtual void initialize(int stage) override;

private:
    int numSenders{0};
    intval_t dwellTime{0};
};

Define_Module(TimerWorkerApp);

Packet *TimerWorkerApp::createDataPacket(B packetBytes)
{
    auto pk = WorkerApp::createDataPacket(packetBytes);
    pk->setAggCounter(0);
    pk->setAggNumber(numSenders);
    pk->setTimer(dwellTime); // in unit ns
    return pk;
}

void TimerWorkerApp::initialize(int stage)
{
    WorkerApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        auto t = par("initDwellTime").doubleValueInUnit("s");
        dwellTime = SimTime(t).inUnit(SIMTIME_NS);
    }
    else if (stage == INITSTAGE_ASSIGN) {
        if (groupAddr > 0)
            numSenders = groupManager->getSendersNumber(groupAddr);
    }
}
