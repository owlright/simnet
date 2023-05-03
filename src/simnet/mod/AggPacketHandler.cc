#include "Routing.h"

Packet* Routing::AggPacketHandler::agg(Packet *pk)
{
    auto group = pk->getDestAddr();
    auto seq = pk->getSeqNumber();
    // Check if this <group, seq> is forbidden before
    if (markNotAgg.find(std::make_pair(group, seq)) != markNotAgg.end()) {
        return pk;
    }
    // ! when packet arrives but not enough memory to hold it , it must be sent out immediately,
    // ! the following packets of the same <groupAddr, seq> cannot be aggregated either
    if (getUsedBufferSize() >= bufferSize) {
        markNotAgg.insert(std::make_pair(group, seq));
        return pk; // ! no buffer to store the packet, just return it
    }

    if (groupTable.find(group) == groupTable.end()) {
        // * the first time we see the group
        auto indegree = groupManager->getFanIndegree(group, 0, switchAddress); // FIXME set treeindex =0
        groupTable[group] = new AggGroupEntry(bufferSize, indegree);
    }
    // ! Now group entry must be in the groupTable
    return groupTable.at(group)->agg(pk);
}

B Routing::AggPacketHandler::getUsedBufferSize() const
{
    // TODO this can be optimized
    B used = 0;
    for (const auto& kv: groupTable) {
        used += kv.second->getUsedBufferSize();
    }
    return used;
}

void Routing::AggPacketHandler::releaseGroupOnSeq(IntAddress group, SeqNumber seq)
{
    auto key = std::make_pair(group, seq);
    if (markNotAgg.find(key) != markNotAgg.end()) {
        // the <group, seq> has not been aggregated at all
        markNotAgg.erase(key);
    } else {
        groupTable[group]->release(seq);
    }
}

const std::unordered_set<int> &Routing::AggPacketHandler::getReversePortIndexes(Packet *pk) const
{
    auto group = pk->getDestAddr();
    auto seq = pk->getSeqNumber();
    if (groupTable.find(group)==groupTable.end())
        throw cRuntimeError("GroupPakcetHandler::getReversePortIndexes: not find the group");
    return groupTable.at(group)->getIncomingPortIndexes(seq);
}
