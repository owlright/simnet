//
// Generated file, do not edit! Created by opp_msgtool 6.0 from ../../src/simnet/mod/Packet.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "Packet_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Enum(PacketType, (PacketType::ACK, PacketType::DATA, PacketType::REMIND));

Register_Class(Packet)

Packet::Packet(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

Packet::Packet(const Packet& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

Packet::~Packet()
{
}

Packet& Packet::operator=(const Packet& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void Packet::copy(const Packet& other)
{
    this->connectionId = other.connectionId;
    this->srcAddr = other.srcAddr;
    this->localPort = other.localPort;
    this->destAddr = other.destAddr;
    this->destPort = other.destPort;
    this->seqNumber = other.seqNumber;
    this->timer = other.timer;
    this->aggCounter = other.aggCounter;
    this->aggNumber = other.aggNumber;
    this->receivedBytes = other.receivedBytes;
    this->startTime = other.startTime;
    this->transmitTime = other.transmitTime;
    this->queueTime = other.queueTime;
    this->ECN = other.ECN;
    this->ECE = other.ECE;
    this->isFlowFinished_ = other.isFlowFinished_;
}

void Packet::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->connectionId);
    doParsimPacking(b,this->srcAddr);
    doParsimPacking(b,this->localPort);
    doParsimPacking(b,this->destAddr);
    doParsimPacking(b,this->destPort);
    doParsimPacking(b,this->seqNumber);
    doParsimPacking(b,this->timer);
    doParsimPacking(b,this->aggCounter);
    doParsimPacking(b,this->aggNumber);
    doParsimPacking(b,this->receivedBytes);
    doParsimPacking(b,this->startTime);
    doParsimPacking(b,this->transmitTime);
    doParsimPacking(b,this->queueTime);
    doParsimPacking(b,this->ECN);
    doParsimPacking(b,this->ECE);
    doParsimPacking(b,this->isFlowFinished_);
}

void Packet::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->connectionId);
    doParsimUnpacking(b,this->srcAddr);
    doParsimUnpacking(b,this->localPort);
    doParsimUnpacking(b,this->destAddr);
    doParsimUnpacking(b,this->destPort);
    doParsimUnpacking(b,this->seqNumber);
    doParsimUnpacking(b,this->timer);
    doParsimUnpacking(b,this->aggCounter);
    doParsimUnpacking(b,this->aggNumber);
    doParsimUnpacking(b,this->receivedBytes);
    doParsimUnpacking(b,this->startTime);
    doParsimUnpacking(b,this->transmitTime);
    doParsimUnpacking(b,this->queueTime);
    doParsimUnpacking(b,this->ECN);
    doParsimUnpacking(b,this->ECE);
    doParsimUnpacking(b,this->isFlowFinished_);
}

int64_t Packet::getConnectionId() const
{
    return this->connectionId;
}

void Packet::setConnectionId(int64_t connectionId)
{
    this->connectionId = connectionId;
}

int64_t Packet::getSrcAddr() const
{
    return this->srcAddr;
}

void Packet::setSrcAddr(int64_t srcAddr)
{
    this->srcAddr = srcAddr;
}

uint16_t Packet::getLocalPort() const
{
    return this->localPort;
}

void Packet::setLocalPort(uint16_t localPort)
{
    this->localPort = localPort;
}

int64_t Packet::getDestAddr() const
{
    return this->destAddr;
}

void Packet::setDestAddr(int64_t destAddr)
{
    this->destAddr = destAddr;
}

uint16_t Packet::getDestPort() const
{
    return this->destPort;
}

void Packet::setDestPort(uint16_t destPort)
{
    this->destPort = destPort;
}

int64_t Packet::getSeqNumber() const
{
    return this->seqNumber;
}

void Packet::setSeqNumber(int64_t seqNumber)
{
    this->seqNumber = seqNumber;
}

int64_t Packet::getTimer() const
{
    return this->timer;
}

void Packet::setTimer(int64_t timer)
{
    this->timer = timer;
}

int32_t Packet::getAggCounter() const
{
    return this->aggCounter;
}

void Packet::setAggCounter(int32_t aggCounter)
{
    this->aggCounter = aggCounter;
}

int32_t Packet::getAggNumber() const
{
    return this->aggNumber;
}

void Packet::setAggNumber(int32_t aggNumber)
{
    this->aggNumber = aggNumber;
}

int64_t Packet::getReceivedBytes() const
{
    return this->receivedBytes;
}

void Packet::setReceivedBytes(int64_t receivedBytes)
{
    this->receivedBytes = receivedBytes;
}

double Packet::getStartTime() const
{
    return this->startTime;
}

void Packet::setStartTime(double startTime)
{
    this->startTime = startTime;
}

double Packet::getTransmitTime() const
{
    return this->transmitTime;
}

void Packet::setTransmitTime(double transmitTime)
{
    this->transmitTime = transmitTime;
}

double Packet::getQueueTime() const
{
    return this->queueTime;
}

void Packet::setQueueTime(double queueTime)
{
    this->queueTime = queueTime;
}

bool Packet::getECN() const
{
    return this->ECN;
}

void Packet::setECN(bool ECN)
{
    this->ECN = ECN;
}

bool Packet::getECE() const
{
    return this->ECE;
}

void Packet::setECE(bool ECE)
{
    this->ECE = ECE;
}

bool Packet::isFlowFinished() const
{
    return this->isFlowFinished_;
}

void Packet::setIsFlowFinished(bool isFlowFinished)
{
    this->isFlowFinished_ = isFlowFinished;
}

class PacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_connectionId,
        FIELD_srcAddr,
        FIELD_localPort,
        FIELD_destAddr,
        FIELD_destPort,
        FIELD_seqNumber,
        FIELD_timer,
        FIELD_aggCounter,
        FIELD_aggNumber,
        FIELD_receivedBytes,
        FIELD_startTime,
        FIELD_transmitTime,
        FIELD_queueTime,
        FIELD_ECN,
        FIELD_ECE,
        FIELD_isFlowFinished,
    };
  public:
    PacketDescriptor();
    virtual ~PacketDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(PacketDescriptor)

