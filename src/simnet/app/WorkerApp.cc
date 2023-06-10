#include "UnicastSenderApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"

class ATPWorker : public UnicastSenderApp
{
protected:
    void initialize(int stage) override;
    void onFlowStart() override;
    void onFlowStop() override;
    virtual Packet* createDataPacket(B packetBytes) override;

private:
    IntAddress groupAddr{INVALID_ADDRESS};
    int treeIndex{INVALID_ID};
    GlobalGroupManager* groupManager;
};

Define_Module(ATPWorker);

void ATPWorker::initialize(int stage)
{
    UnicastSenderApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);
        if (groupManager==nullptr)
            throw cRuntimeError("WorkerApp::initialize: groupManager not found!");
    }
    // if (stage == INITSTAGE_ASSIGN) {
    //     groupAddr = groupManager->getGroupAddress(localAddr);
    //     if (groupAddr > 0 && groupManager->getGroupRootAddress(groupAddr) != localAddr)
    //     {
    //         destAddr = groupAddr;
    //         treeIndex = groupManager->getTreeIndex(localAddr);

    //         EV << "(sender) groupAddr: " << groupAddr <<" localAddr:" << localAddr  << endl;
    //     }
    // }
}

void ATPWorker::onFlowStart()
{
    UnicastSenderApp::onFlowStart();
    groupManager->reportFlowStart(groupAddr, simTime());
}

void ATPWorker::onFlowStop()
{
    UnicastSenderApp::onFlowStop();
    groupManager->reportFlowStop(groupAddr, simTime());
}

Packet* ATPWorker::createDataPacket(B packetBytes)
{
    char pkname[40];
    sprintf(pkname, " %lld-to-%lld-seq%lld",
            localAddr, destAddr, sentBytes);
    auto pk = new ATPPacket(pkname);
    pk->setKind(DATA);
    pk->setSeqNumber(sentBytes);
    pk->setByteLength(packetBytes);
    pk->setECN(false);
    pk->setStartTime(simTime().dbl());
    pk->setTransmitTime(0);
    pk->setQueueTime(0);
    if (sentBytes == currentFlowSize)
        pk->setIsFlowFinished(true);
    // TODO avoid overflow
    auto seqNumber = pk->getSeqNumber();
    auto jobID = pk->getDestAddr();
    auto seq = reinterpret_cast<uint16_t&>(seqNumber);
    auto jobid = reinterpret_cast<uint16_t&>(jobID);
    pk->setAggregatorIndex(hashAggrIndex(jobid, seq));
    return pk;
}

class TimerWorker : public UnicastSenderApp
{
protected:
    virtual Packet* createDataPacket(B packetBytes) override;
    virtual void initialize(int stage) override;

private:
    IntAddress groupAddr{INVALID_ADDRESS};
    int treeIndex{INVALID_ID};
    GlobalGroupManager* groupManager;

    int numSenders{0};
    intval_t dwellTime{0};
};

Define_Module(TimerWorker);

Packet *TimerWorker::createDataPacket(B packetBytes)
{
    char pkname[40];
    sprintf(pkname, " %lld-to-%lld-seq%lld",
            localAddr, destAddr, sentBytes);
    auto pk = new MTATPPacket(pkname);
    pk->setKind(DATA);
    pk->setSeqNumber(sentBytes);
    pk->setByteLength(packetBytes);
    pk->setECN(false);
    pk->setStartTime(simTime().dbl());
    pk->setTransmitTime(0);
    pk->setQueueTime(0);
    if (sentBytes == currentFlowSize)
        pk->setIsFlowFinished(true);

    pk->setAggCounter(0);
    pk->setWorkerNumber(numSenders);
    pk->setTimer(dwellTime); // in unit ns
    return pk;
}

void TimerWorker::initialize(int stage)
{
    UnicastSenderApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);
        if (groupManager==nullptr)
            throw cRuntimeError("WorkerApp::initialize: groupManager not found!");
        auto t = par("initDwellTime").doubleValueInUnit("s");
        dwellTime = SimTime(t).inUnit(SIMTIME_NS);
    }
    // else if (stage == INITSTAGE_ASSIGN) {
    //     groupAddr = groupManager->getGroupAddress(localAddr);
    //     if (groupAddr > 0 && groupManager->getGroupRootAddress(groupAddr) != localAddr)
    //     {
    //         destAddr = groupAddr;
    //         treeIndex = groupManager->getTreeIndex(localAddr);
    //         EV << "(sender) groupAddr: " << groupAddr <<" localAddr:" << localAddr  << endl;
    //     }
    //     if (groupAddr > 0)
    //         numSenders = groupManager->getSendersNumber(groupAddr);
    // }
}
