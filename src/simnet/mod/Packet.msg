enum PacketType
{
    ACK = 0;
    DATA = 1;
    REMIND = 2;
}; // TODO: I just use pk->getKind() for now, so it's not shown in Packet fields

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
    int32_t aggNumber;

    int64_t receivedBytes;
    double startTime;
    double transmitTime;
    double queueTime;
    bool ECN;
    bool ECE;
    bool isFlowFinished;
}

packet commonHeader
{
    byteLength = 14 + 20;
    int64_t connectionId;
    int64_t srcAddr;
    int64_t destAddr;
    uint16_t localPort;
    uint16_t destPort;
    int64_t seqNumber;
    bool ECN;
    bool ECE;

    int64_t receivedBytes;
    double startTime;
    double transmitTime;
    double queueTime;
    bool isFlowFinished;
}

class ATPHeader extends commonHeader
{
    int bitmap0;
    int bitmap1;
    int fanIndegree0;
    int fanIndegree1;
    bool flags[6];
    int aggregatorIndex;
    int jobIdSeqNumber;

    int workerNumber;

    byteLength = 14 + 20 + 16 + 8;
}

class MTATPHeader extends commonHeader
{
    int timer;
    int workerNumber;
    int aggCounter;
    bool flags[5];
    int aggregatorIndex;
    int jobIdSeqNumber;
    int treeCost;
    byteLength = 14 + 20 + 16 + 4;
}