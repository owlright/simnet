#include "AggGroupEntry.h"
#include <iostream>

AggGroupEntry::AggGroupEntry(B size, int indegree)
{
    this->bufferSize = size;
    this->indegree = indegree;
}

Packet *AggGroupEntry::agg(Packet *pk)
{
    auto seq = pk->getSeqNumber();
    if (packetTable.find(seq) == packetTable.end()) {
        packetTable[seq] = new AggPacketEntry(seq, indegree);
        packetTable[seq]->startTime = pk->getArrivalTime();
        packetTable[seq]->computationCount = 0;
        packetTable[seq]->usedBytes = pk->getByteLength();
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

Packet *AggGroupEntry::AggPacketEntry::agg(Packet *pk)
{
//    incomingPortIndexes.insert(pk->getArrivalGate()->getIndex());
    if (counter == 0) {
        assert(packet == nullptr);
        packet = pk; // just keep the first packet is ok
    } else {
        computationCount++;
        delete pk;
    }

    counter++;
    if (counter == fanIndegree) {
        return packet;
    }
    else
        return nullptr;

}

AggGroupEntry::AggPacketEntry::AggPacketEntry(SeqNumber seq, int indegree)
{
    this->seq = seq;
    fanIndegree = indegree;
    packet = nullptr;
    counter = 0;
}