PacketDescriptor::PacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(Packet)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

PacketDescriptor::~PacketDescriptor()
{
    delete[] propertyNames;
}

bool PacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Packet *>(obj)!=nullptr;
}

const char **PacketDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *PacketDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int PacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 16+base->getFieldCount() : 16;
}

unsigned int PacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_connectionId
        FD_ISEDITABLE,    // FIELD_srcAddr
        FD_ISEDITABLE,    // FIELD_localPort
        FD_ISEDITABLE,    // FIELD_destAddr
        FD_ISEDITABLE,    // FIELD_destPort
        FD_ISEDITABLE,    // FIELD_seqNumber
        FD_ISEDITABLE,    // FIELD_timer
        FD_ISEDITABLE,    // FIELD_aggCounter
        FD_ISEDITABLE,    // FIELD_aggNumber
        FD_ISEDITABLE,    // FIELD_receivedBytes
        FD_ISEDITABLE,    // FIELD_startTime
        FD_ISEDITABLE,    // FIELD_transmitTime
        FD_ISEDITABLE,    // FIELD_queueTime
        FD_ISEDITABLE,    // FIELD_ECN
        FD_ISEDITABLE,    // FIELD_ECE
        FD_ISEDITABLE,    // FIELD_isFlowFinished
    };
    return (field >= 0 && field < 16) ? fieldTypeFlags[field] : 0;
}

const char *PacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "connectionId",
        "srcAddr",
        "localPort",
        "destAddr",
        "destPort",
        "seqNumber",
        "timer",
        "aggCounter",
        "aggNumber",
        "receivedBytes",
        "startTime",
        "transmitTime",
        "queueTime",
        "ECN",
        "ECE",
        "isFlowFinished",
    };
    return (field >= 0 && field < 16) ? fieldNames[field] : nullptr;
}

int PacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "connectionId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "srcAddr") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "localPort") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "destAddr") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "destPort") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "seqNumber") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "timer") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "aggCounter") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "aggNumber") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "receivedBytes") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "startTime") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "transmitTime") == 0) return baseIndex + 11;
    if (strcmp(fieldName, "queueTime") == 0) return baseIndex + 12;
    if (strcmp(fieldName, "ECN") == 0) return baseIndex + 13;
    if (strcmp(fieldName, "ECE") == 0) return baseIndex + 14;
    if (strcmp(fieldName, "isFlowFinished") == 0) return baseIndex + 15;
    return base ? base->findField(fieldName) : -1;
}

const char *PacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int64_t",    // FIELD_connectionId
        "int64_t",    // FIELD_srcAddr
        "uint16_t",    // FIELD_localPort
        "int64_t",    // FIELD_destAddr
        "uint16_t",    // FIELD_destPort
        "int64_t",    // FIELD_seqNumber
        "int64_t",    // FIELD_timer
        "int32_t",    // FIELD_aggCounter
        "int32_t",    // FIELD_aggNumber
        "int64_t",    // FIELD_receivedBytes
        "double",    // FIELD_startTime
        "double",    // FIELD_transmitTime
        "double",    // FIELD_queueTime
        "bool",    // FIELD_ECN
        "bool",    // FIELD_ECE
        "bool",    // FIELD_isFlowFinished
    };
    return (field >= 0 && field < 16) ? fieldTypeStrings[field] : nullptr;
}

const char **PacketDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *PacketDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int PacketDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    Packet *pp = omnetpp::fromAnyPtr<Packet>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void PacketDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    Packet *pp = omnetpp::fromAnyPtr<Packet>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'Packet'", field);
    }
}

const char *PacketDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    Packet *pp = omnetpp::fromAnyPtr<Packet>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string PacketDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    Packet *pp = omnetpp::fromAnyPtr<Packet>(object); (void)pp;
    switch (field) {
        case FIELD_connectionId: return int642string(pp->getConnectionId());
        case FIELD_srcAddr: return int642string(pp->getSrcAddr());
        case FIELD_localPort: return ulong2string(pp->getLocalPort());
        case FIELD_destAddr: return int642string(pp->getDestAddr());
        case FIELD_destPort: return ulong2string(pp->getDestPort());
        case FIELD_seqNumber: return int642string(pp->getSeqNumber());
        case FIELD_timer: return int642string(pp->getTimer());
        case FIELD_aggCounter: return long2string(pp->getAggCounter());
        case FIELD_aggNumber: return long2string(pp->getAggNumber());
        case FIELD_receivedBytes: return int642string(pp->getReceivedBytes());
        case FIELD_startTime: return double2string(pp->getStartTime());
        case FIELD_transmitTime: return double2string(pp->getTransmitTime());
        case FIELD_queueTime: return double2string(pp->getQueueTime());
        case FIELD_ECN: return bool2string(pp->getECN());
        case FIELD_ECE: return bool2string(pp->getECE());
        case FIELD_isFlowFinished: return bool2string(pp->isFlowFinished());
        default: return "";
    }
}

void PacketDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Packet *pp = omnetpp::fromAnyPtr<Packet>(object); (void)pp;
    switch (field) {
        case FIELD_connectionId: pp->setConnectionId(string2int64(value)); break;
        case FIELD_srcAddr: pp->setSrcAddr(string2int64(value)); break;
        case FIELD_localPort: pp->setLocalPort(string2ulong(value)); break;
        case FIELD_destAddr: pp->setDestAddr(string2int64(value)); break;
        case FIELD_destPort: pp->setDestPort(string2ulong(value)); break;
        case FIELD_seqNumber: pp->setSeqNumber(string2int64(value)); break;
        case FIELD_timer: pp->setTimer(string2int64(value)); break;
        case FIELD_aggCounter: pp->setAggCounter(string2long(value)); break;
        case FIELD_aggNumber: pp->setAggNumber(string2long(value)); break;
        case FIELD_receivedBytes: pp->setReceivedBytes(string2int64(value)); break;
        case FIELD_startTime: pp->setStartTime(string2double(value)); break;
        case FIELD_transmitTime: pp->setTransmitTime(string2double(value)); break;
        case FIELD_queueTime: pp->setQueueTime(string2double(value)); break;
        case FIELD_ECN: pp->setECN(string2bool(value)); break;
        case FIELD_ECE: pp->setECE(string2bool(value)); break;
        case FIELD_isFlowFinished: pp->setIsFlowFinished(string2bool(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Packet'", field);
    }
}

omnetpp::cValue PacketDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    Packet *pp = omnetpp::fromAnyPtr<Packet>(object); (void)pp;
    switch (field) {
        case FIELD_connectionId: return pp->getConnectionId();
        case FIELD_srcAddr: return pp->getSrcAddr();
        case FIELD_localPort: return (omnetpp::intval_t)(pp->getLocalPort());
        case FIELD_destAddr: return pp->getDestAddr();
        case FIELD_destPort: return (omnetpp::intval_t)(pp->getDestPort());
        case FIELD_seqNumber: return pp->getSeqNumber();
        case FIELD_timer: return pp->getTimer();
        case FIELD_aggCounter: return pp->getAggCounter();
        case FIELD_aggNumber: return pp->getAggNumber();
        case FIELD_receivedBytes: return pp->getReceivedBytes();
        case FIELD_startTime: return pp->getStartTime();
        case FIELD_transmitTime: return pp->getTransmitTime();
        case FIELD_queueTime: return pp->getQueueTime();
        case FIELD_ECN: return pp->getECN();
        case FIELD_ECE: return pp->getECE();
        case FIELD_isFlowFinished: return pp->isFlowFinished();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'Packet' as cValue -- field index out of range?", field);
    }
}

void PacketDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Packet *pp = omnetpp::fromAnyPtr<Packet>(object); (void)pp;
    switch (field) {
        case FIELD_connectionId: pp->setConnectionId(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_srcAddr: pp->setSrcAddr(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_localPort: pp->setLocalPort(omnetpp::checked_int_cast<uint16_t>(value.intValue())); break;
        case FIELD_destAddr: pp->setDestAddr(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_destPort: pp->setDestPort(omnetpp::checked_int_cast<uint16_t>(value.intValue())); break;
        case FIELD_seqNumber: pp->setSeqNumber(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_timer: pp->setTimer(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_aggCounter: pp->setAggCounter(omnetpp::checked_int_cast<int32_t>(value.intValue())); break;
        case FIELD_aggNumber: pp->setAggNumber(omnetpp::checked_int_cast<int32_t>(value.intValue())); break;
        case FIELD_receivedBytes: pp->setReceivedBytes(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_startTime: pp->setStartTime(value.doubleValue()); break;
        case FIELD_transmitTime: pp->setTransmitTime(value.doubleValue()); break;
        case FIELD_queueTime: pp->setQueueTime(value.doubleValue()); break;
        case FIELD_ECN: pp->setECN(value.boolValue()); break;
        case FIELD_ECE: pp->setECE(value.boolValue()); break;
        case FIELD_isFlowFinished: pp->setIsFlowFinished(value.boolValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Packet'", field);
    }
}

const char *PacketDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr PacketDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    Packet *pp = omnetpp::fromAnyPtr<Packet>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void PacketDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    Packet *pp = omnetpp::fromAnyPtr<Packet>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Packet'", field);
    }
}

Register_Class(commonHeader)

commonHeader::commonHeader(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
    this->setByteLength(14 + 20);

}

commonHeader::commonHeader(const commonHeader& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

commonHeader::~commonHeader()
{
}

commonHeader& commonHeader::operator=(const commonHeader& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void commonHeader::copy(const commonHeader& other)
{
    this->connectionId = other.connectionId;
    this->srcAddr = other.srcAddr;
    this->destAddr = other.destAddr;
    this->localPort = other.localPort;
    this->destPort = other.destPort;
    this->seqNumber = other.seqNumber;
    this->ECN = other.ECN;
    this->ECE = other.ECE;
    this->receivedBytes = other.receivedBytes;
    this->startTime = other.startTime;
    this->transmitTime = other.transmitTime;
    this->queueTime = other.queueTime;
    this->isFlowFinished_ = other.isFlowFinished_;
}

void commonHeader::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->connectionId);
    doParsimPacking(b,this->srcAddr);
    doParsimPacking(b,this->destAddr);
    doParsimPacking(b,this->localPort);
    doParsimPacking(b,this->destPort);
    doParsimPacking(b,this->seqNumber);
    doParsimPacking(b,this->ECN);
    doParsimPacking(b,this->ECE);
    doParsimPacking(b,this->receivedBytes);
    doParsimPacking(b,this->startTime);
    doParsimPacking(b,this->transmitTime);
    doParsimPacking(b,this->queueTime);
    doParsimPacking(b,this->isFlowFinished_);
}

void commonHeader::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->connectionId);
    doParsimUnpacking(b,this->srcAddr);
    doParsimUnpacking(b,this->destAddr);
    doParsimUnpacking(b,this->localPort);
    doParsimUnpacking(b,this->destPort);
    doParsimUnpacking(b,this->seqNumber);
    doParsimUnpacking(b,this->ECN);
    doParsimUnpacking(b,this->ECE);
    doParsimUnpacking(b,this->receivedBytes);
    doParsimUnpacking(b,this->startTime);
    doParsimUnpacking(b,this->transmitTime);
    doParsimUnpacking(b,this->queueTime);
    doParsimUnpacking(b,this->isFlowFinished_);
}

int64_t commonHeader::getConnectionId() const
{
    return this->connectionId;
}

void commonHeader::setConnectionId(int64_t connectionId)
{
    this->connectionId = connectionId;
}

int64_t commonHeader::getSrcAddr() const
{
    return this->srcAddr;
}

void commonHeader::setSrcAddr(int64_t srcAddr)
{
    this->srcAddr = srcAddr;
}

int64_t commonHeader::getDestAddr() const
{
    return this->destAddr;
}

void commonHeader::setDestAddr(int64_t destAddr)
{
    this->destAddr = destAddr;
}

uint16_t commonHeader::getLocalPort() const
{
    return this->localPort;
}

void commonHeader::setLocalPort(uint16_t localPort)
{
    this->localPort = localPort;
}

uint16_t commonHeader::getDestPort() const
{
    return this->destPort;
}

void commonHeader::setDestPort(uint16_t destPort)
{
    this->destPort = destPort;
}

int64_t commonHeader::getSeqNumber() const
{
    return this->seqNumber;
}

void commonHeader::setSeqNumber(int64_t seqNumber)
{
    this->seqNumber = seqNumber;
}

bool commonHeader::getECN() const
{
    return this->ECN;
}

void commonHeader::setECN(bool ECN)
{
    this->ECN = ECN;
}

bool commonHeader::getECE() const
{
    return this->ECE;
}

void commonHeader::setECE(bool ECE)
{
    this->ECE = ECE;
}

int64_t commonHeader::getReceivedBytes() const
{
    return this->receivedBytes;
}

void commonHeader::setReceivedBytes(int64_t receivedBytes)
{
    this->receivedBytes = receivedBytes;
}

double commonHeader::getStartTime() const
{
    return this->startTime;
}

void commonHeader::setStartTime(double startTime)
{
    this->startTime = startTime;
}

double commonHeader::getTransmitTime() const
{
    return this->transmitTime;
}

void commonHeader::setTransmitTime(double transmitTime)
{
    this->transmitTime = transmitTime;
}

double commonHeader::getQueueTime() const
{
    return this->queueTime;
}

void commonHeader::setQueueTime(double queueTime)
{
    this->queueTime = queueTime;
}

bool commonHeader::isFlowFinished() const
{
    return this->isFlowFinished_;
}

void commonHeader::setIsFlowFinished(bool isFlowFinished)
{
    this->isFlowFinished_ = isFlowFinished;
}

class commonHeaderDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_connectionId,
        FIELD_srcAddr,
        FIELD_destAddr,
        FIELD_localPort,
        FIELD_destPort,
        FIELD_seqNumber,
        FIELD_ECN,
        FIELD_ECE,
        FIELD_receivedBytes,
        FIELD_startTime,
        FIELD_transmitTime,
        FIELD_queueTime,
        FIELD_isFlowFinished,
    };
  public:
    commonHeaderDescriptor();
    virtual ~commonHeaderDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(commonHeaderDescriptor)

commonHeaderDescriptor::commonHeaderDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(commonHeader)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

commonHeaderDescriptor::~commonHeaderDescriptor()
{
    delete[] propertyNames;
}

bool commonHeaderDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<commonHeader *>(obj)!=nullptr;
}

