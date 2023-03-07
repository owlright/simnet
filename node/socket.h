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

class Socket : public cSimpleModule {
public:
    void SendData(int packets, int packetBytes);
    void ReceivedAck(Packet* pk);
    void ProcessAck(const uint32_t& ackNumber);
    void ReceivedData(Packet* pk);
    void SendEchoAck(uint32_t ackno, bool detectECN, int groupid);
    // void SetApp(cSimpleModule* const app );
    void SetSendersNum(int number);
    void Recv(Packet* pk);
    int GetDestAddr() const;
    int GetLocalAddr() const;

public:
    // explicit Socket(int src, int dest, uint32_t initCwnd=1, uint32_t initSSThresh=INT32_MAX);
    // explicit Socket(int src, int dest, int group);
    void Init(int src, int dest, int group=-1, uint32_t initCwnd=1, uint32_t initSSThresh=INT32_MAX);
    ~Socket();

private:
    cOutVector cWnd;
    uint32_t AvailableWindow() const;
    void SendPendingData();

private:
    int packetNumber{0};
    int packetBytes{0};
    // Callback Send = nullptr;
    // cSimpleModule* m_app = nullptr;
    //dest address
    int m_addr{-1};
    int m_destAddress{-1};
    int m_groupAddr{-1};
    int m_sendersNum{-1};
    std::map<int, int> m_sendersCounter; // seq-sendersCounter
    //congestion control algo
    TcpCongestionOps* m_cong = nullptr;
    // Transmission control block
    TcpSocketState* m_tcb = nullptr;

protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    // virtual void handleMessage(cMessage *msg) override { throw cRuntimeError("this module doesn't handle messages, it runs only in initialize()"); }
};

