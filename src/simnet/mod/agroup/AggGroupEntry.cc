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
    if (markNotAgg.find(seq) != markNotAgg.end())
       return pk;

    if (getUsedBufferSize() >= bufferSize) {
        // When switch has enough buffer space, it doesn't mean the group can use it.
        markNotAgg.insert(seq);
        return pk;
    }

    if (packetTable.find(seq) == packetTable.end()) {
        packetTable[seq] = new AggPacketEntry(seq, indegree);
        packetTable[seq]->startTime = pk->getArrivalTime();
        packetTable[seq]->computationCount = 0;
    }

    return packetTable[seq]->agg(pk);
}

void AggGroupEntry::release(const Packet* pk)
{
    auto seq = pk->getSeqNumber();
    accumulatedTime += pk->getArrivalTime() - packetTable[seq]->startTime; // TODO: take the computation time into account
    computationCount += packetTable[seq]->computationCount;
    packetTable.erase(seq); // it's ok even if seq not exist
    markNotAgg.erase(seq);
}

B AggGroupEntry::getUsedBufferSize() const
{
    B used = 0;
    for (const auto& kv: packetTable) {
        ASSERT(kv.second->packet != nullptr);
        used += kv.second->packet->getByteLength();
    }
    return used;
}

Packet *AggGroupEntry::AggPacketEntry::agg(Packet *pk)
{
    incomingPortIndexes.insert(pk->getArrivalGate()->getIndex());
    if (counter == 0) {
        assert(packet == nullptr);
        packet = pk; // just keep the first packet is ok
    } else {
        computationCount++;
        delete pk;
    }

    counter++;
    if (counter == fanIndegree)
        return packet;
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