const char **commonHeaderDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *commonHeaderDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int commonHeaderDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 13+base->getFieldCount() : 13;
}

unsigned int commonHeaderDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_connectionId
        FD_ISEDITABLE,    // FIELD_srcAddr
        FD_ISEDITABLE,    // FIELD_destAddr
        FD_ISEDITABLE,    // FIELD_localPort
        FD_ISEDITABLE,    // FIELD_destPort
        FD_ISEDITABLE,    // FIELD_seqNumber
        FD_ISEDITABLE,    // FIELD_ECN
        FD_ISEDITABLE,    // FIELD_ECE
        FD_ISEDITABLE,    // FIELD_receivedBytes
        FD_ISEDITABLE,    // FIELD_startTime
        FD_ISEDITABLE,    // FIELD_transmitTime
        FD_ISEDITABLE,    // FIELD_queueTime
        FD_ISEDITABLE,    // FIELD_isFlowFinished
    };
    return (field >= 0 && field < 13) ? fieldTypeFlags[field] : 0;
}

const char *commonHeaderDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "connectionId",
        "srcAddr",
        "destAddr",
        "localPort",
        "destPort",
        "seqNumber",
        "ECN",
        "ECE",
        "receivedBytes",
        "startTime",
        "transmitTime",
        "queueTime",
        "isFlowFinished",
    };
    return (field >= 0 && field < 13) ? fieldNames[field] : nullptr;
}

int commonHeaderDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "connectionId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "srcAddr") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "destAddr") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "localPort") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "destPort") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "seqNumber") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "ECN") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "ECE") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "receivedBytes") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "startTime") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "transmitTime") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "queueTime") == 0) return baseIndex + 11;
    if (strcmp(fieldName, "isFlowFinished") == 0) return baseIndex + 12;
    return base ? base->findField(fieldName) : -1;
}

const char *commonHeaderDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int64_t",    // FIELD_connectionId
        "int64_t",    // FIELD_srcAddr
        "int64_t",    // FIELD_destAddr
        "uint16_t",    // FIELD_localPort
        "uint16_t",    // FIELD_destPort
        "int64_t",    // FIELD_seqNumber
        "bool",    // FIELD_ECN
        "bool",    // FIELD_ECE
        "int64_t",    // FIELD_receivedBytes
        "double",    // FIELD_startTime
        "double",    // FIELD_transmitTime
        "double",    // FIELD_queueTime
        "bool",    // FIELD_isFlowFinished
    };
    return (field >= 0 && field < 13) ? fieldTypeStrings[field] : nullptr;
}

const char **commonHeaderDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *commonHeaderDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int commonHeaderDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    commonHeader *pp = omnetpp::fromAnyPtr<commonHeader>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void commonHeaderDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    commonHeader *pp = omnetpp::fromAnyPtr<commonHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'commonHeader'", field);
    }
}

const char *commonHeaderDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    commonHeader *pp = omnetpp::fromAnyPtr<commonHeader>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string commonHeaderDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    commonHeader *pp = omnetpp::fromAnyPtr<commonHeader>(object); (void)pp;
    switch (field) {
        case FIELD_connectionId: return int642string(pp->getConnectionId());
        case FIELD_srcAddr: return int642string(pp->getSrcAddr());
        case FIELD_destAddr: return int642string(pp->getDestAddr());
        case FIELD_localPort: return ulong2string(pp->getLocalPort());
        case FIELD_destPort: return ulong2string(pp->getDestPort());
        case FIELD_seqNumber: return int642string(pp->getSeqNumber());
        case FIELD_ECN: return bool2string(pp->getECN());
        case FIELD_ECE: return bool2string(pp->getECE());
        case FIELD_receivedBytes: return int642string(pp->getReceivedBytes());
        case FIELD_startTime: return double2string(pp->getStartTime());
        case FIELD_transmitTime: return double2string(pp->getTransmitTime());
        case FIELD_queueTime: return double2string(pp->getQueueTime());
        case FIELD_isFlowFinished: return bool2string(pp->isFlowFinished());
        default: return "";
    }
}

void commonHeaderDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    commonHeader *pp = omnetpp::fromAnyPtr<commonHeader>(object); (void)pp;
    switch (field) {
        case FIELD_connectionId: pp->setConnectionId(string2int64(value)); break;
        case FIELD_srcAddr: pp->setSrcAddr(string2int64(value)); break;
        case FIELD_destAddr: pp->setDestAddr(string2int64(value)); break;
        case FIELD_localPort: pp->setLocalPort(string2ulong(value)); break;
        case FIELD_destPort: pp->setDestPort(string2ulong(value)); break;
        case FIELD_seqNumber: pp->setSeqNumber(string2int64(value)); break;
        case FIELD_ECN: pp->setECN(string2bool(value)); break;
        case FIELD_ECE: pp->setECE(string2bool(value)); break;
        case FIELD_receivedBytes: pp->setReceivedBytes(string2int64(value)); break;
        case FIELD_startTime: pp->setStartTime(string2double(value)); break;
        case FIELD_transmitTime: pp->setTransmitTime(string2double(value)); break;
        case FIELD_queueTime: pp->setQueueTime(string2double(value)); break;
        case FIELD_isFlowFinished: pp->setIsFlowFinished(string2bool(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'commonHeader'", field);
    }
}

omnetpp::cValue commonHeaderDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    commonHeader *pp = omnetpp::fromAnyPtr<commonHeader>(object); (void)pp;
    switch (field) {
        case FIELD_connectionId: return pp->getConnectionId();
        case FIELD_srcAddr: return pp->getSrcAddr();
        case FIELD_destAddr: return pp->getDestAddr();
        case FIELD_localPort: return (omnetpp::intval_t)(pp->getLocalPort());
        case FIELD_destPort: return (omnetpp::intval_t)(pp->getDestPort());
        case FIELD_seqNumber: return pp->getSeqNumber();
        case FIELD_ECN: return pp->getECN();
        case FIELD_ECE: return pp->getECE();
        case FIELD_receivedBytes: return pp->getReceivedBytes();
        case FIELD_startTime: return pp->getStartTime();
        case FIELD_transmitTime: return pp->getTransmitTime();
        case FIELD_queueTime: return pp->getQueueTime();
        case FIELD_isFlowFinished: return pp->isFlowFinished();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'commonHeader' as cValue -- field index out of range?", field);
    }
}

void commonHeaderDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    commonHeader *pp = omnetpp::fromAnyPtr<commonHeader>(object); (void)pp;
    switch (field) {
        case FIELD_connectionId: pp->setConnectionId(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_srcAddr: pp->setSrcAddr(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_destAddr: pp->setDestAddr(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_localPort: pp->setLocalPort(omnetpp::checked_int_cast<uint16_t>(value.intValue())); break;
        case FIELD_destPort: pp->setDestPort(omnetpp::checked_int_cast<uint16_t>(value.intValue())); break;
        case FIELD_seqNumber: pp->setSeqNumber(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_ECN: pp->setECN(value.boolValue()); break;
        case FIELD_ECE: pp->setECE(value.boolValue()); break;
        case FIELD_receivedBytes: pp->setReceivedBytes(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_startTime: pp->setStartTime(value.doubleValue()); break;
        case FIELD_transmitTime: pp->setTransmitTime(value.doubleValue()); break;
        case FIELD_queueTime: pp->setQueueTime(value.doubleValue()); break;
        case FIELD_isFlowFinished: pp->setIsFlowFinished(value.boolValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'commonHeader'", field);
    }
}

const char *commonHeaderDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr commonHeaderDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    commonHeader *pp = omnetpp::fromAnyPtr<commonHeader>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void commonHeaderDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    commonHeader *pp = omnetpp::fromAnyPtr<commonHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'commonHeader'", field);
    }
}

Register_Class(ATPHeader)

ATPHeader::ATPHeader(const char *name) : ::commonHeader(name)
{
    this->setByteLength(14 + 20 + 16 + 8);

    std::fill(this->flags, this->flags + 6, false);
}

ATPHeader::ATPHeader(const ATPHeader& other) : ::commonHeader(other)
{
    copy(other);
}

ATPHeader::~ATPHeader()
{
}

ATPHeader& ATPHeader::operator=(const ATPHeader& other)
{
    if (this == &other) return *this;
    ::commonHeader::operator=(other);
    copy(other);
    return *this;
}

void ATPHeader::copy(const ATPHeader& other)
{
    this->bitmap0 = other.bitmap0;
    this->bitmap1 = other.bitmap1;
    this->fanIndegree0 = other.fanIndegree0;
    this->fanIndegree1 = other.fanIndegree1;
    for (size_t i = 0; i < 6; i++) {
        this->flags[i] = other.flags[i];
    }
    this->aggregatorIndex = other.aggregatorIndex;
    this->jobIdSeqNumber = other.jobIdSeqNumber;
    this->workerNumber = other.workerNumber;
}

void ATPHeader::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::commonHeader::parsimPack(b);
    doParsimPacking(b,this->bitmap0);
    doParsimPacking(b,this->bitmap1);
    doParsimPacking(b,this->fanIndegree0);
    doParsimPacking(b,this->fanIndegree1);
    doParsimArrayPacking(b,this->flags,6);
    doParsimPacking(b,this->aggregatorIndex);
    doParsimPacking(b,this->jobIdSeqNumber);
    doParsimPacking(b,this->workerNumber);
}

void ATPHeader::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::commonHeader::parsimUnpack(b);
    doParsimUnpacking(b,this->bitmap0);
    doParsimUnpacking(b,this->bitmap1);
    doParsimUnpacking(b,this->fanIndegree0);
    doParsimUnpacking(b,this->fanIndegree1);
    doParsimArrayUnpacking(b,this->flags,6);
    doParsimUnpacking(b,this->aggregatorIndex);
    doParsimUnpacking(b,this->jobIdSeqNumber);
    doParsimUnpacking(b,this->workerNumber);
}

int ATPHeader::getBitmap0() const
{
    return this->bitmap0;
}

void ATPHeader::setBitmap0(int bitmap0)
{
    this->bitmap0 = bitmap0;
}

int ATPHeader::getBitmap1() const
{
    return this->bitmap1;
}

void ATPHeader::setBitmap1(int bitmap1)
{
    this->bitmap1 = bitmap1;
}

int ATPHeader::getFanIndegree0() const
{
    return this->fanIndegree0;
}

void ATPHeader::setFanIndegree0(int fanIndegree0)
{
    this->fanIndegree0 = fanIndegree0;
}

int ATPHeader::getFanIndegree1() const
{
    return this->fanIndegree1;
}

void ATPHeader::setFanIndegree1(int fanIndegree1)
{
    this->fanIndegree1 = fanIndegree1;
}

size_t ATPHeader::getFlagsArraySize() const
{
    return 6;
}

bool ATPHeader::getFlags(size_t k) const
{
    if (k >= 6) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)6, (unsigned long)k);
    return this->flags[k];
}

