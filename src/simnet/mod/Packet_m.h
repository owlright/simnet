//
// Generated file, do not edit! Created by opp_msgtool 6.0 from simnet/mod/Packet.msg.
//

#ifndef __PACKET_M_H
#define __PACKET_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// opp_msgtool version check
#define MSGC_VERSION 0x0600
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgtool: 'make clean' should help.
#endif

class Packet;
class EthernetMacHeader;
class IpHeader;
class UdpHeader;
class TcpLikeHeader;
class ATPHeader;
class MTATPHeader;
/**
 * Enum generated from <tt>simnet/mod/Packet.msg:2</tt> by opp_msgtool.
 * <pre>
 * enum PacketType
 * {
 *     ACK = 0;
 *     DATA = 1;
 *     REMIND = 2;
 * } // TODO: I just use pk->getKind() for now, so it's not shown in Packet fields
 * </pre>
 */
enum PacketType {
    ACK = 0,
    DATA = 1,
    REMIND = 2
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const PacketType& e) { b->pack(static_cast<int>(e)); }
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, PacketType& e) { int n; b->unpack(n); e = static_cast<PacketType>(n); }

/**
 * Class generated from <tt>simnet/mod/Packet.msg:9</tt> by opp_msgtool.
 * <pre>
 * packet Packet // to be deleted
 * {
 *     int64_t connectionId;
 *     int64_t srcAddr;
 *     uint16_t localPort;
 *     int64_t destAddr;
 *     uint16_t destPort;
 *     int64_t seqNumber;
 *     int64_t timer;
 *     int32_t aggCounter;
 *     int32_t aggNumber;
 * 
 *     int64_t receivedBytes;
 *     double startTime;
 *     double transmitTime;
 *     double queueTime;
 *     bool ECN;
 *     bool ECE;
 *     bool isFlowFinished;
 * }
 * </pre>
 */
class Packet : public ::omnetpp::cPacket
{
  protected:
    int64_t connectionId = 0;
    int64_t srcAddr = 0;
    uint16_t localPort = 0;
    int64_t destAddr = 0;
    uint16_t destPort = 0;
    int64_t seqNumber = 0;
    int64_t timer = 0;
    int32_t aggCounter = 0;
    int32_t aggNumber = 0;
    int64_t receivedBytes = 0;
    double startTime = 0;
    double transmitTime = 0;
    double queueTime = 0;
    bool ECN = false;
    bool ECE = false;
    bool isFlowFinished_ = false;

  private:
    void copy(const Packet& other);

  protected:
    bool operator==(const Packet&) = delete;

