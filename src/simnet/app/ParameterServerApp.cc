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
    std::unordered_map<SeqNumber, int> aggCounters;
    std::unordered_map<SeqNumber, int> receivedNumber;
    static simsignal_t aggRatioSignal;

private:
    GlobalGroupManager* groupManager;
    const GroupHostInfoWithIndex* groupInfo;
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
            groupAddr = groupInfo->hostinfo->groupAddress;
            EV << "server " << localAddr << " accept job " << groupInfo->hostinfo->jobId
               << " groupAddr: " << groupAddr << endl;
        }
        else {
            EV_WARN << "host " << localAddr << " have an idle ATPServer" << endl;
        }
    }
}

void ParameterServerApp::handleMessage(cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    auto connectionId = pk->getDestAddr(); // ! use groupAddr as connectionId
    auto it = connections.find(connectionId);
    if (it == connections.end()) {
        onNewConnectionArrived(connectionId, pk);
    }
    connections.at(connectionId)->processMessage(pk);
}

void ParameterServerApp::onNewConnectionArrived(IdNumber connId, const Packet* const pk)
{
    EV_DEBUG << "Create new connection id " << connId << endl;
    connections[connId] = createConnection(connId);
    auto connection = connections.at(connId);
    connection->bindRemote(connId, pk->getLocalPort());
}

void ParameterServerApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    ASSERT(pk->getKind() == PacketType::DATA);
    ASSERT(pk->getDestAddr() == connection->getConnectionId());
    auto seq = pk->getSeqNumber();
    if (aggCounters.find(seq) == aggCounters.end())
    {
        aggCounters[seq] = 0;
        receivedNumber[seq] = 0;
    }

    auto aggpk = check_and_cast<MTATPPacket*>(pk->decapsulate());
    aggCounters.at(seq) += aggpk->getAggCounter();
    receivedNumber.at(seq) += 1;
    EV_DEBUG << "Seq " << seq << " aggregated " << aggCounters.at(seq) << endl;
    if (aggCounters.at(seq) == aggpk->getWorkerNumber())
    {
        auto packet = createAckPacket(pk);
        connection->send(packet);
        emit(aggRatioSignal, receivedNumber.at(seq) / double(aggCounters.at(seq)) );
        aggCounters.erase(seq);
        receivedNumber.erase(seq);
        EV_DEBUG << "Seq " << seq << " finished." << endl;
        // TODO destroy this connection
    }
    delete pk;
    delete aggpk;
}

Packet* ParameterServerApp::createAckPacket(const Packet* const pk)
{
    auto packet = UnicastEchoApp::createAckPacket(pk);
    char pkname[40];
    sprintf(pkname, "ACK-%lld-to-%lld-seq%lld",
            localAddr, pk->getDestAddr(), pk->getSeqNumber());
    packet->setName(pkname);
    return packet;
}
