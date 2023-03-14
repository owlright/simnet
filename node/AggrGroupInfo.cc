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
//        if (packets.size() == bufferSize) {
//            return pk; // ! no space just return
//        }
        packets[seq] = pk->dup();
        counter[seq] = 0;
        senderCounter[seq] = 0;
    }
    // deal with the rest packets
    senderCounter[seq] += pk->getAggrCounter();
    delete pk;
    counter[seq]++;
    if (counter[seq] == numberOfChidren) { // ! all packets are aggregated
        auto packet = packets.at(seq);
        packet->setAggrCounter(senderCounter.at(seq));
        return packet;
    }
    return nullptr;
}

void AggrGroupInfo::reset(int seq)
{
    packets.erase(seq); // it's ok even if seq not exist
    counter.erase(seq);
    senderCounter.erase(seq);
    notAggred.erase(seq);
}

bool AggrGroupInfo::isGroupHasBuffer(int seq) const {
    return packets.size() < bufferSize;
}

void AggrGroupInfo::insertChildNode(int address)
{

    children.push_back(address);

}

void AggrGroupInfo::recordNotAggr(int seq) {
    notAggred.insert(seq);
}

bool AggrGroupInfo::isRecorded(int address) const {
    return notAggred.count(address) == 1;
}
