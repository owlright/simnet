enum PacketType
{
    ACK = 0;
    DATA = 1;
} // TODO: I just use pk->getKind() for now, it's not shown in Packet

packet Packet
{
    int64_t connectionId;
    int64_t srcAddr;
    uint16_t localPort;
    int64_t destAddr;
    uint16_t destPort;
    int64_t seqNumber;
    int64_t timer;
    int32_t aggCounter;
    bool ECN;
    bool ECE;
}
