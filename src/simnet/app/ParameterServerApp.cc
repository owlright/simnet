#include "UnicastEchoApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"

class ParameterServerApp : public UnicastEchoApp
{
protected:
    void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void onNewConnectionArrived(IdNumber connId, const Packet* const packet) override;
    virtual void connectionDataArrived(Connection *connection, cMessage *msg) override;
    virtual Packet* createAckPacket(const Packet* const pk) override;

protected:
    std::unordered_map<SeqNumber, std::unordered_set<IntAddress> > aggedWorkers;
    std::unordered_map<SeqNumber, int> receivedNumber; // received packets number
    static simsignal_t aggRatioSignal;

private:
    GlobalGroupManager* groupManager;
    const GroupInfoWithIndex* groupInfo;
    IntAddress groupAddr{INVALID_ADDRESS};
};

Define_Module(ParameterServerApp);

simsignal_t ParameterServerApp::aggRatioSignal = registerSignal("aggRatio");

void ParameterServerApp::initialize(int stage)
{
    UnicastEchoApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);
        if (groupManager == nullptr)
            EV_WARN << "You may forget to set groupManager." << endl;
    }
    if (stage == INITSTAGE_ASSIGN) {
        if (groupManager==nullptr)
            throw cRuntimeError("WorkerApp::initialize: groupManager not found!");
        groupInfo = groupManager->getGroupHostInfo(localAddr);
        if (groupInfo != nullptr && !groupInfo->isWorker) {
            EV << "server " << localAddr << " accept job " << groupInfo->hostinfo->jobId;
            EV << " multicast addr " << groupInfo->hostinfo->multicastAddress << endl;
        }
        else {
            EV_WARN << "host " << localAddr << " have an idle ATPServer" << endl;
        }
    }
}

void ParameterServerApp::handleMessage(cMessage *msg)
{
    auto pk = check_and_cast<AggPacket*>(msg);
    ASSERT(pk->getJobId() == groupInfo->hostinfo->jobId);
    auto connectionId = pk->getJobId();
    auto it = connections.find(connectionId);
    if (it == connections.end()) {
        onNewConnectionArrived(connectionId, pk);
    }
    connections.at(connectionId)->processMessage(pk);
}

void ParameterServerApp::onNewConnectionArrived(IdNumber connId, const Packet* const pk)
{
    if (connections.size() > 1)
        throw cRuntimeError("a PS only serves one group");
    EV_DEBUG << "Create new connection id by jobId " << connId << endl;
    connections[connId] = createConnection(connId);
    auto connection = connections.at(connId);
    connection->bindRemote(groupInfo->hostinfo->multicastAddress, pk->getLocalPort());
}

void ParameterServerApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<ATPPacket*>(msg);
    ASSERT(pk->getPacketType() == AGG);
    ASSERT(pk->getJobId() == connection->getConnectionId());
    auto seq = pk->getSeqNumber();
    if (aggedWorkers.find(seq) == aggedWorkers.end())
    {
        aggedWorkers[seq] = std::unordered_set<IntAddress>();
        receivedNumber[seq] = 0;
    }
    EV_DEBUG << pk->getRecord() << endl;
    auto& tmpWorkersRecord = aggedWorkers.at(seq);
    for (auto& w:pk->getRecord()) {
        if (tmpWorkersRecord.find(w) != tmpWorkersRecord.end()) {
            ASSERT(pk->getResend());
            EV_WARN << "received a resend packet" << endl;
        }
        else {
            if (pk->getCollision())
                EV_WARN << "hash collision happen" << endl;
            receivedNumber[seq] += 1;
        }

        tmpWorkersRecord.insert(w);
    }
    EV_DEBUG << "Seq " << seq << " aggregated " << receivedNumber[seq] << " packets." << endl;
    auto aggedNumber = tmpWorkersRecord.size();
    if (aggedNumber == pk->getWorkerNumber())
    {
        auto packet = createAckPacket(pk);
        connection->send(packet);
        emit(aggRatioSignal, receivedNumber.at(seq) / double(aggedNumber) );
        aggedWorkers.erase(seq);
        receivedNumber.erase(seq);
        EV_DEBUG << "Seq " << seq << " finished." << endl;
    }
    delete pk;
}

Packet* ParameterServerApp::createAckPacket(const Packet* const pk)
{
    char pkname[40];
    sprintf(pkname, "MuACK-%lld-to-%lld-seq%lld",
            localAddr, pk->getDestAddr(), pk->getSeqNumber());
    auto packet = new AggPacket(pkname);
    packet->setSeqNumber(pk->getSeqNumber());
    packet->setKind(PacketType::ACK);
    packet->setByteLength(64);
    packet->setReceivedBytes(pk->getByteLength());
    packet->setStartTime(pk->getStartTime());
    packet->setQueueTime(pk->getQueueTime());
    packet->setTransmitTime(pk->getTransmitTime());
    if (pk->getECN()) {
        packet->setECE(true);
    }
    packet->setIsFlowFinished(pk->isFlowFinished());
    check_and_cast<AggPacket*>(packet)->setIsAck(true);
    return packet;
}
