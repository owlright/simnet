#include "CongApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"
#include "simnet/mod/AggPacket_m.h"
#include <functional>
class ParameterServerApp : public CongApp
{
protected:
    // virtual void handleMessage(cMessage *msg) override;
    // void onNewConnectionArrived(IdNumber connId, const Packet* const packet);
    // virtual void connectionDataArrived(Connection *connection, cMessage *msg) override;
    // virtual void onReceivedAck(const Packet* pk) override;
    virtual void onReceivedData(const Packet* pk) override;
    AggPacket* createAckPacket(const AggPacket* pk);
    // virtual void finish() override;

protected:
    void initialize(int stage) override;

protected:
    std::unordered_map<SeqNumber, std::unordered_set<IntAddress> > aggedWorkers;
    std::unordered_map<SeqNumber, int> receivedNumber; // received packets number
    std::unordered_map<SeqNumber, bool> aggedEcns;
    static simsignal_t aggRatioSignal;

protected:
    Connection* getListeningSocket() {return this->connection;};
    void dealWithAggPacket(const AggPacket* msg);
    // return if aggregation is finished
    bool dealWithIncAggPacket(const AggUseIncPacket* pk);
    bool dealWithNoIncAggPacket(Connection* connection, const AggNoIncPacket* msg); // TODO, no inc packet must ack every one through each connection

private:
    int jobid{-1};
    int numWorkers{0};
    int currentRound{0};
    // SeqNumber minAckedSeq{0}; // for debugging
    IntAddress groupAddr{INVALID_ADDRESS};
    std::vector<IntAddress> workers;
    std::vector<PortNumber> workerPorts;
    // std::map<SeqNumber, TxItem> lastTxBuffer;
    // std::unordered_set<IntAddress> last_round_finished_workers;
    // bool isAllWorkersFinished{false};
};

Define_Module(ParameterServerApp);

simsignal_t ParameterServerApp::aggRatioSignal = registerSignal("aggRatio");

void ParameterServerApp::initialize(int stage)
{
    CongApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        jobid = par("jobId");
        numWorkers = par("numWorkers");
        groupAddr = par("groupAddress");
        auto worker_addrs = cStringTokenizer(par("workers").stringValue(), " ").asIntVector();
        std::for_each(worker_addrs.begin(), worker_addrs.end(), [this](int& n){workers.push_back(n);});
        ASSERT(workers.size() == numWorkers);
    }
}

// void ParameterServerApp::handleMessage(cMessage *msg)
// {
//     auto pk = check_and_cast<AggPacket*>(msg);
//     auto connectionId = pk->getJobId();
//     auto it = connections.find(connectionId);
//     if (it == connections.end()) {
//         onNewConnectionArrived(connectionId, pk);
//     }
//     connections.at(connectionId)->processMessage(pk);
// }

// void ParameterServerApp::onNewConnectionArrived(IdNumber connId, const Packet* const pk)
// {
//     ASSERT(connId == jobid);
//     if (connections.size() > 1)
//         throw cRuntimeError("a PS only serves one group");
//     EV_DEBUG << "Create new connection id by jobId " << connId << endl;
//     connections[connId] = createConnection(connId);
//     auto connection = connections.at(connId);
//     connection->bindRemote(groupAddr, pk->getLocalPort());
// }

// void ParameterServerApp::connectionDataArrived(Connection *connection, cMessage *msg)
// {

//     CongApp::connectionDataArrived(connection, msg);
// }

// void ParameterServerApp::onReceivedAck(const Packet *pkt)
// {

//     CongApp::onReceivedAck(pk);
// }

