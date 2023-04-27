#include "GroupPacketHandler.h"
#include "simnet/common/ModuleAccess.h"
Define_Module(GroupPacketHandler);

void GroupPacketHandler::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        bufferSize = par("bufferSize");
        groupManager = getModuleFromPar<GlobalGroupManager>(par("groupManager"), this) ;
        if (groupManager == nullptr)
            throw cRuntimeError("GroupPacketHandler::initialize: If not manual, you must set the globalGroupManager!");
    }
}

Packet *GroupPacketHandler::agg(Packet *pk)
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
        // set this group
        groupTable[group] = groupManager->getGroupEntry(group);
    }
    // ! Now group entry must be in the groupTable
    return groupTable.at(group)->agg(pk);
}

B GroupPacketHandler::getUsedBufferSize() const
{
    // TODO this can be optimized
    B used = 0;
    for (const auto& kv: groupTable) {
        used += kv.second->getUsedBufferSize();
    }
    return used;
}

void GroupPacketHandler::releaseGroupOnSeq(IntAddress group, SeqNumber seq)
{
    auto key = std::make_pair(group, seq);
    if (markNotAgg.find(key) != markNotAgg.end()) {
        // the <group, seq> has not been aggregated at all
        markNotAgg.erase(key);
    } else {
        groupTable[group]->release(seq);
    }
}

const std::vector<int> &GroupPacketHandler::getReversePortIndexes(Packet *pk) const
{
    auto group = pk->getDestAddr();
    auto seq = pk->getSeqNumber();
    if (groupTable.find(group)!=groupTable.end())
        throw cRuntimeError("GroupPakcetHandler::getReversePortIndexes: not find the group");
    return groupTable.at(group)->getIncomingPortIndexes(seq);
}
