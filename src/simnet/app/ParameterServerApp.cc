#include "CongApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"
#include "simnet/mod/AggPacket_m.h"
#include <functional>
class ParameterServerApp : public CongApp
{

protected:
    void initialize(int stage) override;

protected:
    virtual void onReceivedNewPacket(Packet* pk) override;
    virtual void resend(TxItem& item) override;

protected:
    struct AggRecord {
        std::set<IntAddress> workers;
        SeqNumber seqNumber;
        SeqNumber askedSeqNumber;
        bool ecn;
        bool success{true};
    };
    std::map<SeqNumber, AggRecord> aggRecord;

private:
    AggPacket* createAckPacket(const AggPacket* pk);

private:
    int jobid{-1};
    int numWorkers{0};

    IntAddress groupAddr{INVALID_ADDRESS};
    std::set<IntAddress> workers;

    B aggedBytes{0};
    B totalAggBytes{0};
};

Define_Module(ParameterServerApp);

void ParameterServerApp::initialize(int stage)
{
    CongApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        jobid = par("jobId");
        numWorkers = par("numWorkers");
        groupAddr = par("groupAddress");
        destAddr = groupAddr;
        auto worker_addrs = cStringTokenizer(par("workers").stringValue(), " ").asIntVector();
        std::for_each(worker_addrs.begin(), worker_addrs.end(), [this](int& n){workers.insert(n);});
        ASSERT(workers.size() == numWorkers);
    }
}

void ParameterServerApp::onReceivedNewPacket(Packet* pk)
{

    auto apk = check_and_cast<AggPacket*>(pk);
    ASSERT(apk->getJobId() == jobid);
    auto seq = apk->getSeqNumber();
    auto round = apk->getRound();
    if (localAddr == 398 && apk->getAckNumber() >= 20224)
        std::cout << "now: " << simTime() <<" "<< pk->getName() << endl;
    if (round > currentRound) {
        EV_DEBUG << "Round: " << round << endl;
        ASSERT(aggRecord.empty());
        currentRound = round;
    }

    if (apk->getFIN())
        totalAggBytes = seq + apk->getByteLength();

    auto arrivedAckNumber = pk->getAckNumber();
    if (aggRecord.find(seq) == aggRecord.end()) {
        // ! we see this packet for the first time(it may be aggregated or the first resend packet)
        aggRecord[seq].seqNumber = getNextSeq();
        aggRecord[seq].askedSeqNumber = arrivedAckNumber;
        incrementNextSeqBy(64);
    }
    else {
        if (arrivedAckNumber < aggRecord.at(seq).askedSeqNumber) {
            aggRecord.at(seq).askedSeqNumber = arrivedAckNumber;
        }
    }
    auto& record = aggRecord.at(seq);
    if ((apk->getResend() && record.success == true) || apk->getAggPolicy() == NOINC) {
        // ! we see the resend packet for the first time, clear the record
        record.success = false;
        record.workers.clear();
    }
    record.ecn |= apk->getECN();
    auto& agged_workers = record.workers;
    auto incoming_workers = apk->getRecord();
    for (auto& w : incoming_workers) {
        agged_workers.insert(w);
    }
    if (agged_workers.size() == numWorkers) {
        if (localAddr == 398)
        std::cout << "Round " << currentRound << " Seq " << seq << " finished. " << getNextAskedSeq() << endl;
        ASSERT(workers == agged_workers);
        if (tcpState == OPEN) {
            // ! after we send FIN, we are in state CLOSE_WAIT,
            // ! we will receieve ACK to FIN, do not answer it.
            aggedBytes += pk->getByteLength();
            if (record.success) {
                // ! send a multicast ACK
                auto mpk = createAckPacket(apk);
                mpk->setSeqNumber(record.seqNumber);
                mpk->setPacketType(MACK);
                mpk->setDestAddr(groupAddr);
                mpk->setECE(record.ecn);
                insertTxBuffer(mpk);
            }
            else {
                auto apk_ack = createAckPacket(apk);
                apk_ack->setSeqNumber(record.seqNumber);
                apk_ack->setPacketType(ACK);
                apk_ack->setECE(record.ecn);
                insertTxBuffer(apk_ack);
                auto& item = txBuffer.at(record.seqNumber);
                // ! send each worker one ACK
                std::vector<IntAddress> tmp(agged_workers.begin(), agged_workers.end());
                item.destAddresses = std::move(tmp);
            }
        }
        aggRecord.erase(seq);
        CongApp::onReceivedNewPacket(pk); // ! we see a fully aggregation packet as received a packet
    }
    else {
        ASSERT(pk->getResend());
        delete pk;
    }
}

void ParameterServerApp::resend(TxItem& item)
{
    if (localAddr == 398)
        std::cout << "PS resend " << item.seq << std::endl;
    std::vector<IntAddress> tmp(workers.begin(), workers.end());
    item.pkt->setPacketType(ACK);
    item.destAddresses = std::move(tmp);
    CongApp::resend(item);
}

AggPacket *ParameterServerApp::createAckPacket(const AggPacket* pk)
{
    auto packet = new AggPacket();
    if (aggedBytes == totalAggBytes) {
        if (pk->getFIN() || tcpState == CLOSE_WAIT)
            packet->setFIN(true);
    }

    packet->setByteLength(64);
    packet->setDestPort(2000);
    // copy pk's fields
    packet->setJobId(pk->getJobId());
    packet->setAggSeqNumber(pk->getAggSeqNumber());
    packet->setAggregatorIndex(pk->getAggregatorIndex());
    packet->setRound(pk->getRound());
    return packet;
}
