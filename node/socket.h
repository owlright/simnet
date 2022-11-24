#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#pragma once
#include <omnetpp.h>
#include "Packet_m.h"
#include "tcp-socket-state.h"
#include "tcp-reno.h"

using namespace omnetpp;
// typedef std::function<void(cMessage*, const char *, int)> Callback;
// typedef void (*SendFunCb)(cSimpleModule* app, cMessage *msg, const char *gatename, int gateindex);

class Socket : public cObject {
public:
    void SendData(int packets, int packetBytes);
    // void ReceieveAck(Packet*);
    void ProcessAck(Packet* pk);
    void ProcessData(Packet* pk);
    void SetApp(cSimpleModule* const app );
    // void SetSendCb(Callback f);
    // void SetFlowSize(int packets, int packetBytes);
    int GetDestAddr() const;
public:
    explicit Socket(int src, int dest, uint32_t initCwnd, uint32_t initSSThresh);
    ~Socket();
private:
    uint32_t AvailableWindow() const;
    void Send();
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
    TcpReno* m_cong = nullptr;
    // Transmission control block
    TcpSocketState* m_tcb = nullptr;
};

