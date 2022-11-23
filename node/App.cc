//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <vector>
#include <omnetpp.h>
#include "Packet_m.h"

using namespace omnetpp;
struct FlowItem {
    bool isDirectionIn; // in or out
    long seq; // the packet sequence has been confirmed by now
} ;
std::ostream& operator<<(std::ostream& os, const FlowItem& fl)
{
    std::string direction = fl.isDirectionIn ? "in" : "out";
    os << "direction = " << direction  << "  flow_seq =" << fl.seq; // no endl!
    return os;
}

class TcpSocketState : public cObject
{
public:
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
    uint32_t m_initialCwnd;
    uint32_t m_cWnd{0}; //!< Congestion window
    uint32_t m_ssThresh{0}; //!< Slow start threshold
    // Segment
    uint32_t m_segmentSize{0};          //!< Segment size
    uint32_t m_acked{0};
    uint32_t m_lost{0};
    uint32_t m_retrans{0};
    uint32_t m_sentSize{0};
    uint32_t m_bytesInFlight{0};
};



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

void
TcpReno::IncreaseWindow(TcpSocketState* tcb, uint32_t segmentsAcked)
{
    // Linux tcp_in_slow_start() condition
    if (tcb->m_cWnd < tcb->m_ssThresh)
    {
        EV_DEBUG << "In slow start, m_cWnd " << tcb->m_cWnd << " m_ssThresh " << tcb->m_ssThresh << endl;
        segmentsAcked = SlowStart(tcb, segmentsAcked);
    }
    else
    {
        EV_DEBUG  << "In cong. avoidance, m_cWnd " << tcb->m_cWnd << " m_ssThresh "
                                                   << tcb->m_ssThresh;
        CongestionAvoidance(tcb, segmentsAcked);
    }
}

uint32_t
TcpReno::SlowStart(TcpSocketState* tcb, uint32_t segmentsAcked)
{
    if (segmentsAcked >= 1)
    {
        uint32_t sndCwnd = tcb->m_cWnd;
        tcb->m_cWnd =
            std::min((sndCwnd + (segmentsAcked * tcb->m_segmentSize)), (uint32_t)tcb->m_ssThresh);
        EV << "In SlowStart, updated to cwnd " << tcb->m_cWnd << " ssthresh "
                                                     << tcb->m_ssThresh << endl;
        return segmentsAcked - ((tcb->m_cWnd - sndCwnd) / tcb->m_segmentSize);
    }

    return 0;
}

void
TcpReno::CongestionAvoidance(TcpSocketState* tcb, uint32_t segmentsAcked)
{

    uint32_t w = tcb->m_cWnd / tcb->m_segmentSize;

    // Floor w to 1 if w == 0
    if (w == 0)
    {
        w = 1;
    }

    EV_DEBUG << "w in segments " << w << " m_cWndCnt " << m_cWndCnt << " segments acked "
                                  << segmentsAcked;
    if (m_cWndCnt >= w)
    {
        m_cWndCnt = 0;
        tcb->m_cWnd += tcb->m_segmentSize;
        EV_DEBUG << "Adding 1 segment to m_cWnd" << endl;
    }

    m_cWndCnt += segmentsAcked;
    EV_DEBUG << "Adding 1 segment to m_cWndCnt";
    if (m_cWndCnt >= w)
    {
        uint32_t delta = m_cWndCnt / w;

        m_cWndCnt -= delta * w;
        tcb->m_cWnd += delta * tcb->m_segmentSize;
        EV_DEBUG << "Subtracting delta * w from m_cWndCnt " << delta * w << endl;
    }
    EV_DEBUG << "At end of CongestionAvoidance(), m_cWnd: " << tcb->m_cWnd
                                                             << " m_cWndCnt: " << m_cWndCnt << endl;
}

uint32_t
TcpReno::GetSsThresh(const TcpSocketState* state, uint32_t bytesInFlight)
{
    // NS_LOG_FUNCTION(this << state << bytesInFlight);

    // In Linux, it is written as:  return max(tp->snd_cwnd >> 1U, 2U);
    return std::max<uint32_t>(2 * state->m_segmentSize, state->m_cWnd / 2);
}

