
//
// Represents a packet in the network.
//
//packet Packet
//{
//    int srcAddr @packetData;
//    int destAddr @packetData;
//    int groupAddr @packetData;
//    int aggrCounter @packetData;
//    int aggWin @packetData; // TODO is here right
//    int aggNum @packetData;
//    int hopCount @packetData;
//    unsigned int seq @packetData;
//    unsigned int ackSeq @packetData;
//    bool ECN @packetData;
//}
enum PacketType
{
    ACK = 0;
    DATA = 1;
};

packet Packet
{
    int64_t connectionId;
    int64_t flowId;
    int64_t srcAddr;
    uint16_t localPort;
    int64_t destAddr;
    uint16_t destPort;
    int64_t seqNumber;
    bool ECN;
    bool ECE;
}