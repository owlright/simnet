#include "AggGroupEntry.h"
#include <iostream>
// AggGroupEntry::AggGroupEntry(int groupid, int number, int buffer) {
//     this->groupAddr = groupid;
//     numberOfChidren = number;
//     bufferSize = buffer;
// }

// const Packet *AggGroupEntry::getAggPacket(SeqNumber seq) const
// {
    // auto packetEntry = packetTable.find(seq);
    // if (packets.find(seq)==packets.end()) {
    //     return nullptr;
    // }
    // return packets.at(seq);
// }

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
    }

    return packetTable[seq]->agg(pk);
}

void AggGroupEntry::release(int seq)
{
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