class Socket {
public:
    void SetInitialCwnd(uint32_t cwnd);
    void SetInitialSSThresh(uint32_t cwnd);
    void SendData(Packet*, int destAddr);
    void ReceieveAck(Packet*);
    void ProcessAck(const uint32_t ackNumber, uint32_t currentDelivered, const uint32_t oldSeq);
public:
    Socket(){m_cong=new TcpReno();}
    ~Socket(){delete m_cong;}
private:
    uint32_t AvailableWindow() const;
private:
    //congestion control algo
    TcpReno* m_cong;
    //tcp congestion state
    TcpSocketState* m_tcb;
};

uint32_t
Socket::AvailableWindow() const
{
    uint32_t win = m_tcb->m_cWnd;
    uint32_t inflight = m_tcb->m_sentSize - m_tcb->m_acked + m_tcb->m_retrans;
    m_tcb->m_bytesInFlight = inflight;
    return (inflight > win) ? 0 : win - inflight;
}

void
Socket::SendData(Packet*, int destAddr)
{
    auto availableWindow = AvailableWindow();
    if (availableWindow > 0) {
        // send the packet
    }
    UpdateRttHistory(seq, sz, isRetransmission);
}
void
Socket::ReceieveAck(Packet*)
{
    m_cong->CwndEvent(m_tcb, );
    m_cong->CongControl();
}
/**
 * Generates traffic for the network.
 */
class App : public cSimpleModule
{
  private:
    // configuration
    int myAddress;
    int packetTotalCount;
    int packetLossCounter;
    bool disableSending;
    std::vector<int> destAddresses;
    cPar *sendIATime;
    cPar *packetLengthBytes;
    Socket* m_socket;
    //packet type
    int ACK = 0;
    int DATA = 1;
private:
    std::map<int, FlowItem*> ftable;
    // state
    cMessage *generatePacket = nullptr;
    int pkCounter;
    long ack;
    long seq;
    // signals
    simsignal_t endToEndDelaySignal;
    simsignal_t hopCountSignal;
    simsignal_t sourceAddressSignal;

