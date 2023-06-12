enum PacketType
{
    ACK      = 0;
    DATA     = 1;
    AGG      = 2;
    REMIND   = 3;
    NOUSE    = 4;
};

enum AggPolicy
{
    ATP = 0;
    MTATP = 1;
}

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
    PacketType packetType;
    int64_t connectionId;
    int64_t receivedBytes;
    double startTime;
    double transmitTime;
    double queueTime;
    bool isFlowFinished;
}

class AggPacket extends Packet
{
    packetType = AGG;
    AggPolicy aggPolicy;
    int aggregatorIndex;
    int64_t jobId;
    int workerNumber;
    bool overflow;
    bool resend;
    bool collision;
    bool ecn;
    bool isAck;
}

class ATPPacket extends AggPacket
{
    aggPolicy = ATP;
    byteLength = 16 + 8; // ATP header total size is 58 Bytes
    uint32_t bitmap0;
    uint32_t bitmap1;
    int fanIndegree0;
    int fanIndegree1;
    int switchIdentifier;
}

class MTATPPacket extends AggPacket
{
    aggPolicy = MTATP;
    byteLength = 16 + 4;
    int timer;
    int aggCounter;
    bool flags[5];
    int treeCost;
}
