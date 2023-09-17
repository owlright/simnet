#include "CongApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"
#include "simnet/mod/AggPacket_m.h"
#include <functional>
class ParameterServerApp : public CongApp
{
protected:
    virtual void finish() override;

protected:
    void initialize(int stage) override;

protected:
    virtual void onReceivedNewPacket(Packet* pk) override;
    virtual void resend(TxItem& item) override;
    virtual void onReceivedDuplicatedPacket(Packet* pk) override;
    virtual Packet* createDataPacket() override;

protected:
    struct AggRecord {
        std::set<IntAddress> workers;
        SeqNumber askedSeqNumber;
        Packet* pk;
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
    SeqNumber currSeq{-1}; // ! current seqNumber for aggregation
    std::map<SeqNumber, std::set<IntAddress>> oldPktWithNewAckSeq;
};

Define_Module(ParameterServerApp);

void ParameterServerApp::finish()
{
    CongApp::finish();
    std::cout << jobid << " PS " << destAddr << " retransmit "<< resentBytes << endl;
}


void ParameterServerApp::initialize(int stage) {
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
    if (round > currentRound) {
        EV_DEBUG << "Round: " << round << endl;
        if (!txBuffer.empty()) {
            throw cRuntimeError("PS %d txBuffer not cleared yet.", round);
        }
        ASSERT(aggRecord.empty());
        currentRound = round;
    }

    if (apk->getFIN())
        totalAggBytes = seq + apk->getByteLength();

    auto arrivedAckNumber = pk->getAckNumber();
    if (aggRecord.find(seq) == aggRecord.end()) {
        // ! we see this packet for the first time(it may be aggregated or the first resend packet)
        aggRecord[seq].askedSeqNumber = arrivedAckNumber;
    }
    else {
        // ! this is for resend packets aggregation, store the smallest ackNumber
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

    if (apk->getCollision()) {
        record.askedSeqNumber = INT64_MAX;
        delete apk;
        return;
    }
    auto& agged_workers = record.workers;
    auto incoming_workers = apk->getRecord();
    agged_workers.insert(incoming_workers.begin(), incoming_workers.end());
    if (agged_workers.size() == numWorkers) {
        EV_DEBUG << GREEN << "Round " << currentRound << " Seq " << seq  << " finished. " << getNextAskedSeq() << ENDC;
        ASSERT(workers == agged_workers);
        aggedBytes += pk->getByteLength();
        currSeq = pk->getSeqNumber();
        auto mpk = createAckPacket(apk);
        mpk->setECE(record.ecn);
        if (record.success) {
            // ! send a multicast ACK
            mpk->setPacketType(MACK);
            mpk->setDestAddr(groupAddr);
        }
        else {
            mpk->setPacketType(ACK);
        }
        mpk->setAckNumber(record.askedSeqNumber);
        record.pk = mpk;
        CongApp::onReceivedNewPacket(pk); // ! we see a fully aggregation packet as received a packet
    }
    else {
        ASSERT(pk->getResend());
        currSeq = -1;
        delete pk;
    }
}

void ParameterServerApp::resend(TxItem& item)
{
    std::vector<IntAddress> tmp(workers.begin(), workers.end());
    item.pkt->setPacketType(ACK);
    item.destAddresses = std::move(tmp);
    CongApp::resend(item);
}

void ParameterServerApp::onReceivedDuplicatedPacket(Packet* pk)
{
    auto ack = pk->getAckNumber();
    oldPktWithNewAckSeq[ack].insert(pk->getSrcAddr());
    if (oldPktWithNewAckSeq[ack] == workers) {
        confirmAckNumber(pk);
        oldPktWithNewAckSeq.erase(ack);
    }
    // ! In real world, PS's ACK packet maybe lost
    // if (ack > getNextAskedSeq()) {
    //     // ! I left this code for 2 purpose:
    //     // ! Case 1: If you want to allow multicast entry deleted by resend packets
    //     // * old seq but carry new ackNumber,
    //     // * do not answer this ack until all workers ask for the seq
    //     // * Consider this case: A, B(close) send to C, sent 1000,2000,3000,4000,5000,6000,7000,...
    //     // * seq 2000 and 5000 are both stuck in the network, 2000 will resend first(by broadcasting)
    //     // * if 2000ack is received by a close worker, and its askFor5000 arrived immediately(carried by bigger seqs)
    //     // * and you also sent 5000ack, and its askFor(xxxx big seq) arrived, you will clear 5000
    //     // * then when A's askFor5000 arrive, PS won't send 5000ack anymore, because PS think all workers received this
    //     // ! Case 2: between round and round, PS will receicevd a fake old seq help it clear it's txBuffer
    //     oldPktWithNewAckSeq[ack].insert(pk->getSrcAddr());
    //     if (oldPktWithNewAckSeq[ack] == workers) {
    //         confirmAckNumber(pk);
    //         oldPktWithNewAckSeq.erase(ack);
    //     }
    // } else {
    //     confirmAckNumber(pk); // ! let this trigger the resend process
    // }
}

Packet* ParameterServerApp::createDataPacket()
{
    if (currSeq != -1) {
        ASSERT( aggRecord.find(currSeq) != aggRecord.end() );
        auto& record = aggRecord.at(currSeq);
        auto pk = record.pk;
        if (!record.success)
            pk->assignDestAddresses(record.workers);
        aggRecord.erase(currSeq);
        currSeq = -1;
        return pk;
    }
    return nullptr;
}

AggPacket *ParameterServerApp::createAckPacket(const AggPacket* pk)
{
    auto packet = new AggPacket();
    if (aggedBytes == totalAggBytes) {
        if (pk->getFIN() || tcpState == CLOSE_WAIT)
            packet->setFIN(true);
    }
    packet->setByteLength(messageLength);
    packet->setDestPort(2000);
    // copy pk's fields
    packet->setJobId(pk->getJobId());
    packet->setAggSeqNumber(pk->getAggSeqNumber());
    packet->setAggregatorIndex(pk->getAggregatorIndex());
    packet->setRound(pk->getRound());
    return packet;
}
