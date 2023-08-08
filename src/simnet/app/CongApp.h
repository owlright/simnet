#pragma once
#include <set>
#include "simnet/mod/cong/CongAlgo.h"
#include "SocketApp.h"

enum TcpState_t
{
    OPEN,
    FIN_WAIT_1, // After send out FIN
    FIN_WAIT_2, // received FINACK but not FIN
    TIME_WAIT,
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

class CongApp : public UnicastApp
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
        Packet* pkt;
        TxItem() {
            seq = -1;
            pktSize = 0;
            pkt = nullptr;
        }
        TxItem(Packet* pk) {
            seq = pk->getSeqNumber();
            pktSize = pk->getByteLength();
            pkt = pk;
            resend_timer = maxDisorderNumber;
        }
    };
    friend std::ostream& operator<<(std::ostream& os, const TxItem item) {
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
    virtual void onReceivedData(Packet* pk);
    virtual void confirmSeqNumber(const Packet* pk);
    virtual void confirmAckNumber(const Packet* pk);

    virtual Packet* createDataPacket(B packetBytes) {
        throw cRuntimeError("you must override this function");
    };
    void sendPendingData();
    void resendOldestSeq();
    B inflightBytes() {return nextSentSeq - nextAskedSeq;};

    void setField(Packet* pk);
    void insertTxBuffer(Packet* pk);
    void insertRxBuffer(Packet* pk);
    void clearOldSeqInRxBuffer();

    void resetState();
    bool bindRemote();
    virtual void onConnectionClose();

protected:

    // std::map<SeqNumber, TxItem> getTxBufferCopy() {return txBuffer;}
    const SeqNumber& getNextSeq() const {return nextSeq;};
    const SeqNumber& getNextSentSeq() const {return nextSentSeq;};
    bool isInRxBuffer(SeqNumber seq) {return rxBuffer.find(seq) != rxBuffer.end();};
//    const B& getConfirmedNormalBytes() const {return nextAskedSeq;};
//    const B& getConfirmedResendBytes() const {return confirmedResendBytes;};
    const simtime_t& getCurrentBaseRTT() const {return currentBaseRTT;};
    const double& getMaxSendingRate() const {return bandwidth;}
    void setPacketBeforeSentOut(Packet* pk);

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
    static simsignal_t fctSignal;
    static simsignal_t idealFctSignal;
    static simsignal_t flowSizeSignal;
    static simsignal_t rttSignal;
    static simsignal_t inflightBytesSignal; // for debug

private:
    // ! state
    SeqNumber nextSeq{0};     // ! when a new packet insert txBuffer
    SeqNumber nextSentSeq{0}; // ! the next seq actually will be sent out
    SeqNumber nextAckSeq{0};  // ! the next seq I want
    SeqNumber nextAskedSeq{0}; // ! the next seq the other side want
    B resentBytes{0};
    // B sentBytes{0};

//    B confirmedResendBytes{0}; // bytes that are acked by resending packets
//    B confirmedRedundantBytes{0}; // bytes resend more than once
    simtime_t estimatedRTT;

    SeqNumber markSeq{0};

    std::map<SeqNumber, TxItem> txBuffer;
    std::map<SeqNumber, Packet*> rxBuffer;

    simtime_t currentBaseRTT{0};

    double bandwidth;

    cMessage *RTOTimeout = nullptr;

public:
    virtual ~CongApp();

private:
    void connectionDataArrived(Connection *connection, cMessage *msg) override;
    // void transitTcpStateOnEvent(const TcpEvent_t& event);
};
