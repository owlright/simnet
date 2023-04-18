//
// Generated file, do not edit! Created by opp_msgtool 6.0 from mnet/mod/Packet.msg.
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

Register_Enum(PacketType, (PacketType::ACK, PacketType::DATA));

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
    this->ECN = other.ECN;
    this->ECE = other.ECE;
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
    doParsimPacking(b,this->ECN);
    doParsimPacking(b,this->ECE);
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
    doParsimUnpacking(b,this->ECN);
    doParsimUnpacking(b,this->ECE);
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
        FIELD_ECN,
        FIELD_ECE,
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
    return base ? 8+base->getFieldCount() : 8;
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
        FD_ISEDITABLE,    // FIELD_ECN
        FD_ISEDITABLE,    // FIELD_ECE
    };
    return (field >= 0 && field < 8) ? fieldTypeFlags[field] : 0;
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
        "ECN",
        "ECE",
    };
    return (field >= 0 && field < 8) ? fieldNames[field] : nullptr;
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
    if (strcmp(fieldName, "ECN") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "ECE") == 0) return baseIndex + 7;
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
        "bool",    // FIELD_ECN
        "bool",    // FIELD_ECE
    };
    return (field >= 0 && field < 8) ? fieldTypeStrings[field] : nullptr;
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
        case FIELD_ECN: return bool2string(pp->getECN());
        case FIELD_ECE: return bool2string(pp->getECE());
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
        case FIELD_ECN: pp->setECN(string2bool(value)); break;
        case FIELD_ECE: pp->setECE(string2bool(value)); break;
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
        case FIELD_ECN: return pp->getECN();
        case FIELD_ECE: return pp->getECE();
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
        case FIELD_ECN: pp->setECN(value.boolValue()); break;
        case FIELD_ECE: pp->setECE(value.boolValue()); break;
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

namespace omnetpp {

}  // namespace omnetpp

