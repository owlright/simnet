#include "AggrGroupInfo.h"

AggrGroupInfo::AggrGroupInfo(int groupid, int number, int buffer) {
    this->groupid = groupid;
    numberOfChidren = number;
    bufferSize = buffer;
}

const Packet *AggrGroupInfo::getAggrPacket(int seq) const
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
        if (packets.size() == bufferSize) {
            return pk; // ! no space just return
        }
        packets[seq] = pk->dup();
        counter[seq] = 0;
    }
    // deal with the rest packets, just delete here
    delete pk;
    counter[seq]++;
    if (counter[seq] == numberOfChidren) { // ! all packets are aggregated
        return packets.at(seq);
    }
    return nullptr;
}

void AggrGroupInfo::reset(int seq)
{
    packets.erase(seq);
    counter.erase(seq);
}

void AggrGroupInfo::insertChildNode(int address)
{

    children.push_back(address);

}
