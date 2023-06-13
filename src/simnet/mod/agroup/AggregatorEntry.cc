#include "AggregatorEntry.h"
#include <bitset>

void AggregatorEntry::reset()
{
    counter = 0;
    jobId = 0;
    seqNumber = 0;
    timestamp = 0;
    ecn = false;
    isIdle = true;
}

Packet *AggregatorEntry::doAggregation(Packet *pk)
{
    EV_WARN << "AggregatorEntry::doAggregation(Packet *pk): " <<
                "Make sure you are not meant to use this method directly" << endl;
    return pk;
}

bool AggregatorEntry::checkAdmission(const Packet *pk) const
{
    auto tmp = check_and_cast<const AggPacket*>(pk);
    if (isIdle) {
        if (!tmp->getResend()) // ! do not allocate an idle aggregator to a resend one.
            return true;
    } else {
        if (tmp->getJobId() == jobId && tmp->getSeqNumber() == seqNumber)
            return true;
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
            reset();
            return pk;
        }
        // ! if this is level0, there are still something to do
    }
    if (counter == 0) {
        // first packet， copy infomation
        ASSERT(isIdle && bitmap == 0);
        reset();
        jobId = pkt->getJobId();
        seqNumber = pkt->getSeqNumber();
        timestamp = pkt->getArrivalTime();
        isIdle = false;
    } else {// ! checkAdmission will avoid this case happen
        ASSERT(pkt->getJobId() == jobId && pkt->getSeqNumber() == seqNumber);
    }


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
        ASSERT(pkt->getResend()); // TODO if there is such case that resend==0?
        ecn = pkt->getEcn();
        delete pkt; // drop the packet
        return nullptr;
    } else {
        // do aggregation
        counter++;
        bitmap |= temp.bitmap;
    }

    if (pkt->getResend()) {
        reset();
        return pk;
    }
    if (counter == temp.fanIndegree) {
        std::bitset<32> bits(bitmap);
        ASSERT(bits.count() == temp.fanIndegree);
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
