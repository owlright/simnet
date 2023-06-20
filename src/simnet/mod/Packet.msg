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
    byteLength = 14; // ! only this affects size
    uint32_t destinationMacAddr; // 6B
    uint32_t sourceMacAddr; // 6B
    uint32_t etherType; // 4B
    
}

class IPv4Header extends EthernetMacHeader
{
    byteLength = 14 + 20;
    uint8_t version = 4; // 4 bits
    uint8_t IHL; // internet header length 4 bits
    uint8_t DSCP; // 6 bits
    uint8_t ipECN; // 2 bits
    uint16_t totalLength; // 2B
    uint8_t identification; // 2B
    uint8_t ipFlags; // 3 bits
    uint8_t fragmentOffset; // 13 bits;
    uint8_t TTL; // 1B
    uint8_t protocol; // 1B
    uint16_t checksum; // 2B
    int64_t srcAddr; // 4B
    int64_t destAddr; // 4B
    uint64_t options; // if IHL > 5
}

class IPv6Header extends EthernetMacHeader
{
    byteLength = 14 + 40;
    uint8_t version = 6; // 4 bits
//    uint8_t trafficClass; // 8bits
//    uint32_t flowLabel; // 20 bits
//    uint16_t payloadLength; // 2B
//    uint8_t nextHeader; // 1B
//    uint8_t TTL; // 1B
    int64_t srcAddr; // 16B
    int64_t destAddr; // 16B
}

class SegmentRoutingHeader extends IPv6Header
{
//    uint8_t nextHeader; // 1B
    uint8_t hdrLength; // Header extension length 1B
    uint8_t routingType; // 1B
    uint8_t segmentsLeft; // 1B
    uint8_t lastEntry; //1B
    uint8_t srhFlags; // 1B
    uint16_t srhTag; // 2B
    // below three are together segmentList each 16B
    int64_t SID[];
    string function[];
    string args[];
}

class Packet extends SegmentRoutingHeader
{
    // udp/tcp fields
    int64_t seqNumber;
    uint16_t localPort;
    uint16_t destPort;
    bool ECN;
    bool ECE;
    // cheating fields
    PacketType packetType;
    int64_t connectionId;
    int64_t receivedBytes;
    double startTime;
    double transmitTime;
    double queueTime;
    bool isFlowFinished;
}

cplusplus {{
#include <vector>
}}

class AggPacket extends Packet
{
    packetType = AGG;
    AggPolicy aggPolicy;
    int round;
    int aggregatorIndex;
    int64_t jobId;
    int workerNumber;
    bool overflow;
    bool resend;
    bool collision;
    bool ecn;
    bool isAck;
    // cheating fields
    int64_t workerRecord[] @custom @sizeGetter(getRecordLen)
                           @sizeSetter(setRecordLen)
                           @getter(getRecord)
                           @setter(setRecord);
}

cplusplus(AggPacket) {{
    protected:
        std::vector<int64_t> workerRecord;
    public:
        // expected methods:
        virtual void setRecordLen(size_t size) {
            workerRecord.reserve(size);
        }
        virtual size_t getRecordLen() const {return workerRecord.size();}
        virtual std::vector<int64_t> getRecord() const {return workerRecord;}
        virtual int64_t getRecord(size_t k) {return workerRecord.at(k);}
        virtual void setRecord(size_t k, int64_t worker) {
            EV_WARN << "You should not use this method, but using addRecord instead." << std::endl;
            workerRecord[k] = worker;
        }
        //new methods:
        virtual void addRecord(int64_t worker) {
            workerRecord.push_back(worker);
        }
        virtual void setRecord(std::vector<int64_t>& workers) {
            auto oldLen = getRecordLen();
            for (auto i = 0; i < oldLen; i++) {
                workerRecord[i] = workers.back();
                workers.pop_back();
            }
            while (!workers.empty()) {
                workerRecord.push_back(workers.back());
                workers.pop_back();
            }
        }
}}

cplusplus(AggPacket::copy) {{
    this->workerRecord = other.workerRecord;
}}

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

