cplusplus {{
#include <vector>
#include <set>
#include <algorithm>
using omnetpp::opp_string;
}}

enum PacketType
{
    ACK      = 0;
    DATA     = 1;
    AGG      = 2;
//    REMIND   = 3;
    MACK     = 3; // multicast ack
//    NOUSE    = 5;
};

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
    int64_t srcAddr = -1; // 16B
    int64_t destAddr = -1; // 16B
    int64_t destAddresses[] @custom @sizeGetter(getNumDestAddrs)
                                    @sizeSetter(setNumDestAddrs)
                                    @getter(getDestAddresses)
                                    @setter(setDestAddresses);
}

cplusplus(IPv6Header) {{
    protected:
        std::vector<int64_t> destAddresses;
    public:
        void setNumDestAddrs(size_t size) {destAddresses.resize(size);}
        size_t getNumDestAddrs() const {return destAddresses.size();}
        int64_t getDestAddresses(size_t k) const {
            ASSERT(destAddresses.size() > k);
            return destAddresses.at(k);
        }
        const std::vector<int64_t>& getAllDestAddresses() const {return destAddresses;}
        void setDestAddresses(size_t k, int64_t addr) {
            destAddresses[k] = addr;
        }
        void assignDestAddresses(std::set<int64_t>& addrs) {
            destAddresses.assign(addrs.begin(), addrs.end());
        }
}}
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
    int64_t segments[] @custom @sizeGetter @sizeSetter;
    string funs[] @custom @sizeGetter @sizeSetter; // tell the node what to do
    string args[] @custom @sizeGetter @sizeSetter; // args of functions
}

class Packet extends SegmentRoutingHeader
{
    // udp/tcp fields
    int64_t seqNumber;
    int64_t ackNumber;
    uint16_t localPort;
    uint16_t destPort;
    bool FIN = false;
//    bool FINACK = false;
    bool ECN = false;
    bool ECE;
    // cheating fields
    PacketType packetType;
    int64_t connectionId;
    double transmitTime;
    double queueTime;
    //bool isFlowFinished;
    bool resend;
    int round;
}

cplusplus(SegmentRoutingHeader) {{
	protected:
		std::vector<int64_t> segments; // intermediate node address
		std::vector<opp_string> funs;
		std::vector<opp_string> args;
	public:
		// expected methods:
		virtual void setSegmentsArraySize(size_t size) {segments.resize(size);}
		virtual void setFunsArraySize(size_t size) {funs.resize(size);}
		virtual void setArgsArraySize(size_t size) {args.resize(size);}

		virtual size_t getSegmentsArraySize() const {return segments.size();}
		virtual size_t getFunsArraySize() const {return funs.size();}
		virtual size_t getArgsArraySize() const {return args.size();}

		virtual int64_t getSegments(size_t k) const {
		    ASSERT(segments.size() > k);
		    return segments.at(k);
		}
		virtual const opp_string& getFuns(size_t k) const {
		    ASSERT(funs.size() > k);
		    return funs[k];
		}
		virtual const opp_string& getArgs(size_t k) const {
		    ASSERT(args.size() > k);
		    return args[k];
		}

		virtual void setSegments(size_t k, int64_t addr) {segments[k] = addr;}
		virtual void setFuns(size_t k, const char* fun) {funs[k] = fun;}
		virtual void setArgs(size_t k, const char* arg) {args[k] = arg;}

		// new methods:
		virtual void setSIDSize(size_t size) {
		    setSegmentsArraySize(size);
		    setFunsArraySize(size);
		    setArgsArraySize(size);
		}
		virtual void popSegment() {segments.pop_back();}
		virtual void popFun() {funs.pop_back();}
		virtual void popArg() {args.pop_back();}
        virtual const std::vector<int64_t>& getAllSegments() {return segments;}
}}

cplusplus(SegmentRoutingHeader::copy) {{
    segments = other.segments;
    funs = other.funs;
   	args = other.args;
}}

