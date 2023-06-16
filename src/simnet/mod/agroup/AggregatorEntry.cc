#include "AggregatorEntry.h"
#include <bitset>
#include <algorithm>

void AggregatorEntry::reset()
{
    counter = 0;
    jobId = 0;
    seqNumber = 0;
    timestamp = 0;
    ecn = false;
    isIdle = true;
}

void AggregatorEntry::checkThenAddWorkerId(const Packet *pk)
{
    auto pkt = check_and_cast<const AggPacket*>(pk);
    for (auto& w:pkt->getRecord()) {
        if (std::find(workerRecord.begin(), workerRecord.end(), w) != workerRecord.end()) {
            throw cRuntimeError("worker %lld is already aggregated.", w);
        }
        workerRecord.push_back(w);
    }
}

Packet *AggregatorEntry::doAggregation(Packet *pk)
{
    EV_WARN << "AggregatorEntry::doAggregation(Packet *pk): " <<
                "Make sure you are not meant to use this method directly" << endl;
    return pk;
}

AggregatorEntry::~AggregatorEntry()
{
    reset();
}

bool AggregatorEntry::checkAdmission(Packet *pk) const
{
    auto tmp = check_and_cast<AggPacket*>(pk);
    if (isIdle) {
        if (!tmp->getResend()) // ! do not allocate an idle aggregator to a resend one.
            return true;
    } else {
        if (tmp->getJobId() == jobId && tmp->getSeqNumber() == seqNumber)
            return true;
        else {
            // this means collision
            // set these fields before forwarding
            tmp->setCollision(true);
            tmp->setResend(true); // TODO I don't know why ATP set this
        }
    }
    return false;
}

Packet *ATPEntry::doAggregation(Packet *pk)
{
    auto pkt = check_and_cast<ATPPacket*>(pk); // ! using pkt in the below code

    auto isLevel0 = pkt->getSwitchIdentifier() == 0;
    if (pkt->getResend()) { // ! is this a resend packet?
        ASSERT(!isIdle); // ! checkAdmission will avoid this case happen
        if (!isLevel0) {
            // ! if this is level 1 just send it out, but get its ecn
            ecn |= pkt->getEcn();
            // TODO partial aggregation
            reset();
            return pk;
        }
        // ! if this is level0, there are still something to do
    }
    if (counter == 0) {
        // first packet, copy infomation
        ASSERT(isIdle && bitmap == 0);
        reset();
        jobId = pkt->getJobId();
        seqNumber = pkt->getSeqNumber();
        timestamp = pkt->getArrivalTime();
        isIdle = false;
    } else {// ! checkAdmission will avoid this case happen
        ASSERT(pkt->getJobId() == jobId && pkt->getSeqNumber() == seqNumber);
    }
    // ! ecn will be set any way
    ecn |= pkt->getEcn();
    if (isLevel0) {
        temp.bitmap = pkt->getBitmap0();
        temp.fanIndegree = pkt->getFanIndegree0();
        pkt->setSwitchIdentifier(1);
    } else {
        temp.bitmap = pkt->getBitmap1();
        temp.fanIndegree = pkt->getFanIndegree1();
        pkt->setSwitchIdentifier(0); // TODO I don't know why ATP set this?
    }

    if ((temp.bitmap & bitmap) > 0) {
        // is already aggregated
        ASSERT(pkt->getResend());
        // TODO if there is such case that resend==0?
        // ! case 1: aggregator is not released, so next round the same seq will use the same aggregator
        ecn = pkt->getEcn();
        delete pkt; // drop the packet
        return nullptr;
    } else {        \
        //! cheating
        checkThenAddWorkerId(pkt);
        // do aggregation
        counter++;
        bitmap |= temp.bitmap;
    }
    EV_DEBUG << pkt->getDestAddr() << " aggCounter: " << counter;
    EV_DEBUG << " bitmap: " << std::bitset<32>(bitmap)<< endl;
    if (pkt->getResend()) {
        reset();
        return pk;
    }
    if (counter == temp.fanIndegree) {
        std::bitset<32> bits(bitmap);
        ASSERT(bits.count() == temp.fanIndegree);
        // copy switch recorded info into packet
        if (isLevel0)
            pkt->setBitmap0(bitmap);
        else
            pkt->setBitmap1(bitmap);
        pkt->setEcn(ecn);
        EV_DEBUG << workerRecord << endl;
        pkt->setRecord(workerRecord);
        ASSERT(workerRecord.empty());
        return pk;
    } else {
        delete pk;
        return nullptr;
    }

}

void ATPEntry::reset()
{
    AggregatorEntry::reset();
    bitmap = 0;
    temp.bitmap = 0;
    temp.fanIndegree = 0;
}
