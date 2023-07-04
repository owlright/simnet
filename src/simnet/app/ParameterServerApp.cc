#include "UnicastEchoApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"
#include "simnet/mod/AggPacket_m.h"

class ParameterServerApp : public UnicastEchoApp
{
protected:
    void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void onNewConnectionArrived(IdNumber connId, const Packet* const packet) override;
    virtual void connectionDataArrived(Connection *connection, cMessage *msg) override;
    virtual Packet* createAckPacket(const Packet* const pk) override;
    virtual void finish() override;

protected:
    std::unordered_map<SeqNumber, std::unordered_set<IntAddress> > aggedWorkers;
    std::unordered_map<SeqNumber, int> receivedNumber; // received packets number
    std::unordered_map<SeqNumber, bool> aggedEcns;
    static simsignal_t aggRatioSignal;

private:
    opp_component_ptr<GlobalGroupManager> groupManager;
    int jobid{-1};
    IntAddress groupAddr{INVALID_ADDRESS};
};

Define_Module(ParameterServerApp);

simsignal_t ParameterServerApp::aggRatioSignal = registerSignal("aggRatio");

void ParameterServerApp::initialize(int stage)
{
    UnicastEchoApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        jobid = par("jobId");
        groupAddr = par("groupAddress");
        groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);
        if (groupManager == nullptr)
            EV_WARN << "You may forget to set groupManager." << endl;
    }
}

void ParameterServerApp::handleMessage(cMessage *msg)
{
    auto pk = check_and_cast<AggPacket*>(msg);
    // ASSERT(pk->getJobId() == groupInfo->hostinfo->jobId);
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
    connection->bindRemote(groupAddr, pk->getLocalPort());
}

void ParameterServerApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<AggPacket*>(msg);
    ASSERT(pk->getPacketType() == AGG);
    ASSERT(pk->getJobId() == connection->getConnectionId());
    auto seq = pk->getSeqNumber();
    if (aggedWorkers.find(seq) == aggedWorkers.end())
    {
        aggedWorkers[seq] = std::unordered_set<IntAddress>();
        receivedNumber[seq] = 0;
        aggedEcns[seq] = false;
    }
    aggedEcns.at(seq) |= (pk->getECN() || pk->getEcn());

    EV_DEBUG << pk->getRecord() << endl;
    auto& tmpWorkersRecord = aggedWorkers.at(seq);
    if (pk->getCollision())
        EV_WARN << seq << " hash collision happen" << endl;
    if (pk->getResend())
        EV_WARN << seq << " is a resend packet" << endl;
    for (auto& w:pk->getRecord()) {
        if (tmpWorkersRecord.find(w) != tmpWorkersRecord.end()) {
            ASSERT(pk->getResend());
            EV_WARN << "received a seen packet" << endl;
        }
        else {
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
        aggedEcns.erase(seq);
        EV_DEBUG << "Seq " << seq << " finished." << endl;
    }
    delete pk;
}

Packet* ParameterServerApp::createAckPacket(const Packet* const pk)
{
    char pkname[40];
    sprintf(pkname, "MuACK-%" PRId64 "-seq%" PRId64,
            localAddr, pk->getSeqNumber());
    auto packet = new AggPacket(pkname);
    packet->setSeqNumber(pk->getSeqNumber());
    packet->setKind(MACK);
    packet->setByteLength(64);
    packet->setReceivedBytes(pk->getByteLength());
    packet->setStartTime(pk->getStartTime());
    packet->setQueueTime(pk->getQueueTime());
    packet->setTransmitTime(pk->getTransmitTime());
    if (aggedEcns.at(pk->getSeqNumber())) {
        packet->setECE(true);
    }
    packet->setIsFlowFinished(pk->isFlowFinished());
    check_and_cast<AggPacket*>(packet)->setIsAck(true);
    return packet;
}

void ParameterServerApp::finish()
{

}
