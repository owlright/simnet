#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#pragma once
#include <omnetpp.h>
#include "Packet_m.h"
#include "tcp-socket-state.h"
#include "tcp-congestion-ops.h"

using namespace omnetpp;
// typedef std::function<void(cMessage*, const char *, int)> Callback;
// typedef void (*SendFunCb)(cSimpleModule* app, cMessage *msg, const char *gatename, int gateindex);

class Socket : public cModule {
protected:
    cOutVector cwnd;
public:
    typedef enum
    {
        ACK,
        DATA,
        LAST
    } PacketType;
public:
    void SendData(int packets, int packetBytes);
    void ReceivedAck(Packet* pk);
    void ProcessAck(Packet* pk);
    void ReceivedData(Packet* pk);
    void SendEchoAck(uint32_t ackno, bool detectECN);
    void SetApp(cSimpleModule* const app );
    // void EnterCwr();
    void Recv(Packet* pk);
    // void SetSendCb(Callback f);
    // void SetFlowSize(int packets, int packetBytes);
    int GetDestAddr() const;
public:
    explicit Socket(int src, int dest, uint32_t initCwnd=1, uint32_t initSSThresh=INT32_MAX);
    ~Socket();
private:
    uint32_t AvailableWindow() const;
    void Send();
    // void Retransmit(uint32_t seq);
    // simsignal_t packetsSentCountSignal;
    // void SetInitialCwnd(uint32_t cwnd);
    // void SetInitialSSThresh(uint32_t cwnd);
private:
    int packetNumber;
    int packetBytes;
    // Callback Send = nullptr;
    cSimpleModule* m_app = nullptr;
    //dest address
    int m_addr;
    int m_destAddress;
    //congestion control algo
    TcpCongestionOps* m_cong = nullptr;
    // Transmission control block
    TcpSocketState* m_tcb = nullptr;
};

