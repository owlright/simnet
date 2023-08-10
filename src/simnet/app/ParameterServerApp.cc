#include "CongApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"
#include "simnet/mod/AggPacket_m.h"
#include <functional>
class ParameterServerApp : public CongApp
{
protected:
    virtual void onReceivedData(Packet* pk) override;
    AggPacket* createAckPacket(const AggPacket* pk);

protected:
    void initialize(int stage) override;

protected:
    std::unordered_map<SeqNumber, std::unordered_set<IntAddress> > aggedWorkers;
    std::unordered_map<SeqNumber, int> receivedNumber; // received packets number
    std::unordered_map<SeqNumber, bool> aggedEcns;
    std::unordered_map<SeqNumber, SeqNumber> ackNumber;
    static simsignal_t aggRatioSignal;

protected:
    Connection* getListeningSocket() {return this->connection;};
    void dealWithAggPacket(const AggPacket* msg);
    // return if aggregation is finished
    bool dealWithIncAggPacket(const AggUseIncPacket* pk);
    bool dealWithNoIncAggPacket(Connection* connection, const AggNoIncPacket* msg); // TODO, no inc packet must ack every one through each connection
    void releaseSeq(const SeqNumber& seq);

private:
    int jobid{-1};
    int numWorkers{0};
    int currentRound{0};
    // SeqNumber minAckedSeq{0}; // for debugging
    IntAddress groupAddr{INVALID_ADDRESS};
    std::vector<IntAddress> workers;
    std::vector<PortNumber> workerPorts;
    B aggedBytes{0};
    B totalAggBytes{0};
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
        destAddr = groupAddr;
        auto worker_addrs = cStringTokenizer(par("workers").stringValue(), " ").asIntVector();
        std::for_each(worker_addrs.begin(), worker_addrs.end(), [this](int& n){workers.push_back(n);});
        ASSERT(workers.size() == numWorkers);
    }
}

void ParameterServerApp::onReceivedData(Packet* pkt) {

    auto pk = check_and_cast<AggPacket*>(pkt);
    ASSERT(pk->getJobId() == jobid);
    auto seq = pk->getSeqNumber();
    //  if (localAddr == 789)
    //      std::cout <<"PS received: " << pk->getName() << endl;
    auto round = pk->getRound();
    if (round > currentRound) {
        EV_DEBUG << "Round: " << round << endl;
        ASSERT(aggedWorkers.empty());
        ASSERT(aggedEcns.empty());
        ASSERT(receivedNumber.empty());
        currentRound = round;
    }

    if (pk->getFIN())
        totalAggBytes = seq + pk->getByteLength();

    bool is_agg_finished = false;
    if (ackNumber.find(seq) == ackNumber.end()) {
        ackNumber[seq] = getNextSeq();
        incrementNextSeqBy(64);
    }
    dealWithAggPacket(pk);
    if (pk->getAggPolicy() == INC) {
        is_agg_finished = dealWithIncAggPacket(check_and_cast<const AggUseIncPacket*>(pk));
        if (is_agg_finished && tcpState != LAST_ACK) {
            aggedBytes += pk->getByteLength();
            auto mpk = createAckPacket(pk);
            ASSERT(ackNumber.find(seq) != ackNumber.end());
            mpk->setSeqNumber(ackNumber.at(seq));
            mpk->setPacketType(MACK);
            mpk->setDestAddr(groupAddr);
            mpk->setDestPort(2000);  // TODO this is no use
            mpk->setReceivedNumber(receivedNumber[seq]);
            mpk->setECE(aggedEcns[seq]);
            insertTxBuffer(mpk);
        }
    } else if (pk->getAggPolicy() == NOINC) {
        is_agg_finished = dealWithNoIncAggPacket(connection, check_and_cast<const AggNoIncPacket*>(pk));
        if (is_agg_finished && tcpState != LAST_ACK) {
            aggedBytes += pk->getByteLength();
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
        releaseSeq(seq);
        CongApp::onReceivedData(pkt); // ! we see a fully aggregation packet as received a packet
    } else  {
        delete pk;
    }
}

AggPacket *ParameterServerApp::createAckPacket(const AggPacket* pk)
{
    auto packet = new AggPacket();
    if (aggedBytes == totalAggBytes) {
        if (pk->getFIN() || tcpState == CLOSE_WAIT)
            packet->setFIN(true);
    }
    packet->setAggSeqNumber(pk->getAggSeqNumber());
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

void ParameterServerApp::releaseSeq(const SeqNumber& seq)
{
    EV_DEBUG << "Seq " << seq << " finished." << endl;
    emit(aggRatioSignal, receivedNumber.at(seq) / double(aggedWorkers.size()));
    aggedWorkers.erase(seq);
    receivedNumber.erase(seq);
    aggedEcns.erase(seq);
    ackNumber.erase(seq);
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
