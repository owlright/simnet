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
}

void Aggregator::checkThenAddWorkerId(const AggPacket *pk)
{
    for (auto& w:pk->getRecord()) {
        if (std::find(workerRecord.begin(), workerRecord.end(), w) != workerRecord.end()) {
            throw cRuntimeError("worker %" PRId64 " is already aggregated.", w);
        }
        workerRecord.push_back(w);
    }
}

Packet *Aggregator::doAggregation(AggPacket *pk)
{
    // copy the ECN flag
    ecn |= pk->getECN(); // * ecn will be set any way(normal or resend)
    auto segmentIndex = pk->getSegmentsLeft() - 1;
    auto& fanIndegree = pk->getArgs(segmentIndex);
    auto indegree = atoi(fanIndegree.c_str());

    // ! do not deal with the resend packet,
    // ! the aggregated result in this aggregator is just discarded
    ASSERT (!pk->getResend());

    // * first packet, store infomation
    auto arrivedAckNumber = pk->getAckNumber();
    if (counter == 0) {
        reset();
        jobId = pk->getJobId();
        seqNumber = pk->getSeqNumber();
        ackNumber = arrivedAckNumber;
        timestamp = pk->getArrivalTime();
    } else {// ! checkAdmission will avoid this case happen
        ASSERT(pk->getJobId() == jobId && pk->getSeqNumber() == seqNumber);
        if (arrivedAckNumber < ackNumber) {
            ackNumber = arrivedAckNumber;
        }
    }

    // * do aggregation, no real behaviour, just update some state
    counter++;
    checkThenAddWorkerId(pk); // ! cheating, let parameter server knows which worker lose
    if (counter == indegree) {
        EV_DEBUG << workerRecord << endl;
        pk->setRecord(workerRecord); // ! workerRecord will move into the packet
        pk->setECN(ecn);
        pk->setAckNumber(ackNumber);
        ASSERT(workerRecord.empty());
        return pk;
    } else {
        delete pk;
        return nullptr;
    }

}

Aggregator::~Aggregator()
{
    reset();
}

bool Aggregator::checkAdmission(const AggPacket *pk) const
{
    if (pk->getJobId() == jobId && pk->getSeqNumber() == seqNumber)
        return true;

    return false;
}
