enum PacketType
{
    ACK = 0;
    DATA = 1;
    REMIND = 2;
}; // TODO: I just use pk->getKind() for now, so it's not shown in Packet fields

packet EthernetMacHeader
{
    byteLength = 14;
}

class IpHeader extends EthernetMacHeader
{
    byteLength = 14 + 20;
    int64_t srcAddr;
    int64_t destAddr;
}

class UdpHeader extends IpHeader
{
    byteLength = 14 + 20 + 8;
    uint16_t localPort;
    uint16_t destPort;
}

class TcpLikeHeader extends IpHeader
{
    // ! do not use this class directly, it has no size
    int64_t seqNumber;
    uint16_t localPort;
    uint16_t destPort;
    bool ECN;
    bool ECE;
}

class Packet extends TcpLikeHeader
{
    int64_t connectionId;
    int64_t receivedBytes;
    double startTime;
    double transmitTime;
    double queueTime;
    bool isFlowFinished;
}

class AggPacket extends Packet
{
    int aggregatorIndex;
    int64_t jobId;
    int workerNumber;
}

class ATPPacket extends AggPacket
{
    byteLength = 16 + 8; // ATP header total size is 58 Bytes
    int bitmap0;
    int bitmap1;
    int fanIndegree0;
    int fanIndegree1;
    bool flags[6];
}

class MTATPPacket extends AggPacket
{
    byteLength = 16 + 4;
    int timer;
    int aggCounter;
    bool flags[5];
    int treeCost;
}
