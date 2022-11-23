#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <omnetpp.h>
#include "tcp-socket-state.h"
using namespace omnetpp;

class TcpReno {
public:
    void IncreaseWindow(TcpSocketState* tcb, uint32_t segmentsAcked);
    uint32_t GetSsThresh(const TcpSocketState* tcb, uint32_t bytesInFlight);
    void CwndEvent(TcpSocketState* tcb, const TcpSocketState::TcpCAEvent_t newState);
    void PktsAcked();
private:
    /**
     * Slow start phase handler
     * \param tcb Transmission Control Block of the connection
     * \param segmentsAcked count of segments acked
     * \return Number of segments acked minus the difference between the receiver and sender Cwnd
     */
    uint32_t SlowStart(TcpSocketState* tcb, uint32_t segmentsAcked);
    /**
     * Congestion avoidance phase handler
     * \param tcb Transmission Control Block of the connection
     * \param segmentsAcked count of segments acked
     */
    void CongestionAvoidance(TcpSocketState* tcb, uint32_t segmentsAcked);

private:
    uint32_t m_cWndCnt{0}; //!< Linear increase counter
};