void ATPHeader::setFlags(size_t k, bool flags)
{
    if (k >= 6) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)6, (unsigned long)k);
    this->flags[k] = flags;
}

int ATPHeader::getAggregatorIndex() const
{
    return this->aggregatorIndex;
}

void ATPHeader::setAggregatorIndex(int aggregatorIndex)
{
    this->aggregatorIndex = aggregatorIndex;
}

int ATPHeader::getJobIdSeqNumber() const
{
    return this->jobIdSeqNumber;
}

void ATPHeader::setJobIdSeqNumber(int jobIdSeqNumber)
{
    this->jobIdSeqNumber = jobIdSeqNumber;
}

int ATPHeader::getWorkerNumber() const
{
    return this->workerNumber;
}

void ATPHeader::setWorkerNumber(int workerNumber)
{
    this->workerNumber = workerNumber;
}

class ATPHeaderDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_bitmap0,
        FIELD_bitmap1,
        FIELD_fanIndegree0,
        FIELD_fanIndegree1,
        FIELD_flags,
        FIELD_aggregatorIndex,
        FIELD_jobIdSeqNumber,
        FIELD_workerNumber,
    };
  public:
    ATPHeaderDescriptor();
    virtual ~ATPHeaderDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(ATPHeaderDescriptor)

ATPHeaderDescriptor::ATPHeaderDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(ATPHeader)), "commonHeader")
{
    propertyNames = nullptr;
}

ATPHeaderDescriptor::~ATPHeaderDescriptor()
{
    delete[] propertyNames;
}

bool ATPHeaderDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ATPHeader *>(obj)!=nullptr;
}

const char **ATPHeaderDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ATPHeaderDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ATPHeaderDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 8+base->getFieldCount() : 8;
}

unsigned int ATPHeaderDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_bitmap0
        FD_ISEDITABLE,    // FIELD_bitmap1
        FD_ISEDITABLE,    // FIELD_fanIndegree0
        FD_ISEDITABLE,    // FIELD_fanIndegree1
        FD_ISARRAY | FD_ISEDITABLE,    // FIELD_flags
        FD_ISEDITABLE,    // FIELD_aggregatorIndex
        FD_ISEDITABLE,    // FIELD_jobIdSeqNumber
        FD_ISEDITABLE,    // FIELD_workerNumber
    };
    return (field >= 0 && field < 8) ? fieldTypeFlags[field] : 0;
}

const char *ATPHeaderDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "bitmap0",
        "bitmap1",
        "fanIndegree0",
        "fanIndegree1",
        "flags",
        "aggregatorIndex",
        "jobIdSeqNumber",
        "workerNumber",
    };
    return (field >= 0 && field < 8) ? fieldNames[field] : nullptr;
}

int ATPHeaderDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "bitmap0") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "bitmap1") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "fanIndegree0") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "fanIndegree1") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "flags") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "aggregatorIndex") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "jobIdSeqNumber") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "workerNumber") == 0) return baseIndex + 7;
    return base ? base->findField(fieldName) : -1;
}

const char *ATPHeaderDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_bitmap0
        "int",    // FIELD_bitmap1
        "int",    // FIELD_fanIndegree0
        "int",    // FIELD_fanIndegree1
        "bool",    // FIELD_flags
        "int",    // FIELD_aggregatorIndex
        "int",    // FIELD_jobIdSeqNumber
        "int",    // FIELD_workerNumber
    };
    return (field >= 0 && field < 8) ? fieldTypeStrings[field] : nullptr;
}

const char **ATPHeaderDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *ATPHeaderDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int ATPHeaderDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ATPHeader *pp = omnetpp::fromAnyPtr<ATPHeader>(object); (void)pp;
    switch (field) {
        case FIELD_flags: return 6;
        default: return 0;
    }
}

void ATPHeaderDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ATPHeader *pp = omnetpp::fromAnyPtr<ATPHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ATPHeader'", field);
    }
}

const char *ATPHeaderDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ATPHeader *pp = omnetpp::fromAnyPtr<ATPHeader>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ATPHeaderDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ATPHeader *pp = omnetpp::fromAnyPtr<ATPHeader>(object); (void)pp;
    switch (field) {
        case FIELD_bitmap0: return long2string(pp->getBitmap0());
        case FIELD_bitmap1: return long2string(pp->getBitmap1());
        case FIELD_fanIndegree0: return long2string(pp->getFanIndegree0());
        case FIELD_fanIndegree1: return long2string(pp->getFanIndegree1());
        case FIELD_flags: return bool2string(pp->getFlags(i));
        case FIELD_aggregatorIndex: return long2string(pp->getAggregatorIndex());
        case FIELD_jobIdSeqNumber: return long2string(pp->getJobIdSeqNumber());
        case FIELD_workerNumber: return long2string(pp->getWorkerNumber());
        default: return "";
    }
}

void ATPHeaderDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ATPHeader *pp = omnetpp::fromAnyPtr<ATPHeader>(object); (void)pp;
    switch (field) {
        case FIELD_bitmap0: pp->setBitmap0(string2long(value)); break;
        case FIELD_bitmap1: pp->setBitmap1(string2long(value)); break;
        case FIELD_fanIndegree0: pp->setFanIndegree0(string2long(value)); break;
        case FIELD_fanIndegree1: pp->setFanIndegree1(string2long(value)); break;
        case FIELD_flags: pp->setFlags(i,string2bool(value)); break;
        case FIELD_aggregatorIndex: pp->setAggregatorIndex(string2long(value)); break;
        case FIELD_jobIdSeqNumber: pp->setJobIdSeqNumber(string2long(value)); break;
        case FIELD_workerNumber: pp->setWorkerNumber(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ATPHeader'", field);
    }
}

omnetpp::cValue ATPHeaderDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ATPHeader *pp = omnetpp::fromAnyPtr<ATPHeader>(object); (void)pp;
    switch (field) {
        case FIELD_bitmap0: return pp->getBitmap0();
        case FIELD_bitmap1: return pp->getBitmap1();
        case FIELD_fanIndegree0: return pp->getFanIndegree0();
        case FIELD_fanIndegree1: return pp->getFanIndegree1();
        case FIELD_flags: return pp->getFlags(i);
        case FIELD_aggregatorIndex: return pp->getAggregatorIndex();
        case FIELD_jobIdSeqNumber: return pp->getJobIdSeqNumber();
        case FIELD_workerNumber: return pp->getWorkerNumber();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ATPHeader' as cValue -- field index out of range?", field);
    }
}

void ATPHeaderDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ATPHeader *pp = omnetpp::fromAnyPtr<ATPHeader>(object); (void)pp;
    switch (field) {
        case FIELD_bitmap0: pp->setBitmap0(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_bitmap1: pp->setBitmap1(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_fanIndegree0: pp->setFanIndegree0(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_fanIndegree1: pp->setFanIndegree1(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_flags: pp->setFlags(i,value.boolValue()); break;
        case FIELD_aggregatorIndex: pp->setAggregatorIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_jobIdSeqNumber: pp->setJobIdSeqNumber(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_workerNumber: pp->setWorkerNumber(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ATPHeader'", field);
    }
}

const char *ATPHeaderDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr ATPHeaderDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ATPHeader *pp = omnetpp::fromAnyPtr<ATPHeader>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ATPHeaderDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ATPHeader *pp = omnetpp::fromAnyPtr<ATPHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ATPHeader'", field);
    }
}

Register_Class(MTATPHeader)

MTATPHeader::MTATPHeader(const char *name) : ::commonHeader(name)
{
    this->setByteLength(14 + 20 + 16 + 4);

    std::fill(this->flags, this->flags + 5, false);
}

MTATPHeader::MTATPHeader(const MTATPHeader& other) : ::commonHeader(other)
{
    copy(other);
}

MTATPHeader::~MTATPHeader()
{
}

MTATPHeader& MTATPHeader::operator=(const MTATPHeader& other)
{
    if (this == &other) return *this;
    ::commonHeader::operator=(other);
    copy(other);
    return *this;
}

void MTATPHeader::copy(const MTATPHeader& other)
{
    this->timer = other.timer;
    this->workerNumber = other.workerNumber;
    this->aggCounter = other.aggCounter;
    for (size_t i = 0; i < 5; i++) {
        this->flags[i] = other.flags[i];
    }
    this->aggregatorIndex = other.aggregatorIndex;
    this->jobIdSeqNumber = other.jobIdSeqNumber;
    this->treeCost = other.treeCost;
}

void MTATPHeader::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::commonHeader::parsimPack(b);
    doParsimPacking(b,this->timer);
    doParsimPacking(b,this->workerNumber);
    doParsimPacking(b,this->aggCounter);
    doParsimArrayPacking(b,this->flags,5);
    doParsimPacking(b,this->aggregatorIndex);
    doParsimPacking(b,this->jobIdSeqNumber);
    doParsimPacking(b,this->treeCost);
}

void MTATPHeader::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::commonHeader::parsimUnpack(b);
    doParsimUnpacking(b,this->timer);
    doParsimUnpacking(b,this->workerNumber);
    doParsimUnpacking(b,this->aggCounter);
    doParsimArrayUnpacking(b,this->flags,5);
    doParsimUnpacking(b,this->aggregatorIndex);
    doParsimUnpacking(b,this->jobIdSeqNumber);
    doParsimUnpacking(b,this->treeCost);
}

int MTATPHeader::getTimer() const
{
    return this->timer;
}

void MTATPHeader::setTimer(int timer)
{
    this->timer = timer;
}

int MTATPHeader::getWorkerNumber() const
{
    return this->workerNumber;
}

void MTATPHeader::setWorkerNumber(int workerNumber)
{
    this->workerNumber = workerNumber;
}

int MTATPHeader::getAggCounter() const
{
    return this->aggCounter;
}

void MTATPHeader::setAggCounter(int aggCounter)
{
    this->aggCounter = aggCounter;
}

size_t MTATPHeader::getFlagsArraySize() const
{
    return 5;
}

bool MTATPHeader::getFlags(size_t k) const
{
    if (k >= 5) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)5, (unsigned long)k);
    return this->flags[k];
}

void MTATPHeader::setFlags(size_t k, bool flags)
{
    if (k >= 5) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)5, (unsigned long)k);
    this->flags[k] = flags;
}

int MTATPHeader::getAggregatorIndex() const
{
    return this->aggregatorIndex;
}

void MTATPHeader::setAggregatorIndex(int aggregatorIndex)
{
    this->aggregatorIndex = aggregatorIndex;
}

int MTATPHeader::getJobIdSeqNumber() const
{
    return this->jobIdSeqNumber;
}

void MTATPHeader::setJobIdSeqNumber(int jobIdSeqNumber)
{
    this->jobIdSeqNumber = jobIdSeqNumber;
}

int MTATPHeader::getTreeCost() const
{
    return this->treeCost;
}