  public:
    virtual ~App();

private:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(App);

App::~App()
{
    cancelAndDelete(generatePacket);
    delete m_socket;
}

void App::initialize()
{
    m_socket = new Socket();
    m_socket->SetInitialCwnd(1);
    m_socket->SetInitialSSThresh(UINT32_MAX);


    myAddress = par("address");
    packetTotalCount = par("packetTotalCount");
    packetLengthBytes = &par("packetLength");
    sendIATime = &par("sendIaTime");  // volatile parameter
    packetLossCounter = 0;
    pkCounter = 0;
    seq = 0;

    std::string appType = getParentModule()->par("nodeType");
    disableSending = appType == "Switch" || appType == "Sink";
    if (disableSending)
        EV << "node type is "<< appType << " disable generating packets." << endl;

    WATCH(pkCounter);
    WATCH(myAddress);
    WATCH_VECTOR(destAddresses);
    WATCH_PTRMAP(ftable);
    const char *destAddressesPar = par("destAddresses");
    cStringTokenizer tokenizer(destAddressesPar);
    const char *token;
    while ((token = tokenizer.nextToken()) != nullptr)
        destAddresses.push_back(atoi(token));

    if (destAddresses.size() == 0)
        throw cRuntimeError("At least one address must be specified in the destAddresses parameter!");
    if (!disableSending) {
        generatePacket = new cMessage("nextPacket");
        scheduleAt(sendIATime->doubleValue(), generatePacket);
    }

    endToEndDelaySignal = registerSignal("endToEndDelay");
    hopCountSignal = registerSignal("hopCount");
    sourceAddressSignal = registerSignal("sourceAddress");
}

void App::handleMessage(cMessage *msg)
{
    if (msg == generatePacket) {
        // Sending packet
        int destAddress = destAddresses[intuniform(0, destAddresses.size()-1)];

        char pkname[40];
        sprintf(pkname, "pk-%d-to-%d-#%ld", myAddress, destAddress, seq);
        pkCounter++;
        EV << "generating packet " << pkname << endl;

        Packet *pk = new Packet(pkname);
        pk->setByteLength(packetLengthBytes->intValue());
        pk->setKind(DATA);
        pk->setSeq(pkCounter-1);
        pk->setSrcAddr(myAddress);
        pk->setDestAddr(destAddress);
        send(pk, "out");
        ftable[myAddress] = new FlowItem();
        ftable[myAddress]->isDirectionIn = false;
        ftable[myAddress]->seq = 0;
        EV << *ftable[myAddress] << endl;
        // scheduleAt(simTime() + sendIATime->doubleValue(), generatePacket);
        // if (hasGUI())
        //     getParentModule()->bubble("Generating packet...");
    }
    else {
        // Handle incoming packet
        Packet *pk = check_and_cast<Packet *>(msg);
        EV << "received packet " << pk->getName() << " after " << pk->getHopCount() << "hops" << endl;
        emit(endToEndDelaySignal, simTime() - pk->getCreationTime());
        emit(hopCountSignal, pk->getHopCount());
        emit(sourceAddressSignal, pk->getSrcAddr());
        int senderAddr = pk->getSrcAddr();
        short packetKind = pk->getKind();
        if (senderAddr==myAddress) { // the packet send from myself
            delete pk;
            return;
        }
        if (hasGUI())
            getParentModule()->bubble("Arrived!");

        char pkname[40];
        if (packetKind == DATA) {
            int outPortIndex = pk->par("outGateIndex");
            double rate = getParentModule()->gate("port$o", outPortIndex)->getChannel()->par("datarate");
            EV << pk->getName() <<"comes from port " << outPortIndex << " channelrate is " << rate <<endl;
            auto pkseq = pk->getSeq();
            auto it = ftable.find(senderAddr);
            if (it != ftable.end()) {
                if (it->second->seq <= pkseq)
                    it->second->seq = pkseq + 1; // ask for next packet
            }
            else {
                // deal with the first data packet
                ftable[senderAddr] = new FlowItem();
                ftable[senderAddr]->isDirectionIn = true;
                ftable[senderAddr]->seq = pkseq + 1;
            }

            sprintf(pkname, "Ack-%d-to-%d-#%ld", myAddress, senderAddr, ftable[senderAddr]->seq);
            Packet *ackpk = new Packet(pkname);
            ackpk->setByteLength(1);
            ackpk->setKind(ACK);
            ackpk->setAckSeq(ftable[senderAddr]->seq);
            ackpk->setSrcAddr(myAddress);
            ackpk->setDestAddr(senderAddr);

            if (hasGUI())
                getParentModule()->bubble("Generating ack packet...");
            send(ackpk, "out");
            delete pk;
       }
       else if (packetKind == ACK) {
            if (pkCounter-packetLossCounter < packetTotalCount) {
                auto pkAckSeq = pk->getAckSeq();
                auto it = ftable.find(myAddress);
                assert(!it->second->isDirectionIn); // the flow must start with this app.
                if (pkAckSeq == it->second->seq + 1) {
                    it->second->seq++;
                    pkCounter++;
                }
                else {
                    packetLossCounter++;
                }
                char pkname[40];
                sprintf(pkname, "pk-%d-to-%d-#%ld", myAddress, senderAddr, it->second->seq);
                Packet *npk = new Packet(pkname);
                npk->setByteLength(packetLengthBytes->intValue());
                npk->setKind(DATA);
                npk->setSeq(it->second->seq);
                npk->setSrcAddr(myAddress);
                npk->setDestAddr(senderAddr);
                if (hasGUI())
                    getParentModule()->bubble("Generating next data packet...");
                send(npk, "out");
            }
            delete pk;
       }
    }
}

