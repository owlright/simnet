#include "AggrGroupInfo.h"

AggrGroupInfo::AggrGroupInfo(int groupid, int number) {
    this->groupid = groupid;
    numberOfChidren = number;
}

Packet *AggrGroupInfo::getAggrPacket(int seq) const
{
    if (packets.find(seq)==packets.end()) {
        return nullptr;
    }
    return packets.at(seq);
}

bool AggrGroupInfo::isChildrenFull() const
{
    return children.size() == numberOfChidren;
}

Packet* AggrGroupInfo::aggrPacket(int seq, Packet *pk)
{
    if (packets.find(seq) == packets.end()) { // first packet of a round
        packets[seq] = pk->dup();
        counter[seq] = numberOfChidren;
    }
    // deal with the rest packets, just delete here
    delete pk;
    counter[seq]--;
    if (counter[seq] == 0) { // ! all packets are aggregated
        return packets.at(seq);
    }
    return nullptr;
}

void AggrGroupInfo::reset(int seq)
{
    packets[seq] = nullptr;
    counter[seq] = numberOfChidren;
    rounds[seq] += 1;
}

void AggrGroupInfo::insertChildNode(int address)
{

    children.push_back(address);

}