void MTATPHeader::setTreeCost(int treeCost)
{
    this->treeCost = treeCost;
}

class MTATPHeaderDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_timer,
        FIELD_workerNumber,
        FIELD_aggCounter,
        FIELD_flags,
        FIELD_aggregatorIndex,
        FIELD_jobIdSeqNumber,
        FIELD_treeCost,
    };
  public:
    MTATPHeaderDescriptor();
    virtual ~MTATPHeaderDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(MTATPHeaderDescriptor)

MTATPHeaderDescriptor::MTATPHeaderDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(MTATPHeader)), "commonHeader")
{
    propertyNames = nullptr;
}

MTATPHeaderDescriptor::~MTATPHeaderDescriptor()
{
    delete[] propertyNames;
}

bool MTATPHeaderDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MTATPHeader *>(obj)!=nullptr;
}

const char **MTATPHeaderDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *MTATPHeaderDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int MTATPHeaderDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 7+base->getFieldCount() : 7;
}

unsigned int MTATPHeaderDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_timer
        FD_ISEDITABLE,    // FIELD_workerNumber
        FD_ISEDITABLE,    // FIELD_aggCounter
        FD_ISARRAY | FD_ISEDITABLE,    // FIELD_flags
        FD_ISEDITABLE,    // FIELD_aggregatorIndex
        FD_ISEDITABLE,    // FIELD_jobIdSeqNumber
        FD_ISEDITABLE,    // FIELD_treeCost
    };
    return (field >= 0 && field < 7) ? fieldTypeFlags[field] : 0;
}

const char *MTATPHeaderDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "timer",
        "workerNumber",
        "aggCounter",
        "flags",
        "aggregatorIndex",
        "jobIdSeqNumber",
        "treeCost",
    };
    return (field >= 0 && field < 7) ? fieldNames[field] : nullptr;
}

int MTATPHeaderDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "timer") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "workerNumber") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "aggCounter") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "flags") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "aggregatorIndex") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "jobIdSeqNumber") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "treeCost") == 0) return baseIndex + 6;
    return base ? base->findField(fieldName) : -1;
}

const char *MTATPHeaderDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_timer
        "int",    // FIELD_workerNumber
        "int",    // FIELD_aggCounter
        "bool",    // FIELD_flags
        "int",    // FIELD_aggregatorIndex
        "int",    // FIELD_jobIdSeqNumber
        "int",    // FIELD_treeCost
    };
    return (field >= 0 && field < 7) ? fieldTypeStrings[field] : nullptr;
}

const char **MTATPHeaderDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MTATPHeaderDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MTATPHeaderDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    MTATPHeader *pp = omnetpp::fromAnyPtr<MTATPHeader>(object); (void)pp;
    switch (field) {
        case FIELD_flags: return 5;
        default: return 0;
    }
}

void MTATPHeaderDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    MTATPHeader *pp = omnetpp::fromAnyPtr<MTATPHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'MTATPHeader'", field);
    }
}

const char *MTATPHeaderDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    MTATPHeader *pp = omnetpp::fromAnyPtr<MTATPHeader>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MTATPHeaderDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    MTATPHeader *pp = omnetpp::fromAnyPtr<MTATPHeader>(object); (void)pp;
    switch (field) {
        case FIELD_timer: return long2string(pp->getTimer());
        case FIELD_workerNumber: return long2string(pp->getWorkerNumber());
        case FIELD_aggCounter: return long2string(pp->getAggCounter());
        case FIELD_flags: return bool2string(pp->getFlags(i));
        case FIELD_aggregatorIndex: return long2string(pp->getAggregatorIndex());
        case FIELD_jobIdSeqNumber: return long2string(pp->getJobIdSeqNumber());
        case FIELD_treeCost: return long2string(pp->getTreeCost());
        default: return "";
    }
}

void MTATPHeaderDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    MTATPHeader *pp = omnetpp::fromAnyPtr<MTATPHeader>(object); (void)pp;
    switch (field) {
        case FIELD_timer: pp->setTimer(string2long(value)); break;
        case FIELD_workerNumber: pp->setWorkerNumber(string2long(value)); break;
        case FIELD_aggCounter: pp->setAggCounter(string2long(value)); break;
        case FIELD_flags: pp->setFlags(i,string2bool(value)); break;
        case FIELD_aggregatorIndex: pp->setAggregatorIndex(string2long(value)); break;
        case FIELD_jobIdSeqNumber: pp->setJobIdSeqNumber(string2long(value)); break;
        case FIELD_treeCost: pp->setTreeCost(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'MTATPHeader'", field);
    }
}

omnetpp::cValue MTATPHeaderDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    MTATPHeader *pp = omnetpp::fromAnyPtr<MTATPHeader>(object); (void)pp;
    switch (field) {
        case FIELD_timer: return pp->getTimer();
        case FIELD_workerNumber: return pp->getWorkerNumber();
        case FIELD_aggCounter: return pp->getAggCounter();
        case FIELD_flags: return pp->getFlags(i);
        case FIELD_aggregatorIndex: return pp->getAggregatorIndex();
        case FIELD_jobIdSeqNumber: return pp->getJobIdSeqNumber();
        case FIELD_treeCost: return pp->getTreeCost();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'MTATPHeader' as cValue -- field index out of range?", field);
    }
}

void MTATPHeaderDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    MTATPHeader *pp = omnetpp::fromAnyPtr<MTATPHeader>(object); (void)pp;
    switch (field) {
        case FIELD_timer: pp->setTimer(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_workerNumber: pp->setWorkerNumber(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_aggCounter: pp->setAggCounter(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_flags: pp->setFlags(i,value.boolValue()); break;
        case FIELD_aggregatorIndex: pp->setAggregatorIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_jobIdSeqNumber: pp->setJobIdSeqNumber(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_treeCost: pp->setTreeCost(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'MTATPHeader'", field);
    }
}

const char *MTATPHeaderDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr MTATPHeaderDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    MTATPHeader *pp = omnetpp::fromAnyPtr<MTATPHeader>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void MTATPHeaderDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    MTATPHeader *pp = omnetpp::fromAnyPtr<MTATPHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'MTATPHeader'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

