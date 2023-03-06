#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#pragma once
#include <omnetpp.h>
#include "Packet_m.h"
#include "tcp-socket-state.h"
#include "tcp-congestion-ops.h"
#include "../common/Defs.h"
using namespace omnetpp;
// typedef std::function<void(cMessage*, const char *, int)> Callback;
// typedef void (*SendFunCb)(cSimpleModule* app, cMessage *msg, const char *gatename, int gateindex);

class Socket : public cModule {
public:
    // typedef enum
    // {
    //     ACK,
    //     DATA,
    //     LAST
    // } PacketType;
public:
    void SendData(int packets, int packetBytes);
    void ReceivedAck(Packet* pk);
    void ProcessAck(const uint32_t& ackNumber);
    void ReceivedData(Packet* pk);
    void SendEchoAck(uint32_t ackno, bool detectECN, int groupid);
    void SetApp(cSimpleModule* const app );
    void SetSendersNum(int number);
    // void EnterCwr();
    void Recv(Packet* pk);
    // void SetSendCb(Callback f);
    // void SetFlowSize(int packets, int packetBytes);
    int GetDestAddr() const;
public:
    explicit Socket(int src, int dest, uint32_t initCwnd=1, uint32_t initSSThresh=INT32_MAX);
    explicit Socket(int src, int dest, int group);
    ~Socket();
private:
    cOutVector cWnd;
    uint32_t AvailableWindow() const;
    void SendPendingData();
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
    int m_groupAddr;
    int m_sendersNum;
    std::map<int, int> m_sendersCounter; // seq-sendersCounter
    //congestion control algo
    TcpCongestionOps* m_cong = nullptr;
    // Transmission control block
    TcpSocketState* m_tcb = nullptr;

};

