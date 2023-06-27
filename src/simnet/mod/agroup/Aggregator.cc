#include "Aggregator.h"
#include <bitset>
#include <algorithm>

void Aggregator::reset()
{
    counter = 0;
    jobId = 0;
    seqNumber = 0;
    timestamp = 0;
    ecn = false;
    isIdle = true;
}

void Aggregator::checkThenAddWorkerId(const Packet *pk)
{
    auto pkt = check_and_cast<const AggPacket*>(pk);
    for (auto& w:pkt->getRecord()) {
        if (std::find(workerRecord.begin(), workerRecord.end(), w) != workerRecord.end()) {
            throw cRuntimeError("worker %" PRId64 " is already aggregated.", w);
        }
        workerRecord.push_back(w);
    }
}

Packet *Aggregator::doAggregation(Packet *pk)
{
    auto apk = check_and_cast<AggPacket*>(pk);
    if (apk->getECN()) {
        apk->setEcn(true); // copy the ECN flag
    }
    ecn |= apk->getEcn(); // * ecn will be set any way(normal or resend)
    auto entryIndex = pk->getLastEntry();
    auto& fanIndegree = pk->getArgs(entryIndex);
    auto indegree = atoi(fanIndegree.c_str());

    // ! do not deal with the resend packet,
    // ! the aggregated result in this aggregator is just discarded
    if (apk->getResend()) {
        ASSERT(!isIdle); // ! a resend packet mustn't get an idle aggregator
        ASSERT(apk==pk);
        return pk;
    }

    // * first packet, store infomation
    if (counter == 0) {
        ASSERT(isIdle);
        reset();
        jobId = apk->getJobId();
        seqNumber = apk->getSeqNumber();
        timestamp = apk->getArrivalTime();
        isIdle = false;
    } else {// ! checkAdmission will avoid this case happen
        ASSERT(apk->getJobId() == jobId && apk->getSeqNumber() == seqNumber);
    }

    // * do aggregation, no real behaviour, just update some state
    counter++;
    checkThenAddWorkerId(apk); // ! cheating, let parameter server knows which worker lose
    if (counter == indegree) {
        EV_DEBUG << workerRecord << endl;
        apk->setRecord(workerRecord);
        apk->setEcn(ecn);
        ASSERT(workerRecord.empty());
        return apk;
    } else {
        delete pk;
        return nullptr;
    }

}

Aggregator::~Aggregator()
{
    reset();
}

bool Aggregator::checkAdmission(const Packet *pk) const
{
    auto tmp = check_and_cast<const AggPacket*>(pk);
    if (isIdle) {
        return true;
    } else {
        if (tmp->getJobId() == jobId && tmp->getSeqNumber() == seqNumber)
            return true;
    }
    return false;
}
