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
    Connection* getListeningSocket() {return this->connection;};
    void dealWithAggPacket(const cMessage* msg);
    void dealWithIncAggPacket(Connection* connection, const cMessage* msg);
    void dealWithNoIncAggPacket(const cMessage* msg);

private:
    int jobid{-1};
    int numWorkers{0};
    int currentRound{0};
    IntAddress groupAddr{INVALID_ADDRESS};
    std::vector<IntAddress> workers;
    std::vector<PortNumber> workerPorts;
    std::unordered_set<SeqNumber> ackedAlready;
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
    auto round = pk->getRound();
    if (round > currentRound) {
        // ! it's very important to clear information left by last Round
        aggedWorkers.clear();
        receivedNumber.clear();
        aggedEcns.clear();
        ackedAlready.clear();
        currentRound = round;
    }
   if (round == currentRound) {
        if (pk->isFlowFinished())
            isAllWorkersFinished = false;
        auto seq = pk->getSeqNumber();
        // ! 1. deal with duplicate resend packets, we must answer these packets to help senders
        // ! remove the retransmitBytes, otherwise sender's cwnd will be occupied and stuck.
        // ! 2. if duplicate resend packets arrive nextRound, we don't need to send ack for them
        // ! since new round begins, sender must receive all packets and reset it's state already
        if (ackedAlready.find(seq) != ackedAlready.end()) {
            // if (localAddr == 789 && pk->getSrcAddr() == 784 && currentRound == 2)
            //     std::cout << "redundant seq " << seq << " from " << pk->getSrcAddr() << endl;
            ASSERT(pk->getResend());
            auto packet = createAckPacket(pk);
            char pkname[40];
            sprintf(pkname, "ACK-%" PRId64 "-seq%" PRId64,
                        localAddr, pk->getSeqNumber());
            packet->setName(pkname);
            packet->setPacketType(ACK);
            packet->setDestAddr(pk->getSrcAddr());
            packet->setDestPort(pk->getLocalPort());

            getListeningSocket()->send(packet); // ! HACK: this connection is the listening socket
            delete pk;
            return;
        }
        dealWithAggPacket(msg);
        if (pk->getAggPolicy() == INC) {
            dealWithIncAggPacket(connection, msg);
        }
        else if (pk->getAggPolicy() == NOINC) {
            dealWithNoIncAggPacket(msg);
        }
    }
    delete pk;
}

Packet* ParameterServerApp::createAckPacket(const Packet* const pkt)
{
    auto pk = check_and_cast<const AggPacket*>(pkt);
    char pkname[40];
    auto seq = pk->getSeqNumber();
    sprintf(pkname, "MuACK-%" PRId64 "-seq%" PRId64,
            localAddr, seq);
    auto packet = new AggPacket(pkname);
    packet->setSeqNumber(seq);
    packet->setPacketType(MACK);
    packet->setByteLength(64);
    packet->setReceivedBytes(pk->getByteLength());
    packet->setReceivedNumber(receivedNumber[seq]);
    packet->setJobId(pk->getJobId());
    packet->setPSAddr(localAddr);
    packet->setRound(pk->getRound());
    packet->setStartTime(pk->getStartTime());
    packet->setQueueTime(pk->getQueueTime());
    packet->setTransmitTime(pk->getTransmitTime());
    if (aggedEcns[seq]) {
        packet->setECE(true);
    }
    packet->setResend(pkt->getResend());
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

    // I left this for future debuging
    //  auto round = pk->getRound();
    //  auto record = pk->getRecord();
    //  auto is_resend = pk->getResend();
//     if (localAddr == 787 && seq == 48000) {
//         std::cout << "PS receives "<< seq << " round "<< round << " resend " << is_resend << " " << record << endl;
//         std::cout << simTime() << endl;
//         // std::cout << "aggregated ";
//         // for (auto& t:aggedWorkers.at(seq)) {
//         //     std::cout << t << " ";
//         // }
//         // std::cout << endl;
//     }

    EV_DEBUG << "Seq " << seq << " aggregated workers: " << pk->getRecord() << endl;
    // * first packet of the same seq
    if (aggedWorkers.find(seq) == aggedWorkers.end())
    {
        aggedWorkers[seq] = std::unordered_set<IntAddress>();
        receivedNumber[seq] = 0;
        aggedEcns[seq] = false;
    }
    aggedEcns.at(seq) |= pk->getECN();
    auto& tmpWorkersRecord = aggedWorkers.at(seq);
    receivedNumber[seq] += 1;
    for (auto& w:pk->getRecord()) {
        if (tmpWorkersRecord.find(w) != tmpWorkersRecord.end()) {
            ASSERT(pk->getResend());
            EV_WARN << "received a seen resend packet" << endl;
        }
        else {
            // receivedNumber[seq] += 1;
            if (workers.size() < numWorkers) { // ! just avoid push_back many times
                workers.push_back(w);
                workerPorts.push_back(pk->getLocalPort());
            }
        }
        tmpWorkersRecord.insert(w);
    }

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
           char pkname[40];
           sprintf(pkname, "ACK-%" PRId64 "-seq%" PRId64,
                      localAddr, pk->getSeqNumber());
           packet->setName(pkname);
           packet->setPacketType(ACK);
           packet->setDestAddr(workers[i]);
           packet->setDestPort(workerPorts[i]);
           getListeningSocket()->send(packet); // ! HACK: this connection is the listening socket
        }
        emit(aggRatioSignal, receivedNumber.at(seq) / double(aggedNumber) );
        aggedWorkers.erase(seq);
        receivedNumber.erase(seq);
        aggedEcns.erase(seq);
        ackedAlready.insert(seq);
        EV_DEBUG << "Seq " << seq << " finished." << endl;
    }
}

void ParameterServerApp::dealWithIncAggPacket(Connection* connection, const cMessage* msg)
{
    Enter_Method("ParameterServerApp::dealWithIncAggPacket");
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
         if (localAddr == 787 && seq==48000) {

             std::cout <<  "Seq " << seq <<" round " << pk->getRound() << " finished." << endl;
         }
        EV_DEBUG << "Seq " << seq << " finished." << endl;
        ackedAlready.insert(seq);
    }
}
