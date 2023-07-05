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

protected:
    void dealWithAggPacket(const cMessage* msg);
    void dealWithIncAggPacket(Connection* connection, const cMessage* msg);
    void dealWithNoIncAggPacket(const cMessage* msg);

private:
    int jobid{-1};
    int numWorkers{0};
    IntAddress groupAddr{INVALID_ADDRESS};
    std::vector<IntAddress> workers;
    std::vector<PortNumber> workerPorts;
};

Define_Module(ParameterServerApp);

simsignal_t ParameterServerApp::aggRatioSignal = registerSignal("aggRatio");

void ParameterServerApp::initialize(int stage)
{
    UnicastEchoApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        jobid = par("jobId");
        numWorkers = par("numWorkers");
        groupAddr = par("groupAddress");
    }
}

void ParameterServerApp::handleMessage(cMessage *msg)
{
    auto pk = check_and_cast<AggPacket*>(msg);
    auto connectionId = pk->getJobId();
    auto it = connections.find(connectionId);
    if (it == connections.end()) {
        onNewConnectionArrived(connectionId, pk);
    }
    connections.at(connectionId)->processMessage(pk);
}

void ParameterServerApp::onNewConnectionArrived(IdNumber connId, const Packet* const pk)
{
    ASSERT(connId == jobid);
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
    ASSERT(pk->getJobId() == connection->getConnectionId());
    dealWithAggPacket(msg);
    if (pk->getAggPolicy() == INC) {
        dealWithIncAggPacket(connection, msg);
    }
    else if (pk->getAggPolicy() == NOINC) {
        dealWithNoIncAggPacket(msg);
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
    packet->setPacketType(MACK);
    packet->setByteLength(64);
    packet->setReceivedBytes(pk->getByteLength());
    packet->setStartTime(pk->getStartTime());
    packet->setQueueTime(pk->getQueueTime());
    packet->setTransmitTime(pk->getTransmitTime());
    if (aggedEcns[pk->getSeqNumber()]) {
        packet->setECE(true);
    }
    packet->setIsFlowFinished(pk->isFlowFinished());
    check_and_cast<AggPacket*>(packet)->setIsAck(true);
    // * set these fields is for no-inc agg packets
    packet->setDestAddr(INVALID_ADDRESS);
    packet->setDestPort(INVALID_PORT);
    return packet;
}

void ParameterServerApp::finish()
{

}

void ParameterServerApp::dealWithAggPacket(const cMessage *msg)
{
    auto pk = check_and_cast<const AggPacket*>(msg);
    ASSERT(pk->getPacketType() == AGG);
    auto seq = pk->getSeqNumber();
    EV_DEBUG << pk->getRecord() << endl;
    // * first packet of the same seq
    if (aggedWorkers.find(seq) == aggedWorkers.end())
    {
        aggedWorkers[seq] = std::unordered_set<IntAddress>();
        receivedNumber[seq] = 0;
        aggedEcns[seq] = false;
    }
    aggedEcns.at(seq) |= pk->getECN();
    auto& tmpWorkersRecord = aggedWorkers.at(seq);
    for (auto& w:pk->getRecord()) {
        if (tmpWorkersRecord.find(w) != tmpWorkersRecord.end()) {
            ASSERT(pk->getResend());
            EV_WARN << "received a seen packet" << endl;
        }
        else {
            receivedNumber[seq] += 1;
            if (workers.size() < numWorkers) {
                workers.push_back(w);
                workerPorts.push_back(pk->getLocalPort());
            }
        }
        tmpWorkersRecord.insert(w);
    }
    EV_DEBUG << "Seq " << seq << " aggregated " << receivedNumber.at(seq) << " packets." << endl;

}

void ParameterServerApp::dealWithNoIncAggPacket(const cMessage *msg)
{
    auto pk = check_and_cast<const AggPacket*>(msg); // FIXME
    auto seq = pk->getSeqNumber();
    auto& tmpWorkersRecord = aggedWorkers.at(seq);
    auto aggedNumber = tmpWorkersRecord.size();
    if (aggedNumber == pk->getWorkerNumber())
    {
        ASSERT(workers.size() == numWorkers);
        ASSERT(aggedNumber == numWorkers);
        for (auto i = 0; i < workers.size(); i++) {
           auto packet = createAckPacket(pk);
           packet->setPacketType(ACK);
           packet->setDestAddr(workers[i]);
           packet->setDestPort(workerPorts[i]);
           connection->send(packet); // ! HACK: this connection is the listening socket
        }
        emit(aggRatioSignal, receivedNumber.at(seq) / double(aggedNumber) );
        aggedWorkers.erase(seq);
        receivedNumber.erase(seq);
        aggedEcns.erase(seq);
        EV_DEBUG << "Seq " << seq << " finished." << endl;
    }
}

void ParameterServerApp::dealWithIncAggPacket(Connection* connection, const cMessage* msg)
{
    auto pk = check_and_cast<const AggUseIncPacket*>(msg);
    auto seq = pk->getSeqNumber();
    aggedEcns.at(seq) |= pk->getEcn();

    if (pk->getCollision())
        EV_WARN << seq << " hash collision happen" << endl;
    if (pk->getResend())
        EV_WARN << seq << " is a resend packet" << endl;

    auto& tmpWorkersRecord = aggedWorkers.at(seq);
    auto aggedNumber = tmpWorkersRecord.size();
    if (aggedNumber == pk->getWorkerNumber())
    {
        ASSERT(workers.size() == numWorkers);
        ASSERT(aggedNumber == numWorkers);
        auto packet = createAckPacket(pk);
        connection->send(packet);
        emit(aggRatioSignal, receivedNumber.at(seq) / double(aggedNumber) );
        aggedWorkers.erase(seq);
        receivedNumber.erase(seq);
        aggedEcns.erase(seq);
        EV_DEBUG << "Seq " << seq << " finished." << endl;
    }
}