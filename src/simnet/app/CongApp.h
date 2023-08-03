#pragma once
#include <set>
#include "simnet/mod/cong/CongAlgo.h"
#include "SocketApp.h"

enum TcpState_t
{
    OPEN,
    FIN_WAIT,
    CLOSE_WAIT,
    CLOSED
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
    virtual void onReceivedAck(const Packet* pk);
    virtual void onReceivedData(const Packet* pk);
    void sendPendingData();
    void resendOldestSeq();
    B inflightBytes() {return nextSentSeq + resentBytes - maxConfirmedSeq;};

    void setField(Packet* pk);
    void insertTxBuffer(Packet* pk);
    void insertRxBuffer(Packet* pk);

    void resetState();
    bool bindRemote();

protected:

    // std::map<SeqNumber, TxItem> getTxBufferCopy() {return txBuffer;}
    const SeqNumber& getNextSeq() const {return nextSeq;};
//    const B& getConfirmedNormalBytes() const {return maxConfirmedSeq;};
//    const B& getConfirmedResendBytes() const {return confirmedResendBytes;};
    const simtime_t& getCurrentBaseRTT() const {return currentBaseRTT;};
    const double& getMaxSendingRate() const {return bandwidth;}

protected:
    std::vector<IntAddress> destAddresses;
    IntAddress destAddr{INVALID_ADDRESS};
    PortNumber destPort{INVALID_PORT};

    B messageLength{0};
    int maxDisorderNumber{0};
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
    SeqNumber nextSeq{0};
    SeqNumber nextSentSeq{0};
    SeqNumber nextAckSeq{0};

    B resentBytes{0};
    // B sentBytes{0};
    SeqNumber maxConfirmedSeq{-1}; // bytes sent and receive ack only once
//    B confirmedResendBytes{0}; // bytes that are acked by resending packets
//    B confirmedRedundantBytes{0}; // bytes resend more than once
    simtime_t estimatedRTT;

    SeqNumber oldestNotAckedSeq{0};
    SeqNumber last_oldestNotAckedSeq{0};

    std::map<SeqNumber, TxItem> txBuffer;
    std::map<SeqNumber, Packet*> rxBuffer;

    simtime_t currentBaseRTT{0};

    double bandwidth;

    cMessage *RTOTimeout = nullptr;

public:
    virtual ~CongApp();

private:
    void connectionDataArrived(Connection *connection, cMessage *msg) override;
    void transitToNextState(const Packet* pk);
};