void ParameterServerApp::onReceivedData(const Packet* pkt)
{
    auto pk = check_and_cast<const AggPacket*>(pkt);
    ASSERT(pk->getJobId() == jobid);
    auto round = pk->getRound();
    auto seq = pk->getSeqNumber();

    // if (!isAllWorkersFinished && round < currentRound) {
    //     auto record = pk->getRecord();
    //     for (auto& worker : record) {
    //         last_round_finished_workers.insert(worker);
    //     }
    //     if (last_round_finished_workers.size() == numWorkers) {
    //         lastTxBuffer.clear();
    //         isAllWorkersFinished = true;
    //     }
    // }
    if (round > currentRound) {
        ASSERT(aggedWorkers.empty());
        ASSERT(aggedEcns.empty());
        ASSERT(receivedNumber.empty());
    //     // ! it's very important to clear information left by last Round
    //     // lastTxBuffer = getTxBufferCopy();
    //     // minAckedSeq = 0;
    //     aggedWorkers.clear();
    //     receivedNumber.clear();
    //     aggedEcns.clear();
    //     // txBuffer.clear();
    //     currentRound = round;
    }
    // if (round == currentRound) {
        // if (pk->isFlowFinished()) isAllWorkersFinished = false;
        // auto seq = pk->getSeqNumber();
        // // ! 1. deal with duplicate resend packets, we must answer these packets to help senders
        // // ! remove the retransmitBytes, otherwise sender's cwnd will be occupied and stuck.
        // // ! 2. if duplicate resend packets arrive nextRound, we don't need to send ack for them
        // // ! since new round begins, sender must have already received all packets and reset it's state.
        // if (txBuffer.find(seq) != txBuffer.end()) {
        //     // if (localAddr == 789 && pk->getSrcAddr() == 784 && currentRound == 2)
        //     //     std::cout << "redundant seq " << seq << " from " << pk->getSrcAddr() << endl;
        //     ASSERT(pk->getResend());
        //     auto packet = createAckPacket(pk);
        //     char pkname[40];
        //     sprintf(pkname, "ACK-%" PRId64 "-seq%" PRId64, localAddr, pk->getSeqNumber());
        //     packet->setName(pkname);
        //     packet->setPacketType(ACK);
        //     packet->setDestAddr(pk->getSrcAddr());
        //     packet->setDestPort(pk->getLocalPort());

        //     getListeningSocket()->send(packet);  // ! HACK: this connection is the listening socket
        //     delete pk;
        //     return;
        // }
        bool is_agg_finished = false;
        dealWithAggPacket(pk);
        if (pk->getAggPolicy() == INC) {
            is_agg_finished = dealWithIncAggPacket(check_and_cast<const AggUseIncPacket*>(pk));
            if (is_agg_finished) {
                auto mpk = createAckPacket(pk);
                mpk->setReceivedNumber(receivedNumber[seq]);
                mpk->setECE(aggedEcns[seq]);
                insertTxBuffer(mpk);
            }
        } else if (pk->getAggPolicy() == NOINC) {
            is_agg_finished = dealWithNoIncAggPacket(connection, check_and_cast<const AggNoIncPacket*>(pk));
            if (is_agg_finished) {
                for (auto i = 0; i < workers.size(); i++) {
                    auto packet = createAckPacket(pk);
                    char pkname[40];
                    sprintf(pkname, "ACK-%" PRId64 "-seq%" PRId64, localAddr, pk->getSeqNumber());
                    packet->setName(pkname);
                    packet->setPacketType(ACK);
                    packet->setDestAddr(workers[i]);
                    packet->setDestPort(workerPorts[i]);
                    insertTxBuffer(packet);
                    // getListeningSocket()->send(packet);  // ! HACK: this connection is the listening socket
                }
            }
        }

        if (is_agg_finished) {
            EV_DEBUG << "Seq " << seq << " finished." << endl;
            emit(aggRatioSignal, receivedNumber.at(seq) / double(aggedWorkers.size()) );
            aggedWorkers.erase(seq);
            receivedNumber.erase(seq);
            aggedEcns.erase(seq);
        }
    // } else {
        // // ! some senders send next round too early, the info is updated upabove.
        // // ! Again, we answer only the first-batch resend packets and ingore the others.
        // if (lastTxBuffer.find(pk->getSeqNumber()) != lastTxBuffer.end()) {
        //     ASSERT(pk->getResend());
        //     auto packet = createAckPacket(pk);
        //     char pkname[40];
        //     sprintf(pkname, "ACK-%" PRId64 "-seq%" PRId64, localAddr, pk->getSeqNumber());
        //     packet->setName(pkname);
        //     packet->setPacketType(ACK);
        //     packet->setDestAddr(pk->getSrcAddr());
        //     packet->setDestPort(pk->getLocalPort());
        //     insertTxBuffer(packet);
        //     // getListeningSocket()->send(packet);  // ! HACK: this connection is the listening socket
        //     // delete pk;
        //     return;
        // }
    // }
}

