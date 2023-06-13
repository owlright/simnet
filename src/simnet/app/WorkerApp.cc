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
    GlobalGroupManager* groupManager;
    const GroupInfoWithIndex* groupInfo;
    int jobId;
};

Define_Module(ATPWorker);

void ATPWorker::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        isUnicastSender = false;
        groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);
        if (groupManager==nullptr) // sometimes for quick debug
            EV_WARN << "You may forget to set groupManager." << endl;
    }
    else if (stage == INITSTAGE_ASSIGN) {
        if (groupManager==nullptr)
            throw cRuntimeError("WorkerApp::initialize: groupManager not found!");
        groupInfo = groupManager->getGroupHostInfo(localAddr);
        if (groupInfo != nullptr && groupInfo->isWorker) {
            jobId = groupInfo->hostinfo->jobId;
            EV << "host " << localAddr << " accept job " << jobId;
            EV << " possible PSes: ";
            EV << groupInfo->hostinfo->PSes << endl;
            // ! TODO FIXME only send to a single sever
            // ! Split Worker App from UnicastApp or Let UnicastApp have multiple Paramter servers ?
            destAddr = groupInfo->hostinfo->PSes.at(0);
        }
        else {
            EV_WARN << "host " << localAddr << " have an idle ATPWorker" << endl;
        }

    }
    UnicastSenderApp::initialize(stage);
}

void ATPWorker::onFlowStart()
{
    UnicastSenderApp::onFlowStart();
    groupManager->reportFlowStart(jobId, simTime());
}

void ATPWorker::onFlowStop()
{
    UnicastSenderApp::onFlowStop();
    groupManager->reportFlowStop(jobId, simTime());
}

Packet* ATPWorker::createDataPacket(B packetBytes)
{
    // ASSERT(destAddr == -1); // destAddr is useless
    IntAddress dest = groupInfo->hostinfo->PSes.at(0); // TODO use more PSes
    char pkname[40];
    sprintf(pkname, " %lld-to-%lld-seq%lld",
            localAddr, dest, sentBytes);
    auto pk = new ATPPacket(pkname);
    pk->setJobId(jobId);
    pk->setDestAddr(dest);
    pk->setSeqNumber(sentBytes);
    pk->setByteLength(packetBytes);
    pk->setECN(false);
    pk->setStartTime(simTime().dbl());
    pk->setTransmitTime(0);
    pk->setQueueTime(0);
    if (sentBytes == currentFlowSize)
        pk->setIsFlowFinished(true);

    auto seqNumber = pk->getSeqNumber();
    auto jobID = pk->getJobId();
    // TODO avoid overflow
    auto seq = reinterpret_cast<uint16_t&>(seqNumber);
    auto jobid = reinterpret_cast<uint16_t&>(jobID);
    auto agtrIndex = hashAggrIndex(jobid, seq);
    EV_DEBUG << "aggregator index: " << agtrIndex << endl;
    pk->setAggregatorIndex(agtrIndex);
    pk->setBitmap0(groupInfo->switchinfo->bitmap0);
    pk->setBitmap1(groupInfo->switchinfo->bitmap1);
    pk->setFanIndegree0(groupInfo->switchinfo->fanIndegree0);
    pk->setFanIndegree1(groupInfo->switchinfo->fanIndegree1);
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

    if (stage == INITSTAGE_LOCAL) {
        isUnicastSender = false;
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
    UnicastSenderApp::initialize(stage);
}