  public:
    Packet(const char *name=nullptr, short kind=0);
    Packet(const Packet& other);
    virtual ~Packet();
    Packet& operator=(const Packet& other);
    virtual Packet *dup() const override {return new Packet(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int64_t getConnectionId() const;
    virtual void setConnectionId(int64_t connectionId);

    virtual int64_t getSrcAddr() const;
    virtual void setSrcAddr(int64_t srcAddr);

    virtual uint16_t getLocalPort() const;
    virtual void setLocalPort(uint16_t localPort);

    virtual int64_t getDestAddr() const;
    virtual void setDestAddr(int64_t destAddr);

    virtual uint16_t getDestPort() const;
    virtual void setDestPort(uint16_t destPort);

    virtual int64_t getSeqNumber() const;
    virtual void setSeqNumber(int64_t seqNumber);

    virtual int64_t getTimer() const;
    virtual void setTimer(int64_t timer);

    virtual int32_t getAggCounter() const;
    virtual void setAggCounter(int32_t aggCounter);

    virtual int32_t getAggNumber() const;
    virtual void setAggNumber(int32_t aggNumber);

    virtual int64_t getReceivedBytes() const;
    virtual void setReceivedBytes(int64_t receivedBytes);

    virtual double getStartTime() const;
    virtual void setStartTime(double startTime);

    virtual double getTransmitTime() const;
    virtual void setTransmitTime(double transmitTime);

    virtual double getQueueTime() const;
    virtual void setQueueTime(double queueTime);

    virtual bool getECN() const;
    virtual void setECN(bool ECN);

    virtual bool getECE() const;
    virtual void setECE(bool ECE);

    virtual bool isFlowFinished() const;
    virtual void setIsFlowFinished(bool isFlowFinished);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Packet& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Packet& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>simnet/mod/Packet.msg:30</tt> by opp_msgtool.
 * <pre>
 * packet EthernetMacHeader
 * {
 *     byteLength = 14;
 * }
 * </pre>
 */
class EthernetMacHeader : public ::omnetpp::cPacket
{
  protected:

  private:
    void copy(const EthernetMacHeader& other);

  protected:
    bool operator==(const EthernetMacHeader&) = delete;

  public:
    EthernetMacHeader(const char *name=nullptr, short kind=0);
    EthernetMacHeader(const EthernetMacHeader& other);
    virtual ~EthernetMacHeader();
    EthernetMacHeader& operator=(const EthernetMacHeader& other);
    virtual EthernetMacHeader *dup() const override {return new EthernetMacHeader(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const EthernetMacHeader& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, EthernetMacHeader& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>simnet/mod/Packet.msg:35</tt> by opp_msgtool.
 * <pre>
 * class IpHeader extends EthernetMacHeader
 * {
 *     byteLength = 14 + 20;
 *     int64_t srcAddr;
 *     int64_t destAddr;
 * }
 * </pre>
 */
class IpHeader : public ::EthernetMacHeader
{
  protected:
    int64_t srcAddr = 0;
    int64_t destAddr = 0;

  private:
    void copy(const IpHeader& other);

  protected:
    bool operator==(const IpHeader&) = delete;

  public:
    IpHeader(const char *name=nullptr);
    IpHeader(const IpHeader& other);
    virtual ~IpHeader();
    IpHeader& operator=(const IpHeader& other);
    virtual IpHeader *dup() const override {return new IpHeader(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int64_t getSrcAddr() const;
    virtual void setSrcAddr(int64_t srcAddr);

    virtual int64_t getDestAddr() const;
    virtual void setDestAddr(int64_t destAddr);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const IpHeader& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, IpHeader& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>simnet/mod/Packet.msg:42</tt> by opp_msgtool.
 * <pre>
 * class UdpHeader extends IpHeader
 * {
 *     byteLength = 14 + 20 + 8;
 *     uint16_t localPort;
 *     uint16_t destPort;
 * }
 * </pre>
 */
class UdpHeader : public ::IpHeader
{
  protected:
    uint16_t localPort = 0;
    uint16_t destPort = 0;

  private:
    void copy(const UdpHeader& other);

  protected:
    bool operator==(const UdpHeader&) = delete;

  public:
    UdpHeader(const char *name=nullptr);
    UdpHeader(const UdpHeader& other);
    virtual ~UdpHeader();
    UdpHeader& operator=(const UdpHeader& other);
    virtual UdpHeader *dup() const override {return new UdpHeader(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual uint16_t getLocalPort() const;
    virtual void setLocalPort(uint16_t localPort);

    virtual uint16_t getDestPort() const;
    virtual void setDestPort(uint16_t destPort);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const UdpHeader& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, UdpHeader& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>simnet/mod/Packet.msg:49</tt> by opp_msgtool.
 * <pre>
 * class TcpLikeHeader extends IpHeader
 * {
 *     // ! do not use this class directly, it has no size
 *     int64_t connectionId;
 *     int64_t seqNumber;
 *     uint16_t localPort;
 *     uint16_t destPort;
 *     bool ECN;
 *     bool ECE;
 * 
 *     int64_t receivedBytes;
 *     double startTime;
 *     double transmitTime;
 *     double queueTime;
 *     bool isFlowFinished;
 * }
 * </pre>
 */
class TcpLikeHeader : public ::IpHeader
{
  protected:
    int64_t connectionId = 0;
    int64_t seqNumber = 0;
    uint16_t localPort = 0;
    uint16_t destPort = 0;
    bool ECN = false;
    bool ECE = false;
    int64_t receivedBytes = 0;
    double startTime = 0;
    double transmitTime = 0;
    double queueTime = 0;
    bool isFlowFinished_ = false;

  private:
    void copy(const TcpLikeHeader& other);

  protected:
    bool operator==(const TcpLikeHeader&) = delete;

  public:
    TcpLikeHeader(const char *name=nullptr);
    TcpLikeHeader(const TcpLikeHeader& other);
    virtual ~TcpLikeHeader();
    TcpLikeHeader& operator=(const TcpLikeHeader& other);
    virtual TcpLikeHeader *dup() const override {return new TcpLikeHeader(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int64_t getConnectionId() const;
    virtual void setConnectionId(int64_t connectionId);

    virtual int64_t getSeqNumber() const;
    virtual void setSeqNumber(int64_t seqNumber);

    virtual uint16_t getLocalPort() const;
    virtual void setLocalPort(uint16_t localPort);

    virtual uint16_t getDestPort() const;
    virtual void setDestPort(uint16_t destPort);

    virtual bool getECN() const;
    virtual void setECN(bool ECN);

    virtual bool getECE() const;
    virtual void setECE(bool ECE);

    virtual int64_t getReceivedBytes() const;
    virtual void setReceivedBytes(int64_t receivedBytes);

    virtual double getStartTime() const;
    virtual void setStartTime(double startTime);

    virtual double getTransmitTime() const;
    virtual void setTransmitTime(double transmitTime);

    virtual double getQueueTime() const;
    virtual void setQueueTime(double queueTime);

    virtual bool isFlowFinished() const;
    virtual void setIsFlowFinished(bool isFlowFinished);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const TcpLikeHeader& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, TcpLikeHeader& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>simnet/mod/Packet.msg:66</tt> by opp_msgtool.
 * <pre>
 * class ATPHeader extends TcpLikeHeader
 * {
 *     byteLength = 14 + 20 + 16 + 8; // ATP header total size is 58 Bytes
 *     int bitmap0;
 *     int bitmap1;
 *     int fanIndegree0;
 *     int fanIndegree1;
 *     bool flags[6];
 *     int aggregatorIndex;
 *     int jobIdSeqNumber;
 *     int workerNumber;
 * }
 * </pre>
 */
class ATPHeader : public ::TcpLikeHeader
{
  protected:
    int bitmap0 = 0;
    int bitmap1 = 0;
    int fanIndegree0 = 0;
    int fanIndegree1 = 0;
    bool flags[6];
    int aggregatorIndex = 0;
    int jobIdSeqNumber = 0;
    int workerNumber = 0;

  private:
    void copy(const ATPHeader& other);

  protected:
    bool operator==(const ATPHeader&) = delete;

  public:
    ATPHeader(const char *name=nullptr);
    ATPHeader(const ATPHeader& other);
    virtual ~ATPHeader();
    ATPHeader& operator=(const ATPHeader& other);
    virtual ATPHeader *dup() const override {return new ATPHeader(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int getBitmap0() const;
    virtual void setBitmap0(int bitmap0);

    virtual int getBitmap1() const;
    virtual void setBitmap1(int bitmap1);

    virtual int getFanIndegree0() const;
    virtual void setFanIndegree0(int fanIndegree0);

    virtual int getFanIndegree1() const;
    virtual void setFanIndegree1(int fanIndegree1);

    virtual size_t getFlagsArraySize() const;
    virtual bool getFlags(size_t k) const;
    virtual void setFlags(size_t k, bool flags);

    virtual int getAggregatorIndex() const;
    virtual void setAggregatorIndex(int aggregatorIndex);

    virtual int getJobIdSeqNumber() const;
    virtual void setJobIdSeqNumber(int jobIdSeqNumber);

    virtual int getWorkerNumber() const;
    virtual void setWorkerNumber(int workerNumber);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const ATPHeader& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, ATPHeader& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>simnet/mod/Packet.msg:79</tt> by opp_msgtool.
 * <pre>
 * class MTATPHeader extends TcpLikeHeader
 * {
 *     byteLength = 14 + 20 + 16 + 4;
 *     int timer;
 *     int workerNumber;
 *     int aggCounter;
 *     bool flags[5];
 *     int aggregatorIndex;
 *     int jobIdSeqNumber;
 *     int treeCost;
 * }
 * </pre>
 */
class MTATPHeader : public ::TcpLikeHeader
{
  protected:
    int timer = 0;
    int workerNumber = 0;
    int aggCounter = 0;
    bool flags[5];
    int aggregatorIndex = 0;
    int jobIdSeqNumber = 0;
    int treeCost = 0;

  private:
    void copy(const MTATPHeader& other);

  protected:
    bool operator==(const MTATPHeader&) = delete;

  public:
    MTATPHeader(const char *name=nullptr);
    MTATPHeader(const MTATPHeader& other);
    virtual ~MTATPHeader();
    MTATPHeader& operator=(const MTATPHeader& other);
    virtual MTATPHeader *dup() const override {return new MTATPHeader(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int getTimer() const;
    virtual void setTimer(int timer);

    virtual int getWorkerNumber() const;
    virtual void setWorkerNumber(int workerNumber);

    virtual int getAggCounter() const;
    virtual void setAggCounter(int aggCounter);

    virtual size_t getFlagsArraySize() const;
    virtual bool getFlags(size_t k) const;
    virtual void setFlags(size_t k, bool flags);

    virtual int getAggregatorIndex() const;
    virtual void setAggregatorIndex(int aggregatorIndex);

    virtual int getJobIdSeqNumber() const;
    virtual void setJobIdSeqNumber(int jobIdSeqNumber);

    virtual int getTreeCost() const;
    virtual void setTreeCost(int treeCost);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const MTATPHeader& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, MTATPHeader& obj) {obj.parsimUnpack(b);}


namespace omnetpp {

template<> inline Packet *fromAnyPtr(any_ptr ptr) { return check_and_cast<Packet*>(ptr.get<cObject>()); }
template<> inline EthernetMacHeader *fromAnyPtr(any_ptr ptr) { return check_and_cast<EthernetMacHeader*>(ptr.get<cObject>()); }
template<> inline IpHeader *fromAnyPtr(any_ptr ptr) { return check_and_cast<IpHeader*>(ptr.get<cObject>()); }
template<> inline UdpHeader *fromAnyPtr(any_ptr ptr) { return check_and_cast<UdpHeader*>(ptr.get<cObject>()); }
template<> inline TcpLikeHeader *fromAnyPtr(any_ptr ptr) { return check_and_cast<TcpLikeHeader*>(ptr.get<cObject>()); }
template<> inline ATPHeader *fromAnyPtr(any_ptr ptr) { return check_and_cast<ATPHeader*>(ptr.get<cObject>()); }
template<> inline MTATPHeader *fromAnyPtr(any_ptr ptr) { return check_and_cast<MTATPHeader*>(ptr.get<cObject>()); }

}  // namespace omnetpp

#endif // ifndef __PACKET_M_H

