#pragma once
#include <set>

#include "ConnectionApp.h"
#include "simnet/mod/cong/CongAlgo.h"

enum TcpState_t
{
    OPEN,
    FIN_WAIT_1, // After send out FIN
    FIN_WAIT_2, // received ACK to FIN but not otherside's FIN
    TIME_WAIT, // received otherside's FIN
    CLOSE_WAIT,
    CLOSED,
    LAST_ACK
};

enum TcpEvent_t
{
    SEND_FIN,
    SEND_ACK,
    RECV_FIN,
    RECV_ACK
};

class CongApp : public ConnectionApp
{
public:
    void setDestAddr(IntAddress addr) {
        destAddr = addr;
        if (getEnvir()->isGUI()) // ! this is only good for debug in gui
            par("destAddress") = destAddr;
    };
    const IntAddress getDestAddr() const { return destAddr;};
    struct TxItem
    {
        bool is_sent{false};
        bool is_resend_already{false};
        int resend_timer{0};
        SeqNumber seq;
        B pktSize;
        simtime_t sendTime;
        Packet* pkt;
        std::vector<IntAddress> destAddresses{};
        explicit TxItem() {
            seq = -1;
            pktSize = 0;
            pkt = nullptr;
        }
        explicit TxItem(Packet* pk) {
            seq = pk->getSeqNumber();
            pktSize = pk->getByteLength();
            pkt = pk;
            resend_timer = maxDisorderNumber;
        }
       explicit TxItem(const TxItem& other) {
           seq = other.seq;
           pktSize = other.pktSize;
           is_sent = other.is_sent;
           is_resend_already = other.is_resend_already;
           resend_timer = other.resend_timer;
           pkt = other.pkt;
       }
    };
    friend std::ostream& operator<<(std::ostream& os, const TxItem& item) {
        os << item.seq << ",";
        return os;
    }

protected:
    // inherited functions
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;
    virtual void handleParameterChange(const char *parameterName) override;

protected:
    virtual void onReceivedNewPacket(Packet* pk);
    virtual void onReceivedDuplicatedPacket(Packet* pk);
    virtual void confirmSeqNumber(const Packet* pk);
    virtual void confirmAckNumber(const Packet* pk);
    virtual void onConnectionClose();
    virtual Packet* createDataPacket(B packetBytes) {
        throw cRuntimeError("you must override this function");
    };
    virtual void resend(TxItem& item);

protected:
    void sendPendingData();
    void setField(Packet* pk);
    void insertTxBuffer(Packet* pk);
    void deleteFromTxBuffer(SeqNumber seq) {
        ASSERT(txBuffer.find(seq) != txBuffer.end());
        delete txBuffer.at(seq).pkt;
        txBuffer.erase(seq);
    }
    void insertRxBuffer(Packet* pk);
    void resetState();
    bool bindRemote();


protected:
    const SeqNumber& getNextSeq() const {return nextSeq;};
    void incrementNextSeqBy(B segmentSize) const {nextSeq += segmentSize;};
    const SeqNumber& getNextSentSeq() const {return nextSentSeq;};
    const SeqNumber& getNextAckSeq() const {return nextAckSeq;};
    const SeqNumber& getNextAskedSeq() const {return nextAskedSeq;};

    const simtime_t& getCurrentBaseRTT() const {return currentBaseRTT;};
    const double& getMaxSendingRate() const {return bandwidth;}

protected:
    std::vector<IntAddress> destAddresses;
    IntAddress destAddr{INVALID_ADDRESS};
    PortNumber destPort{INVALID_PORT};

    B messageLength{0};
    static int maxDisorderNumber;
    int currentRound{0};

    opp_component_ptr<CongAlgo> cong;
    TcpState_t tcpState{CLOSED};
    // ! signals
    static simsignal_t cwndSignal;
    static simsignal_t fctSignal;
    static simsignal_t idealFctSignal;
    static simsignal_t flowSizeSignal;
    static simsignal_t rttSignal;
    static simsignal_t inflightBytesSignal; // for debug

protected:
    std::map<SeqNumber, TxItem> txBuffer;
    std::map<SeqNumber, Packet*> rxBuffer;

private:
    // ! state
    mutable SeqNumber nextSeq{0};     // ! when a new packet insert txBuffer
    SeqNumber nextSentSeq{0}; // ! the next seq actually will be sent out
    SeqNumber nextAckSeq{0};  // ! the next seq I want
    SeqNumber nextAskedSeq{0}; // ! the next seq the other side want
    B resentBytes{0};
    // B sentBytes{0};

//    B confirmedResendBytes{0}; // bytes that are acked by resending packets
//    B confirmedRedundantBytes{0}; // bytes resend more than once
    simtime_t estimatedRTT;

    // SeqNumber markSeq{0};

    simtime_t currentBaseRTT{0};

    double bandwidth;

    cMessage *RTOTimeout = nullptr;

public:
    virtual ~CongApp();

private:
    void connectionDataArrived(Connection *connection, Packet* pk) override;
    void resendTimeoutSeqs();
    void sendFirstTime(TxItem& item);
    void setBeforeSentOut(TxItem& item);
    B inflightBytes() {return nextSentSeq - nextAskedSeq;};
};
