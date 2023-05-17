#include "AggGroupEntry.h"
#include <iostream>

AggGroupEntry::AggGroupEntry(B size, int indegree)
{
    this->bufferSize = size;
    this->indegree = indegree; // TODO: indegree is only related to group for now
}

Packet *AggGroupEntry::agg(Packet *pk)
{
    auto seq = pk->getSeqNumber();
    if (pk->isSelfMessage())
        ASSERT(packetTable.find(seq) != packetTable.end());

    if (packetTable.find(seq) == packetTable.end()) {
        packetTable[seq] = new AggPacketEntry(seq);
        packetTable[seq]->fanIndegree = indegree;
        packetTable[seq]->timer = SimTime(pk->getTimer(), SIMTIME_NS);
        packetTable[seq]->startTime = pk->getArrivalTime();
        packetTable[seq]->computationCount = 0;
        packetTable[seq]->usedBytes = pk->getByteLength();
        packetTable[seq]->isTimerPolicy = isTimerPolicy;
        usedBuffer += packetTable[seq]->usedBytes; // ! only subtract the first packet size
    }
    return packetTable[seq]->agg(pk);
}

B AggGroupEntry::release(const Packet* pk)
{
    auto seq = pk->getSeqNumber();
    // TODO: take the computation time into account
    accumulatedTime += pk->getArrivalTime() - packetTable[seq]->startTime;
    computationCount += packetTable[seq]->computationCount;
    auto releasedBuffer = packetTable[seq]->usedBytes;
    usedBuffer -= releasedBuffer;
    delete packetTable[seq];
    packetTable.erase(seq); // it's ok even if seq not exist
    return releasedBuffer;
}

void AggGroupEntry::setAggPolicy(std::string &aggPolicy)
{
    if (aggPolicy == "Count") {
        isTimerPolicy = false;
    }
    else if (aggPolicy == "Timer") {
        isTimerPolicy = true;
    }
}

Packet *AggGroupEntry::AggPacketEntry::agg(Packet *pk)
{
    // ! if use timer policy, when timer is expired, directly sent out the packet
    bool isAggFinished = false;
    if (!pk->isSelfMessage()) {
        if (counter == 0) {
            ASSERT(packet == nullptr);
            packet = pk; // just keep the first packet is ok
        } else {
            computationCount++; // computation begin when the second packet arrives
            // delete pk;
        }
        counter++;
        EV_DEBUG << "group " << pk->getDestAddr() << " seq " << pk->getSeqNumber() << " elapsed " << pk->getArrivalTime() - startTime << endl;
    }
    else {
        isAggFinished = true;
        EV_DEBUG << "(dummy)group " << pk->getDestAddr() << " seq " << pk->getSeqNumber() << " elapsed " << pk->getArrivalTime() - startTime << endl;
    }

    if (isTimerPolicy)
    {
        auto now = pk->getArrivalTime();
        if (now - startTime >= timer) {
            packet->setAggCounter(packet->getAggCounter()+counter);
            isAggFinished = true;
        }
    } else {
        if (counter == fanIndegree) {
            isAggFinished = true;
        }
    }

    if (isAggFinished) {
        if (pk != packet) // in case your group has only one sender which may be used for debug
            delete pk;
        return packet;
    } else {
        if (pk != packet) // ! do not delete the first packet
            delete pk;
        return nullptr;
    }

}

AggGroupEntry::AggPacketEntry::AggPacketEntry(SeqNumber seq)
{
    this->seq = seq;
    packet = nullptr;
    counter = 0;
}
