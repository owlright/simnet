#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <omnetpp.h>
#include "tcp-reno.h"
using namespace omnetpp;

class TcpDctcp : public TcpReno {
public:
    //Documented in base class
    uint32_t GetSsThresh(const TcpSocketState* tcb, uint32_t bytesInFlight) override;
    void PktsAcked(TcpSocketState* tcb) override;
    void CwndEvent(TcpSocketState* tcb, const TcpSocketState::TcpCAEvent_t newState) override;
    virtual void Init(TcpSocketState* tcb [[maybe_unused]]) override;
    void Reset(TcpSocketState* tcb);

    TcpDctcp();
private:
//    cOutVector obcWnd; //! observe window
    uint32_t m_nextSeq;//!< TCP sequence number threshold for beginning a new observation window
    bool m_nextSeqFlag{false};
    uint32_t m_ackedPacketsEcn{0};

    uint32_t m_ackedPacketsTotal{0};
    double m_alpha;
    double m_g;
};
