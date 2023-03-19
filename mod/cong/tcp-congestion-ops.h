
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#pragma once
#include "tcp-socket-state.h"
#include "../../common/Defs.h"
class TcpCongestionOps : public cSimpleModule
{
public:

    virtual ~TcpCongestionOps() {};

    /**
     * \brief Set configuration required by congestion control algorithm
     *
     * \param tcb internal congestion state
     */
    virtual void Init(TcpSocketState* tcb [[maybe_unused]])
    {
    }

    /**
     * \brief Get the slow start threshold after a loss event
     *
     * Is guaranteed that the congestion control state (\p TcpAckState_t) is
     * changed BEFORE the invocation of this method.
     * The implementator should return the slow start threshold (and not change
     * it directly) because, in the future, the TCP implementation may require to
     * instantly recover from a loss event (e.g. when there is a network with an high
     * reordering factor).
     *
     * \param tcb internal congestion state
     * \param bytesInFlight total bytes in flight
     * \return Slow start threshold
     */
    virtual uint32_t GetSsThresh(const TcpSocketState* tcb, uint32_t bytesInFlight) = 0;

    /**
     * \brief Congestion avoidance algorithm implementation
     *
     * Mimic the function \pname{cong_avoid} in Linux. New segments have been ACKed,
     * and the congestion control duty is to update the window.
     *
     * The function is allowed to change directly cWnd and/or ssThresh.
     *
     * \param tcb internal congestion state
     * \param segmentsAcked count of segments acked
     */
    virtual void IncreaseWindow(TcpSocketState* tcb) = 0;

    /**
     * \brief Timing information on received ACK
     *
     * The function is called every time an ACK is received (only one time
     * also for cumulative ACKs) and contains timing information. It is
     * optional (congestion controls need not implement it) and the default
     * implementation does nothing.
     *
     * \param tcb internal congestion state
     * \param segmentsAcked count of segments acked
     * \param rtt last rtt
     */
    virtual void PktsAcked(TcpSocketState* tcb)
    {
        //do noting here
    };

    /**
     * \brief Trigger events/calculations on occurrence of congestion window event
     *
     * This function mimics the function \pname{cwnd_event} in Linux.
     * The function is called in case of congestion window events.
     *
     * \param tcb internal congestion state
     * \param event the event which triggered this function
     */
    virtual void CwndEvent(TcpSocketState* tcb, const TcpSocketState::TcpCAEvent_t event){};

    // Present in Linux but not in ns-3 yet:
    /* call when ack arrives (optional) */
    //     void (*in_ack_event)(struct sock *sk, u32 flags);
    /* new value of cwnd after loss (optional) */
    //     u32  (*undo_cwnd)(struct sock *sk);
    /* hook for packet ack accounting (optional) */
    //     void (*pkts_acked)(struct sock *sk, u32 ext, int *attr, union tcp_cc_info *info);

protected:
    virtual void initialize(int stage) override{};
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override { throw cRuntimeError("this module doesn't handle messages, it runs only in initialize()"); }
};
