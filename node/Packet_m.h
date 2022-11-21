//
// Generated file, do not edit! Created by opp_msgtool 6.0 from node/Packet.msg.
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
/**
 * Class generated from <tt>node/Packet.msg:15</tt> by opp_msgtool.
 * <pre>
 * //
 * // Represents a packet in the network.
 * //
 * packet Packet
 * {
 *     int srcAddr \@packetData;
 *     int destAddr \@packetData;
 *     int hopCount \@packetData;
 *     long seq \@packetData;
 *     long ackSeq \@packetData;
 * }
 * </pre>
 */
class Packet : public ::omnetpp::cPacket
{
  protected:
    int srcAddr = 0;
    int destAddr = 0;
    int hopCount = 0;
    long seq = 0;
    long ackSeq = 0;

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

    virtual int getSrcAddr() const;
    virtual void setSrcAddr(int srcAddr);

    virtual int getDestAddr() const;
    virtual void setDestAddr(int destAddr);

    virtual int getHopCount() const;
    virtual void setHopCount(int hopCount);

    virtual long getSeq() const;
    virtual void setSeq(long seq);

    virtual long getAckSeq() const;
    virtual void setAckSeq(long ackSeq);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Packet& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Packet& obj) {obj.parsimUnpack(b);}


namespace omnetpp {

template<> inline Packet *fromAnyPtr(any_ptr ptr) { return check_and_cast<Packet*>(ptr.get<cObject>()); }

}  // namespace omnetpp

#endif // ifndef __PACKET_M_H

