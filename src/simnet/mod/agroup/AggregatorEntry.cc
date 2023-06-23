#include "AggregatorEntry.h"
#include <bitset>
#include <algorithm>

ATPEntry::ATPEntry(const Packet* pk) {
    auto pkt = check_and_cast<const ATPPacket*>(pk);
    reset();
    jobId = pkt->getJobId();
    seqNumber = pkt->getSeqNumber();
}

Packet *ATPEntry::doAggregation(Packet *pk)
{
    auto pkt = check_and_cast<ATPPacket*>(pk); // ! using pkt in the below code

    auto isLevel0 = pkt->getSwitchIdentifier() == 0;
    if (pkt->getResend()) { // ! is this a resend packet?
        if (!isLevel0) {
            // ! if switchId==1 just send it out, but get its ecn, the aggregator value is just disposed
            ecn |= pkt->getEcn();
            return pk;
        }
        // ! if this is level0, there are still something to do even if it's a resend packet
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
    } else {
        //! cheating
        checkThenAddWorkerId(pkt);
        // do aggregation
        counter++;
        bitmap |= temp.bitmap;
    }
    EV_DEBUG << pkt->getDestAddr() << " aggCounter: " << counter;
    EV_DEBUG << " bitmap: " << std::bitset<32>(bitmap)<< endl;
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
    Aggregator::reset();
    bitmap = 0;
    temp.bitmap = 0;
    temp.fanIndegree = 0;
}
