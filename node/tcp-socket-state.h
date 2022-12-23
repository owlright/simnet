#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#pragma once
#include <omnetpp.h>
using namespace omnetpp;

class TcpSocketState
{
public:
    /**
     * \brief Definition of the Congestion state machine
     *
     * The design of this state machine is taken from Linux v4.0, but it has been
     * maintained in the Linux mainline from ages. It basically avoids to maintain
     * a lot of boolean variables, and it allows to check the transitions from
     * different algorithm in a cleaner way.
     *
     * These states represent the situation from a congestion control point of view:
     * in fact, apart the CA_OPEN state, the other states represent a situation in
     * which there is a congestion, and different actions should be taken,
     * depending on the case.
     *
     */
    typedef enum
    {
        CA_OPEN,     //!< Normal state, no dubious events
        CA_DISORDER, //!< In all the respects it is "Open",
                     //!< but requires a bit more attention. It is entered when we see some SACKs or
                     //!< dupacks. It is split of "Open".
        CA_CWR,      //!< cWnd was reduced due to some congestion notification event, such as ECN,
                     //!< ICMP source quench, local device congestion.
        CA_RECOVERY, //!< CWND was reduced, we are fast-retransmitting.
        CA_LOSS,     //!< CWND was reduced due to RTO timeout or SACK reneging.
        CA_LAST_STATE //!< Used only in debug messages
    } TcpCongState_t;

    typedef enum
    {
        CA_EVENT_TX_START,        //!< first transmit when no packets in flight
        CA_EVENT_CWND_RESTART,    //!< congestion window restart. Not triggered
        CA_EVENT_COMPLETE_CWR,    //!< end of congestion recovery
        CA_EVENT_LOSS,            //!< loss timeout
        CA_EVENT_ECN_NO_CE,       //!< ECT set, but not CE marked. Not triggered
        CA_EVENT_ECN_IS_CE,       //!< received CE marked IP packet. Not triggered
        CA_EVENT_DELAYED_ACK,     //!< Delayed ack is sent
        CA_EVENT_NON_DELAYED_ACK, //!< Non-delayed ack is sent
    } TcpCAEvent_t;
    // uint32_t m_seq{0}; // current data packet seq
    uint32_t m_lastAckedSeq{0}; // packet seq has already been acked by now
    uint32_t m_ackSeq{0}; // ack seq
    uint32_t m_initialCwnd{INT32_MAX};
    uint32_t m_cWnd{0}; // * Congestion window
    uint32_t m_ssThresh{UINT32_MAX}; //!< Slow start threshold
    uint32_t m_nextTxSequence{0}; //!< Next seqnum to be sent (SND.NXT)
    uint32_t m_obWnd{0}; //! observe window
    // Segment
    uint32_t m_segmentSize{0};          //!< Segment size
    uint32_t m_acked{0}; // acked packets number
    uint32_t m_lost{0};
    uint32_t m_retrans{0};
    uint32_t m_sentSize{0}; // sent packets number
    uint32_t m_bytesInFlight{0}; // sent - acked
    // uint32_t m_ackedBytesEcn{0};
    TcpCongState_t m_congState{CA_OPEN}; //!< State in the Congestion state machine
};
