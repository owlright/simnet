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
    void Send(int packets, int packetBytes);
    void SetSendersNum(int number);
    void Bind(int srcaddr, int destaddr, int groupaddr);
    int GetDestAddr() const;
    int GetLocalAddr() const;

private:
    // process incoming packet
    void Recv(Packet* pk);
    void ReceivedData(Packet* pk);
    void ReceivedAck(Packet* pk);
    // send packet
    void SendPendingData();
    void SendAck(uint32_t ackno, bool detectECN);
    // help function
    uint32_t AvailableWindow() const;
    void SetPacketCommonField(Packet* pk) const;

    simsignal_t cwndSignal;

private:
    cPar* jitter;
    int packetNumber{0};
    int packetBytes{0};
    int m_recover;

    //addresses
    int m_addr{-1};
    int m_destAddress{-1};
    int m_groupAddr{-1};
    int m_sendersNum{-1};
    std::map<int, int> m_sendersCounter; // seq-sendersCounter
    TcpCongestionOps* m_cong = nullptr; //congestion control algo
    TcpSocketState* m_tcb = nullptr; // Transmission control block

public:
    virtual ~Socket();

protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;

};