AggPacket *ParameterServerApp::createAckPacket(const AggPacket* pk)
{
    char pkname[40];
    // auto seq = pk->getSeqNumber();
    sprintf(pkname, "MuACK-%" PRId64 "-seq%" PRId64,
            localAddr, getNextSeq());

    auto packet = new AggPacket(pkname);
    // packet->setSeqNumber(seq);
    packet->setPacketType(MACK);
    packet->setByteLength(64);
    // packet->setReceivedBytes(pk->getByteLength());
    // packet->setReceivedNumber(receivedNumber[seq]);
    packet->setJobId(pk->getJobId());
    packet->setPSAddr(localAddr);
    packet->setRound(pk->getRound());
    packet->setStartTime(pk->getStartTime());
    packet->setQueueTime(pk->getQueueTime());
    packet->setTransmitTime(pk->getTransmitTime());
    // if (aggedEcns.find(seq) != aggedEcns.end()) {
    //     packet->setECE(aggedEcns.at(seq));
    // }
    // else { // ! this is an ack to a single host
    //     packet->setECE(pk->getECN());
    // }
    packet->setIsAck(true);
    // * set these fields is for no-inc agg packets
    packet->setDestAddr(INVALID_ADDRESS);
    packet->setDestPort(INVALID_PORT);
    return packet;
}

void ParameterServerApp::dealWithAggPacket(const AggPacket *pk)
{
    auto seq = pk->getSeqNumber();
    Enter_Method("ParameterServerApp::dealWithAggPacket");

    if (pk->getResend())
        EV_WARN << seq << " is a resend packet" << endl;

    // // debug
    // auto round = pk->getRound();
    // auto record = pk->getRecord();
    // auto is_resend = pk->getResend();

    //  if (localAddr == 786 && seq == 232000) {
    //      std::cout << simTime() << " PS receives "<< seq << " round "<< round
    //              << " resend " << is_resend << " " << record << " max ack so far:" << minAckedSeq << endl;
    //  }

    EV_DEBUG << "Seq " << seq << " aggregated workers: " << pk->getRecord() << endl;
    // * first packet of the same seq, make a record
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
        // else {
        //     // * this infomation is for no inc agg workers
        //     if (workers.size() < numWorkers) { // ! just avoid push_back many times
        //         workers.push_back(w);
        //         workerPorts.push_back(pk->getLocalPort());
        //     }
        // }
        tmpWorkersRecord.insert(w);
    }
}

bool ParameterServerApp::dealWithNoIncAggPacket(Connection* connection, const AggNoIncPacket *pk)
{
    auto seq = pk->getSeqNumber();
    auto& tmpWorkersRecord = aggedWorkers.at(seq);
    auto aggedNumber = tmpWorkersRecord.size();
    if (aggedNumber == pk->getWorkerNumber())
    {
        ASSERT(aggedNumber == numWorkers);
        EV_DEBUG << "Seq " << seq << " finished." << endl;
        return true;
    }
    return false;
}

bool ParameterServerApp::dealWithIncAggPacket(const AggUseIncPacket* pk)
{
    Enter_Method("ParameterServerApp::dealWithIncAggPacket");
    auto seq = pk->getSeqNumber();
    if (pk->getCollision())
        EV_WARN << seq << " hash collision happen" << endl;

    auto& tmpWorkersRecord = aggedWorkers.at(seq);
    auto aggedNumber = tmpWorkersRecord.size();
    if (aggedNumber == pk->getWorkerNumber()) // * aggregation is finished
    {
        ASSERT(aggedNumber == numWorkers);
        return true;
    }
    return false;
}
