#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <omnetpp.h>
#include "tcp-socket-state.h"
#include "tcp-congestion-ops.h"
using namespace omnetpp;

class TcpReno : public TcpCongestionOps{
public:
    void IncreaseWindow(TcpSocketState* tcb) override;
    uint32_t GetSsThresh(const TcpSocketState* tcb, uint32_t bytesInFlight) override;
    void CwndEvent(TcpSocketState* tcb, const TcpSocketState::TcpCAEvent_t newState) override;
    void PktsAcked(TcpSocketState* tcb) override;
    void Init(TcpSocketState* tcb) override;
private:
    /**
     * Slow start phase handler
     * \param tcb Transmission Control Block of the connection
     * \param segmentsAcked count of segments acked
     * \return Number of segments acked minus the difference between the receiver and sender Cwnd
     */
    void SlowStart(TcpSocketState* tcb);
    /**
     * Congestion avoidance phase handler
     * \param tcb Transmission Control Block of the connection
     * \param segmentsAcked count of segments acked
     */
    void CongestionAvoidance(TcpSocketState* tcb);

private:
    cOutVector obcWnd;
    uint32_t m_ackedBytesEcn{0};
    uint32_t m_cWndCnt{0}; //!< Linear increase counter
    uint32_t m_nextSeq;      //!< TCP sequence number threshold for beginning a new observation window
    bool m_nextSeqFlag{false};
};

