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
    void SetSendersNum(int number);
    void Recv(Packet* pk);
    int GetDestAddr() const;
    int GetLocalAddr() const;

public:
    void Init(int src, int dest, int group=-1, uint32_t initCwnd=1, uint32_t initSSThresh=INT32_MAX);
    ~Socket();

private:
    uint32_t AvailableWindow() const;
    void SendPendingData();

private:
    int packetNumber{0};
    int packetBytes{0};

    //addresses
    int m_addr{-1};
    int m_destAddress{-1};
    int m_groupAddr{-1};
    int m_sendersNum{-1};
    std::map<int, int> m_sendersCounter; // seq-sendersCounter
    TcpCongestionOps* m_cong = nullptr; //congestion control algo
    TcpSocketState* m_tcb = nullptr; // Transmission control block

protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;

};

