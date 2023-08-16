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
    struct AggRecord {
        std::set<IntAddress> workers;
        SeqNumber seqNumber;
        bool ecn;
        bool success{true};
    };
    std::map<SeqNumber, AggRecord> aggRecord;

private:
    int jobid{-1};
    int numWorkers{0};
    int currentRound{0};

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

void ParameterServerApp::onReceivedData(Packet* pk) {

    auto apk = check_and_cast<AggPacket*>(pk);
    ASSERT(apk->getJobId() == jobid);
    auto seq = apk->getSeqNumber();
    //  if (localAddr == 789)
    //      std::cout <<"PS received: " << pk->getName() << endl;
    auto round = apk->getRound();
    if (round > currentRound) {
        EV_DEBUG << "Round: " << round << endl;
        ASSERT(aggRecord.empty());
        currentRound = round;
    }

    if (apk->getFIN())
        totalAggBytes = seq + apk->getByteLength();

    if (aggRecord.find(seq) == aggRecord.end()) {
        // ! we see this packet for the first time(it may be aggregated or the first resend packet)
        aggRecord[seq].seqNumber = getNextSeq();
        incrementNextSeqBy(64);
    }
    auto& record = aggRecord.at(seq);
    if ((apk->getResend() && record.success == true) || apk->getAggPolicy() == NOINC) {
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
        EV_DEBUG << "Seq " << seq << " finished." << endl;
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
                // ! send each worker one ACK
                for (auto& w : agged_workers) {
                    auto apk_ack = createAckPacket(apk);
                    apk_ack->setSeqNumber(record.seqNumber);
                    apk_ack->setPacketType(ACK);
                    apk_ack->setDestAddr(w);
                    apk_ack->setECE(record.ecn);
                    insertTxBuffer(apk_ack);
                }
            }
        }
        aggRecord.erase(seq);
        CongApp::onReceivedData(pk); // ! we see a fully aggregation packet as received a packet
    }
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
    packet->setStartTime(pk->getStartTime());
    packet->setQueueTime(pk->getQueueTime());
    packet->setTransmitTime(pk->getTransmitTime());
    return packet;
}
