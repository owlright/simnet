#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <omnetpp.h>
#include "Packet_m.h"
#include "tcp-socket-state.h"
#include "tcp-reno.h"
#include <functional>
using namespace omnetpp;
typedef std::function<void(cMessage*, const char *, int)> Callback;
// typedef void (*SendFunCb)(cSimpleModule* app, cMessage *msg, const char *gatename, int gateindex);
class Socket {
public:
    void SendData(int packets, int packetBytes);
    // void ReceieveAck(Packet*);
    void ProcessAck(Packet* pk);
    void ProcessData(Packet* pk);
    void SetSendCb(Callback f);

    int GetDestAddr() const;
public:
    explicit Socket(int src, int dest, uint32_t initCwnd, uint32_t initSSThresh);
    ~Socket(){delete m_cong;}
private:
    uint32_t AvailableWindow() const;
    void SetInitialCwnd(uint32_t cwnd);
    void SetInitialSSThresh(uint32_t cwnd);
private:
    Callback Send = nullptr;
    //dest address
    int m_addr;
    int m_destAddress;
    //congestion control algo
    TcpReno* m_cong;
    // Transmission control block
    TcpSocketState* m_tcb;
};

