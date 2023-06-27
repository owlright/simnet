//
// Generated file, do not edit! Created by opp_msgtool 6.0 from simnet/mod/Packet.msg.
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

Register_Enum(PacketType, (PacketType::ACK, PacketType::DATA, PacketType::AGG, PacketType::REMIND, PacketType::MACK, PacketType::NOUSE));

Register_Class(EthernetMacHeader)

EthernetMacHeader::EthernetMacHeader(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
    this->setByteLength(14);

}

EthernetMacHeader::EthernetMacHeader(const EthernetMacHeader& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

EthernetMacHeader::~EthernetMacHeader()
{
}

EthernetMacHeader& EthernetMacHeader::operator=(const EthernetMacHeader& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void EthernetMacHeader::copy(const EthernetMacHeader& other)
{
    this->destinationMacAddr = other.destinationMacAddr;
    this->sourceMacAddr = other.sourceMacAddr;
    this->etherType = other.etherType;
}

void EthernetMacHeader::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->destinationMacAddr);
    doParsimPacking(b,this->sourceMacAddr);
    doParsimPacking(b,this->etherType);
}

void EthernetMacHeader::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->destinationMacAddr);
    doParsimUnpacking(b,this->sourceMacAddr);
    doParsimUnpacking(b,this->etherType);
}

uint32_t EthernetMacHeader::getDestinationMacAddr() const
{
    return this->destinationMacAddr;
}

void EthernetMacHeader::setDestinationMacAddr(uint32_t destinationMacAddr)
{
    this->destinationMacAddr = destinationMacAddr;
}

uint32_t EthernetMacHeader::getSourceMacAddr() const
{
    return this->sourceMacAddr;
}

void EthernetMacHeader::setSourceMacAddr(uint32_t sourceMacAddr)
{
    this->sourceMacAddr = sourceMacAddr;
}

uint32_t EthernetMacHeader::getEtherType() const
{
    return this->etherType;
}

void EthernetMacHeader::setEtherType(uint32_t etherType)
{
    this->etherType = etherType;
}

class EthernetMacHeaderDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_destinationMacAddr,
        FIELD_sourceMacAddr,
        FIELD_etherType,
    };
  public:
    EthernetMacHeaderDescriptor();
    virtual ~EthernetMacHeaderDescriptor();

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

Register_ClassDescriptor(EthernetMacHeaderDescriptor)

EthernetMacHeaderDescriptor::EthernetMacHeaderDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(EthernetMacHeader)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

EthernetMacHeaderDescriptor::~EthernetMacHeaderDescriptor()
{
    delete[] propertyNames;
}

bool EthernetMacHeaderDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<EthernetMacHeader *>(obj)!=nullptr;
}

const char **EthernetMacHeaderDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *EthernetMacHeaderDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int EthernetMacHeaderDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int EthernetMacHeaderDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_destinationMacAddr
        FD_ISEDITABLE,    // FIELD_sourceMacAddr
        FD_ISEDITABLE,    // FIELD_etherType
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *EthernetMacHeaderDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "destinationMacAddr",
        "sourceMacAddr",
        "etherType",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int EthernetMacHeaderDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "destinationMacAddr") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "sourceMacAddr") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "etherType") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *EthernetMacHeaderDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint32_t",    // FIELD_destinationMacAddr
        "uint32_t",    // FIELD_sourceMacAddr
        "uint32_t",    // FIELD_etherType
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **EthernetMacHeaderDescriptor::getFieldPropertyNames(int field) const
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

const char *EthernetMacHeaderDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int EthernetMacHeaderDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    EthernetMacHeader *pp = omnetpp::fromAnyPtr<EthernetMacHeader>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void EthernetMacHeaderDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    EthernetMacHeader *pp = omnetpp::fromAnyPtr<EthernetMacHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'EthernetMacHeader'", field);
    }
}

const char *EthernetMacHeaderDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    EthernetMacHeader *pp = omnetpp::fromAnyPtr<EthernetMacHeader>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string EthernetMacHeaderDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    EthernetMacHeader *pp = omnetpp::fromAnyPtr<EthernetMacHeader>(object); (void)pp;
    switch (field) {
        case FIELD_destinationMacAddr: return ulong2string(pp->getDestinationMacAddr());
        case FIELD_sourceMacAddr: return ulong2string(pp->getSourceMacAddr());
        case FIELD_etherType: return ulong2string(pp->getEtherType());
        default: return "";
    }
}

void EthernetMacHeaderDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    EthernetMacHeader *pp = omnetpp::fromAnyPtr<EthernetMacHeader>(object); (void)pp;
    switch (field) {
        case FIELD_destinationMacAddr: pp->setDestinationMacAddr(string2ulong(value)); break;
        case FIELD_sourceMacAddr: pp->setSourceMacAddr(string2ulong(value)); break;
        case FIELD_etherType: pp->setEtherType(string2ulong(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EthernetMacHeader'", field);
    }
}

omnetpp::cValue EthernetMacHeaderDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    EthernetMacHeader *pp = omnetpp::fromAnyPtr<EthernetMacHeader>(object); (void)pp;
    switch (field) {
        case FIELD_destinationMacAddr: return (omnetpp::intval_t)(pp->getDestinationMacAddr());
        case FIELD_sourceMacAddr: return (omnetpp::intval_t)(pp->getSourceMacAddr());
        case FIELD_etherType: return (omnetpp::intval_t)(pp->getEtherType());
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'EthernetMacHeader' as cValue -- field index out of range?", field);
    }
}

void EthernetMacHeaderDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    EthernetMacHeader *pp = omnetpp::fromAnyPtr<EthernetMacHeader>(object); (void)pp;
    switch (field) {
        case FIELD_destinationMacAddr: pp->setDestinationMacAddr(omnetpp::checked_int_cast<uint32_t>(value.intValue())); break;
        case FIELD_sourceMacAddr: pp->setSourceMacAddr(omnetpp::checked_int_cast<uint32_t>(value.intValue())); break;
        case FIELD_etherType: pp->setEtherType(omnetpp::checked_int_cast<uint32_t>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EthernetMacHeader'", field);
    }
}

const char *EthernetMacHeaderDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr EthernetMacHeaderDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    EthernetMacHeader *pp = omnetpp::fromAnyPtr<EthernetMacHeader>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void EthernetMacHeaderDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    EthernetMacHeader *pp = omnetpp::fromAnyPtr<EthernetMacHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'EthernetMacHeader'", field);
    }
}

Register_Class(IPv4Header)

IPv4Header::IPv4Header(const char *name) : ::EthernetMacHeader(name)
{
    this->setByteLength(14 + 20);

}

IPv4Header::IPv4Header(const IPv4Header& other) : ::EthernetMacHeader(other)
{
    copy(other);
}

IPv4Header::~IPv4Header()
{
}

IPv4Header& IPv4Header::operator=(const IPv4Header& other)
{
    if (this == &other) return *this;
    ::EthernetMacHeader::operator=(other);
    copy(other);
    return *this;
}

void IPv4Header::copy(const IPv4Header& other)
{
    this->version = other.version;
    this->IHL = other.IHL;
    this->DSCP = other.DSCP;
    this->ipECN = other.ipECN;
    this->totalLength = other.totalLength;
    this->identification = other.identification;
    this->ipFlags = other.ipFlags;
    this->fragmentOffset = other.fragmentOffset;
    this->TTL = other.TTL;
    this->protocol = other.protocol;
    this->checksum = other.checksum;
    this->srcAddr = other.srcAddr;
    this->destAddr = other.destAddr;
    this->options = other.options;
}

void IPv4Header::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::EthernetMacHeader::parsimPack(b);
    doParsimPacking(b,this->version);
    doParsimPacking(b,this->IHL);
    doParsimPacking(b,this->DSCP);
    doParsimPacking(b,this->ipECN);
    doParsimPacking(b,this->totalLength);
    doParsimPacking(b,this->identification);
    doParsimPacking(b,this->ipFlags);
    doParsimPacking(b,this->fragmentOffset);
    doParsimPacking(b,this->TTL);
    doParsimPacking(b,this->protocol);
    doParsimPacking(b,this->checksum);
    doParsimPacking(b,this->srcAddr);
    doParsimPacking(b,this->destAddr);
    doParsimPacking(b,this->options);
}

void IPv4Header::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::EthernetMacHeader::parsimUnpack(b);
    doParsimUnpacking(b,this->version);
    doParsimUnpacking(b,this->IHL);
    doParsimUnpacking(b,this->DSCP);
    doParsimUnpacking(b,this->ipECN);
    doParsimUnpacking(b,this->totalLength);
    doParsimUnpacking(b,this->identification);
    doParsimUnpacking(b,this->ipFlags);
    doParsimUnpacking(b,this->fragmentOffset);
    doParsimUnpacking(b,this->TTL);
    doParsimUnpacking(b,this->protocol);
    doParsimUnpacking(b,this->checksum);
    doParsimUnpacking(b,this->srcAddr);
    doParsimUnpacking(b,this->destAddr);
    doParsimUnpacking(b,this->options);
}

uint8_t IPv4Header::getVersion() const
{
    return this->version;
}

void IPv4Header::setVersion(uint8_t version)
{
    this->version = version;
}

uint8_t IPv4Header::getIHL() const
{
    return this->IHL;
}

void IPv4Header::setIHL(uint8_t IHL)
{
    this->IHL = IHL;
}

uint8_t IPv4Header::getDSCP() const
{
    return this->DSCP;
}

void IPv4Header::setDSCP(uint8_t DSCP)
{
    this->DSCP = DSCP;
}

uint8_t IPv4Header::getIpECN() const
{
    return this->ipECN;
}

void IPv4Header::setIpECN(uint8_t ipECN)
{
    this->ipECN = ipECN;
}

uint16_t IPv4Header::getTotalLength() const
{
    return this->totalLength;
}

void IPv4Header::setTotalLength(uint16_t totalLength)
{
    this->totalLength = totalLength;
}

uint8_t IPv4Header::getIdentification() const
{
    return this->identification;
}

void IPv4Header::setIdentification(uint8_t identification)
{
    this->identification = identification;
}

uint8_t IPv4Header::getIpFlags() const
{
    return this->ipFlags;
}

void IPv4Header::setIpFlags(uint8_t ipFlags)
{
    this->ipFlags = ipFlags;
}

uint8_t IPv4Header::getFragmentOffset() const
{
    return this->fragmentOffset;
}

void IPv4Header::setFragmentOffset(uint8_t fragmentOffset)
{
    this->fragmentOffset = fragmentOffset;
}

uint8_t IPv4Header::getTTL() const
{
    return this->TTL;
}

void IPv4Header::setTTL(uint8_t TTL)
{
    this->TTL = TTL;
}

uint8_t IPv4Header::getProtocol() const
{
    return this->protocol;
}

void IPv4Header::setProtocol(uint8_t protocol)
{
    this->protocol = protocol;
}

uint16_t IPv4Header::getChecksum() const
{
    return this->checksum;
}

void IPv4Header::setChecksum(uint16_t checksum)
{
    this->checksum = checksum;
}

int64_t IPv4Header::getSrcAddr() const
{
    return this->srcAddr;
}

void IPv4Header::setSrcAddr(int64_t srcAddr)
{
    this->srcAddr = srcAddr;
}

int64_t IPv4Header::getDestAddr() const
{
    return this->destAddr;
}

void IPv4Header::setDestAddr(int64_t destAddr)
{
    this->destAddr = destAddr;
}

uint64_t IPv4Header::getOptions() const
{
    return this->options;
}

void IPv4Header::setOptions(uint64_t options)
{
    this->options = options;
}

class IPv4HeaderDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_version,
        FIELD_IHL,
        FIELD_DSCP,
        FIELD_ipECN,
        FIELD_totalLength,
        FIELD_identification,
        FIELD_ipFlags,
        FIELD_fragmentOffset,
        FIELD_TTL,
        FIELD_protocol,
        FIELD_checksum,
        FIELD_srcAddr,
        FIELD_destAddr,
        FIELD_options,
    };
  public:
    IPv4HeaderDescriptor();
    virtual ~IPv4HeaderDescriptor();

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

Register_ClassDescriptor(IPv4HeaderDescriptor)

IPv4HeaderDescriptor::IPv4HeaderDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(IPv4Header)), "EthernetMacHeader")
{
    propertyNames = nullptr;
}

IPv4HeaderDescriptor::~IPv4HeaderDescriptor()
{
    delete[] propertyNames;
}

bool IPv4HeaderDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<IPv4Header *>(obj)!=nullptr;
}

const char **IPv4HeaderDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *IPv4HeaderDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int IPv4HeaderDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 14+base->getFieldCount() : 14;
}

unsigned int IPv4HeaderDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_version
        FD_ISEDITABLE,    // FIELD_IHL
        FD_ISEDITABLE,    // FIELD_DSCP
        FD_ISEDITABLE,    // FIELD_ipECN
        FD_ISEDITABLE,    // FIELD_totalLength
        FD_ISEDITABLE,    // FIELD_identification
        FD_ISEDITABLE,    // FIELD_ipFlags
        FD_ISEDITABLE,    // FIELD_fragmentOffset
        FD_ISEDITABLE,    // FIELD_TTL
        FD_ISEDITABLE,    // FIELD_protocol
        FD_ISEDITABLE,    // FIELD_checksum
        FD_ISEDITABLE,    // FIELD_srcAddr
        FD_ISEDITABLE,    // FIELD_destAddr
        FD_ISEDITABLE,    // FIELD_options
    };
    return (field >= 0 && field < 14) ? fieldTypeFlags[field] : 0;
}

const char *IPv4HeaderDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "version",
        "IHL",
        "DSCP",
        "ipECN",
        "totalLength",
        "identification",
        "ipFlags",
        "fragmentOffset",
        "TTL",
        "protocol",
        "checksum",
        "srcAddr",
        "destAddr",
        "options",
    };
    return (field >= 0 && field < 14) ? fieldNames[field] : nullptr;
}

int IPv4HeaderDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "version") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "IHL") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "DSCP") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "ipECN") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "totalLength") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "identification") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "ipFlags") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "fragmentOffset") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "TTL") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "protocol") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "checksum") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "srcAddr") == 0) return baseIndex + 11;
    if (strcmp(fieldName, "destAddr") == 0) return baseIndex + 12;
    if (strcmp(fieldName, "options") == 0) return baseIndex + 13;
    return base ? base->findField(fieldName) : -1;
}

const char *IPv4HeaderDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint8_t",    // FIELD_version
        "uint8_t",    // FIELD_IHL
        "uint8_t",    // FIELD_DSCP
        "uint8_t",    // FIELD_ipECN
        "uint16_t",    // FIELD_totalLength
        "uint8_t",    // FIELD_identification
        "uint8_t",    // FIELD_ipFlags
        "uint8_t",    // FIELD_fragmentOffset
        "uint8_t",    // FIELD_TTL
        "uint8_t",    // FIELD_protocol
        "uint16_t",    // FIELD_checksum
        "int64_t",    // FIELD_srcAddr
        "int64_t",    // FIELD_destAddr
        "uint64_t",    // FIELD_options
    };
    return (field >= 0 && field < 14) ? fieldTypeStrings[field] : nullptr;
}

const char **IPv4HeaderDescriptor::getFieldPropertyNames(int field) const
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

const char *IPv4HeaderDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int IPv4HeaderDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    IPv4Header *pp = omnetpp::fromAnyPtr<IPv4Header>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void IPv4HeaderDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    IPv4Header *pp = omnetpp::fromAnyPtr<IPv4Header>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'IPv4Header'", field);
    }
}

const char *IPv4HeaderDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    IPv4Header *pp = omnetpp::fromAnyPtr<IPv4Header>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string IPv4HeaderDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    IPv4Header *pp = omnetpp::fromAnyPtr<IPv4Header>(object); (void)pp;
    switch (field) {
        case FIELD_version: return ulong2string(pp->getVersion());
        case FIELD_IHL: return ulong2string(pp->getIHL());
        case FIELD_DSCP: return ulong2string(pp->getDSCP());
        case FIELD_ipECN: return ulong2string(pp->getIpECN());
        case FIELD_totalLength: return ulong2string(pp->getTotalLength());
        case FIELD_identification: return ulong2string(pp->getIdentification());
        case FIELD_ipFlags: return ulong2string(pp->getIpFlags());
        case FIELD_fragmentOffset: return ulong2string(pp->getFragmentOffset());
        case FIELD_TTL: return ulong2string(pp->getTTL());
        case FIELD_protocol: return ulong2string(pp->getProtocol());
        case FIELD_checksum: return ulong2string(pp->getChecksum());
        case FIELD_srcAddr: return int642string(pp->getSrcAddr());
        case FIELD_destAddr: return int642string(pp->getDestAddr());
        case FIELD_options: return uint642string(pp->getOptions());
        default: return "";
    }
}

void IPv4HeaderDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    IPv4Header *pp = omnetpp::fromAnyPtr<IPv4Header>(object); (void)pp;
    switch (field) {
        case FIELD_version: pp->setVersion(string2ulong(value)); break;
        case FIELD_IHL: pp->setIHL(string2ulong(value)); break;
        case FIELD_DSCP: pp->setDSCP(string2ulong(value)); break;
        case FIELD_ipECN: pp->setIpECN(string2ulong(value)); break;
        case FIELD_totalLength: pp->setTotalLength(string2ulong(value)); break;
        case FIELD_identification: pp->setIdentification(string2ulong(value)); break;
        case FIELD_ipFlags: pp->setIpFlags(string2ulong(value)); break;
        case FIELD_fragmentOffset: pp->setFragmentOffset(string2ulong(value)); break;
        case FIELD_TTL: pp->setTTL(string2ulong(value)); break;
        case FIELD_protocol: pp->setProtocol(string2ulong(value)); break;
        case FIELD_checksum: pp->setChecksum(string2ulong(value)); break;
        case FIELD_srcAddr: pp->setSrcAddr(string2int64(value)); break;
        case FIELD_destAddr: pp->setDestAddr(string2int64(value)); break;
        case FIELD_options: pp->setOptions(string2uint64(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'IPv4Header'", field);
    }
}

omnetpp::cValue IPv4HeaderDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    IPv4Header *pp = omnetpp::fromAnyPtr<IPv4Header>(object); (void)pp;
    switch (field) {
        case FIELD_version: return (omnetpp::intval_t)(pp->getVersion());
        case FIELD_IHL: return (omnetpp::intval_t)(pp->getIHL());
        case FIELD_DSCP: return (omnetpp::intval_t)(pp->getDSCP());
        case FIELD_ipECN: return (omnetpp::intval_t)(pp->getIpECN());
        case FIELD_totalLength: return (omnetpp::intval_t)(pp->getTotalLength());
        case FIELD_identification: return (omnetpp::intval_t)(pp->getIdentification());
        case FIELD_ipFlags: return (omnetpp::intval_t)(pp->getIpFlags());
        case FIELD_fragmentOffset: return (omnetpp::intval_t)(pp->getFragmentOffset());
        case FIELD_TTL: return (omnetpp::intval_t)(pp->getTTL());
        case FIELD_protocol: return (omnetpp::intval_t)(pp->getProtocol());
        case FIELD_checksum: return (omnetpp::intval_t)(pp->getChecksum());
        case FIELD_srcAddr: return pp->getSrcAddr();
        case FIELD_destAddr: return pp->getDestAddr();
        case FIELD_options: return (omnetpp::intval_t)(pp->getOptions());
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'IPv4Header' as cValue -- field index out of range?", field);
    }
}

void IPv4HeaderDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    IPv4Header *pp = omnetpp::fromAnyPtr<IPv4Header>(object); (void)pp;
    switch (field) {
        case FIELD_version: pp->setVersion(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_IHL: pp->setIHL(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_DSCP: pp->setDSCP(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_ipECN: pp->setIpECN(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_totalLength: pp->setTotalLength(omnetpp::checked_int_cast<uint16_t>(value.intValue())); break;
        case FIELD_identification: pp->setIdentification(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_ipFlags: pp->setIpFlags(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_fragmentOffset: pp->setFragmentOffset(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_TTL: pp->setTTL(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_protocol: pp->setProtocol(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_checksum: pp->setChecksum(omnetpp::checked_int_cast<uint16_t>(value.intValue())); break;
        case FIELD_srcAddr: pp->setSrcAddr(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_destAddr: pp->setDestAddr(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_options: pp->setOptions(omnetpp::checked_int_cast<uint64_t>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'IPv4Header'", field);
    }
}

const char *IPv4HeaderDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr IPv4HeaderDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    IPv4Header *pp = omnetpp::fromAnyPtr<IPv4Header>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void IPv4HeaderDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    IPv4Header *pp = omnetpp::fromAnyPtr<IPv4Header>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'IPv4Header'", field);
    }
}

Register_Class(IPv6Header)

IPv6Header::IPv6Header(const char *name) : ::EthernetMacHeader(name)
{
    this->setByteLength(14 + 40);

}

IPv6Header::IPv6Header(const IPv6Header& other) : ::EthernetMacHeader(other)
{
    copy(other);
}

IPv6Header::~IPv6Header()
{
}

IPv6Header& IPv6Header::operator=(const IPv6Header& other)
{
    if (this == &other) return *this;
    ::EthernetMacHeader::operator=(other);
    copy(other);
    return *this;
}

void IPv6Header::copy(const IPv6Header& other)
{
    this->version = other.version;
    this->srcAddr = other.srcAddr;
    this->destAddr = other.destAddr;
}

void IPv6Header::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::EthernetMacHeader::parsimPack(b);
    doParsimPacking(b,this->version);
    doParsimPacking(b,this->srcAddr);
    doParsimPacking(b,this->destAddr);
}

void IPv6Header::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::EthernetMacHeader::parsimUnpack(b);
    doParsimUnpacking(b,this->version);
    doParsimUnpacking(b,this->srcAddr);
    doParsimUnpacking(b,this->destAddr);
}

uint8_t IPv6Header::getVersion() const
{
    return this->version;
}

void IPv6Header::setVersion(uint8_t version)
{
    this->version = version;
}

int64_t IPv6Header::getSrcAddr() const
{
    return this->srcAddr;
}

void IPv6Header::setSrcAddr(int64_t srcAddr)
{
    this->srcAddr = srcAddr;
}

int64_t IPv6Header::getDestAddr() const
{
    return this->destAddr;
}

void IPv6Header::setDestAddr(int64_t destAddr)
{
    this->destAddr = destAddr;
}

class IPv6HeaderDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_version,
        FIELD_srcAddr,
        FIELD_destAddr,
    };
  public:
    IPv6HeaderDescriptor();
    virtual ~IPv6HeaderDescriptor();

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

Register_ClassDescriptor(IPv6HeaderDescriptor)

IPv6HeaderDescriptor::IPv6HeaderDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(IPv6Header)), "EthernetMacHeader")
{
    propertyNames = nullptr;
}

IPv6HeaderDescriptor::~IPv6HeaderDescriptor()
{
    delete[] propertyNames;
}

bool IPv6HeaderDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<IPv6Header *>(obj)!=nullptr;
}

const char **IPv6HeaderDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *IPv6HeaderDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int IPv6HeaderDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int IPv6HeaderDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_version
        FD_ISEDITABLE,    // FIELD_srcAddr
        FD_ISEDITABLE,    // FIELD_destAddr
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *IPv6HeaderDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "version",
        "srcAddr",
        "destAddr",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int IPv6HeaderDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "version") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "srcAddr") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "destAddr") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *IPv6HeaderDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint8_t",    // FIELD_version
        "int64_t",    // FIELD_srcAddr
        "int64_t",    // FIELD_destAddr
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **IPv6HeaderDescriptor::getFieldPropertyNames(int field) const
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

const char *IPv6HeaderDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int IPv6HeaderDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    IPv6Header *pp = omnetpp::fromAnyPtr<IPv6Header>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void IPv6HeaderDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    IPv6Header *pp = omnetpp::fromAnyPtr<IPv6Header>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'IPv6Header'", field);
    }
}

const char *IPv6HeaderDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    IPv6Header *pp = omnetpp::fromAnyPtr<IPv6Header>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string IPv6HeaderDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    IPv6Header *pp = omnetpp::fromAnyPtr<IPv6Header>(object); (void)pp;
    switch (field) {
        case FIELD_version: return ulong2string(pp->getVersion());
        case FIELD_srcAddr: return int642string(pp->getSrcAddr());
        case FIELD_destAddr: return int642string(pp->getDestAddr());
        default: return "";
    }
}

void IPv6HeaderDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    IPv6Header *pp = omnetpp::fromAnyPtr<IPv6Header>(object); (void)pp;
    switch (field) {
        case FIELD_version: pp->setVersion(string2ulong(value)); break;
        case FIELD_srcAddr: pp->setSrcAddr(string2int64(value)); break;
        case FIELD_destAddr: pp->setDestAddr(string2int64(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'IPv6Header'", field);
    }
}

omnetpp::cValue IPv6HeaderDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    IPv6Header *pp = omnetpp::fromAnyPtr<IPv6Header>(object); (void)pp;
    switch (field) {
        case FIELD_version: return (omnetpp::intval_t)(pp->getVersion());
        case FIELD_srcAddr: return pp->getSrcAddr();
        case FIELD_destAddr: return pp->getDestAddr();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'IPv6Header' as cValue -- field index out of range?", field);
    }
}

void IPv6HeaderDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    IPv6Header *pp = omnetpp::fromAnyPtr<IPv6Header>(object); (void)pp;
    switch (field) {
        case FIELD_version: pp->setVersion(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_srcAddr: pp->setSrcAddr(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_destAddr: pp->setDestAddr(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'IPv6Header'", field);
    }
}

const char *IPv6HeaderDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr IPv6HeaderDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    IPv6Header *pp = omnetpp::fromAnyPtr<IPv6Header>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void IPv6HeaderDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    IPv6Header *pp = omnetpp::fromAnyPtr<IPv6Header>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'IPv6Header'", field);
    }
}

Register_Class(SegmentRoutingHeader)

SegmentRoutingHeader::SegmentRoutingHeader(const char *name) : ::IPv6Header(name)
{
}

SegmentRoutingHeader::SegmentRoutingHeader(const SegmentRoutingHeader& other) : ::IPv6Header(other)
{
    copy(other);
}

SegmentRoutingHeader::~SegmentRoutingHeader()
{
}

SegmentRoutingHeader& SegmentRoutingHeader::operator=(const SegmentRoutingHeader& other)
{
    if (this == &other) return *this;
    ::IPv6Header::operator=(other);
    copy(other);
    return *this;
}

void SegmentRoutingHeader::copy(const SegmentRoutingHeader& other)
{
    this->hdrLength = other.hdrLength;
    this->routingType = other.routingType;
    this->segmentsLeft = other.segmentsLeft;
    this->lastEntry = other.lastEntry;
    this->srhFlags = other.srhFlags;
    this->srhTag = other.srhTag;
// cplusplus {{
    segments = other.segments;
    funs = other.funs;
   	args = other.args;
// }}
}

void SegmentRoutingHeader::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::IPv6Header::parsimPack(b);
    doParsimPacking(b,this->hdrLength);
    doParsimPacking(b,this->routingType);
    doParsimPacking(b,this->segmentsLeft);
    doParsimPacking(b,this->lastEntry);
    doParsimPacking(b,this->srhFlags);
    doParsimPacking(b,this->srhTag);
    // field segments is abstract or custom -- please do packing in customized class
    // field funs is abstract or custom -- please do packing in customized class
    // field args is abstract or custom -- please do packing in customized class
}

void SegmentRoutingHeader::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::IPv6Header::parsimUnpack(b);
    doParsimUnpacking(b,this->hdrLength);
    doParsimUnpacking(b,this->routingType);
    doParsimUnpacking(b,this->segmentsLeft);
    doParsimUnpacking(b,this->lastEntry);
    doParsimUnpacking(b,this->srhFlags);
    doParsimUnpacking(b,this->srhTag);
    // field segments is abstract or custom -- please do unpacking in customized class
    // field funs is abstract or custom -- please do unpacking in customized class
    // field args is abstract or custom -- please do unpacking in customized class
}

uint8_t SegmentRoutingHeader::getHdrLength() const
{
    return this->hdrLength;
}

void SegmentRoutingHeader::setHdrLength(uint8_t hdrLength)
{
    this->hdrLength = hdrLength;
}

uint8_t SegmentRoutingHeader::getRoutingType() const
{
    return this->routingType;
}

void SegmentRoutingHeader::setRoutingType(uint8_t routingType)
{
    this->routingType = routingType;
}

uint8_t SegmentRoutingHeader::getSegmentsLeft() const
{
    return this->segmentsLeft;
}

void SegmentRoutingHeader::setSegmentsLeft(uint8_t segmentsLeft)
{
    this->segmentsLeft = segmentsLeft;
}

uint8_t SegmentRoutingHeader::getLastEntry() const
{
    return this->lastEntry;
}

void SegmentRoutingHeader::setLastEntry(uint8_t lastEntry)
{
    this->lastEntry = lastEntry;
}

uint8_t SegmentRoutingHeader::getSrhFlags() const
{
    return this->srhFlags;
}

void SegmentRoutingHeader::setSrhFlags(uint8_t srhFlags)
{
    this->srhFlags = srhFlags;
}

uint16_t SegmentRoutingHeader::getSrhTag() const
{
    return this->srhTag;
}

void SegmentRoutingHeader::setSrhTag(uint16_t srhTag)
{
    this->srhTag = srhTag;
}

class SegmentRoutingHeaderDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_hdrLength,
        FIELD_routingType,
        FIELD_segmentsLeft,
        FIELD_lastEntry,
        FIELD_srhFlags,
        FIELD_srhTag,
        FIELD_segments,
        FIELD_funs,
        FIELD_args,
    };
  public:
    SegmentRoutingHeaderDescriptor();
    virtual ~SegmentRoutingHeaderDescriptor();

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

Register_ClassDescriptor(SegmentRoutingHeaderDescriptor)

SegmentRoutingHeaderDescriptor::SegmentRoutingHeaderDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(SegmentRoutingHeader)), "IPv6Header")
{
    propertyNames = nullptr;
}

SegmentRoutingHeaderDescriptor::~SegmentRoutingHeaderDescriptor()
{
    delete[] propertyNames;
}

bool SegmentRoutingHeaderDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<SegmentRoutingHeader *>(obj)!=nullptr;
}

const char **SegmentRoutingHeaderDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *SegmentRoutingHeaderDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int SegmentRoutingHeaderDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 9+base->getFieldCount() : 9;
}

unsigned int SegmentRoutingHeaderDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_hdrLength
        FD_ISEDITABLE,    // FIELD_routingType
        FD_ISEDITABLE,    // FIELD_segmentsLeft
        FD_ISEDITABLE,    // FIELD_lastEntry
        FD_ISEDITABLE,    // FIELD_srhFlags
        FD_ISEDITABLE,    // FIELD_srhTag
        FD_ISARRAY | FD_ISEDITABLE | FD_ISRESIZABLE,    // FIELD_segments
        FD_ISARRAY | FD_ISEDITABLE | FD_ISRESIZABLE,    // FIELD_funs
        FD_ISARRAY | FD_ISEDITABLE | FD_ISRESIZABLE,    // FIELD_args
    };
    return (field >= 0 && field < 9) ? fieldTypeFlags[field] : 0;
}

const char *SegmentRoutingHeaderDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "hdrLength",
        "routingType",
        "segmentsLeft",
        "lastEntry",
        "srhFlags",
        "srhTag",
        "segments",
        "funs",
        "args",
    };
    return (field >= 0 && field < 9) ? fieldNames[field] : nullptr;
}

int SegmentRoutingHeaderDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "hdrLength") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "routingType") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "segmentsLeft") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "lastEntry") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "srhFlags") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "srhTag") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "segments") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "funs") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "args") == 0) return baseIndex + 8;
    return base ? base->findField(fieldName) : -1;
}

const char *SegmentRoutingHeaderDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint8_t",    // FIELD_hdrLength
        "uint8_t",    // FIELD_routingType
        "uint8_t",    // FIELD_segmentsLeft
        "uint8_t",    // FIELD_lastEntry
        "uint8_t",    // FIELD_srhFlags
        "uint16_t",    // FIELD_srhTag
        "int64_t",    // FIELD_segments
        "string",    // FIELD_funs
        "string",    // FIELD_args
    };
    return (field >= 0 && field < 9) ? fieldTypeStrings[field] : nullptr;
}

const char **SegmentRoutingHeaderDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_segments: {
            static const char *names[] = { "custom", "sizeGetter", "sizeSetter",  nullptr };
            return names;
        }
        case FIELD_funs: {
            static const char *names[] = { "custom", "sizeGetter", "sizeSetter",  nullptr };
            return names;
        }
        case FIELD_args: {
            static const char *names[] = { "custom", "sizeGetter", "sizeSetter",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *SegmentRoutingHeaderDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_segments:
            if (!strcmp(propertyName, "custom")) return "";
            if (!strcmp(propertyName, "sizeGetter")) return "";
            if (!strcmp(propertyName, "sizeSetter")) return "";
            return nullptr;
        case FIELD_funs:
            if (!strcmp(propertyName, "custom")) return "";
            if (!strcmp(propertyName, "sizeGetter")) return "";
            if (!strcmp(propertyName, "sizeSetter")) return "";
            return nullptr;
        case FIELD_args:
            if (!strcmp(propertyName, "custom")) return "";
            if (!strcmp(propertyName, "sizeGetter")) return "";
            if (!strcmp(propertyName, "sizeSetter")) return "";
            return nullptr;
        default: return nullptr;
    }
}

int SegmentRoutingHeaderDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    SegmentRoutingHeader *pp = omnetpp::fromAnyPtr<SegmentRoutingHeader>(object); (void)pp;
    switch (field) {
        case FIELD_segments: return pp->getSegmentsArraySize();
        case FIELD_funs: return pp->getFunsArraySize();
        case FIELD_args: return pp->getArgsArraySize();
        default: return 0;
    }
}

void SegmentRoutingHeaderDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    SegmentRoutingHeader *pp = omnetpp::fromAnyPtr<SegmentRoutingHeader>(object); (void)pp;
    switch (field) {
        case FIELD_segments: pp->setSegmentsArraySize(size); break;
        case FIELD_funs: pp->setFunsArraySize(size); break;
        case FIELD_args: pp->setArgsArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'SegmentRoutingHeader'", field);
    }
}

const char *SegmentRoutingHeaderDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    SegmentRoutingHeader *pp = omnetpp::fromAnyPtr<SegmentRoutingHeader>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string SegmentRoutingHeaderDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    SegmentRoutingHeader *pp = omnetpp::fromAnyPtr<SegmentRoutingHeader>(object); (void)pp;
    switch (field) {
        case FIELD_hdrLength: return ulong2string(pp->getHdrLength());
        case FIELD_routingType: return ulong2string(pp->getRoutingType());
        case FIELD_segmentsLeft: return ulong2string(pp->getSegmentsLeft());
        case FIELD_lastEntry: return ulong2string(pp->getLastEntry());
        case FIELD_srhFlags: return ulong2string(pp->getSrhFlags());
        case FIELD_srhTag: return ulong2string(pp->getSrhTag());
        case FIELD_segments: return int642string(pp->getSegments(i));
        case FIELD_funs: return oppstring2string(pp->getFuns(i));
        case FIELD_args: return oppstring2string(pp->getArgs(i));
        default: return "";
    }
}

void SegmentRoutingHeaderDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    SegmentRoutingHeader *pp = omnetpp::fromAnyPtr<SegmentRoutingHeader>(object); (void)pp;
    switch (field) {
        case FIELD_hdrLength: pp->setHdrLength(string2ulong(value)); break;
        case FIELD_routingType: pp->setRoutingType(string2ulong(value)); break;
        case FIELD_segmentsLeft: pp->setSegmentsLeft(string2ulong(value)); break;
        case FIELD_lastEntry: pp->setLastEntry(string2ulong(value)); break;
        case FIELD_srhFlags: pp->setSrhFlags(string2ulong(value)); break;
        case FIELD_srhTag: pp->setSrhTag(string2ulong(value)); break;
        case FIELD_segments: pp->setSegments(i,string2int64(value)); break;
        case FIELD_funs: pp->setFuns(i,(value)); break;
        case FIELD_args: pp->setArgs(i,(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'SegmentRoutingHeader'", field);
    }
}

omnetpp::cValue SegmentRoutingHeaderDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    SegmentRoutingHeader *pp = omnetpp::fromAnyPtr<SegmentRoutingHeader>(object); (void)pp;
    switch (field) {
        case FIELD_hdrLength: return (omnetpp::intval_t)(pp->getHdrLength());
        case FIELD_routingType: return (omnetpp::intval_t)(pp->getRoutingType());
        case FIELD_segmentsLeft: return (omnetpp::intval_t)(pp->getSegmentsLeft());
        case FIELD_lastEntry: return (omnetpp::intval_t)(pp->getLastEntry());
        case FIELD_srhFlags: return (omnetpp::intval_t)(pp->getSrhFlags());
        case FIELD_srhTag: return (omnetpp::intval_t)(pp->getSrhTag());
        case FIELD_segments: return pp->getSegments(i);
        case FIELD_funs: return pp->getFuns(i);
        case FIELD_args: return pp->getArgs(i);
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'SegmentRoutingHeader' as cValue -- field index out of range?", field);
    }
}

void SegmentRoutingHeaderDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    SegmentRoutingHeader *pp = omnetpp::fromAnyPtr<SegmentRoutingHeader>(object); (void)pp;
    switch (field) {
        case FIELD_hdrLength: pp->setHdrLength(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_routingType: pp->setRoutingType(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_segmentsLeft: pp->setSegmentsLeft(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_lastEntry: pp->setLastEntry(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_srhFlags: pp->setSrhFlags(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_srhTag: pp->setSrhTag(omnetpp::checked_int_cast<uint16_t>(value.intValue())); break;
        case FIELD_segments: pp->setSegments(i,omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_funs: pp->setFuns(i,value.stringValue()); break;
        case FIELD_args: pp->setArgs(i,value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'SegmentRoutingHeader'", field);
    }
}

const char *SegmentRoutingHeaderDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr SegmentRoutingHeaderDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    SegmentRoutingHeader *pp = omnetpp::fromAnyPtr<SegmentRoutingHeader>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void SegmentRoutingHeaderDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    SegmentRoutingHeader *pp = omnetpp::fromAnyPtr<SegmentRoutingHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'SegmentRoutingHeader'", field);
    }
}

Register_Class(Packet)

Packet::Packet(const char *name) : ::SegmentRoutingHeader(name)
{
}

Packet::Packet(const Packet& other) : ::SegmentRoutingHeader(other)
{
    copy(other);
}

Packet::~Packet()
{
}

Packet& Packet::operator=(const Packet& other)
{
    if (this == &other) return *this;
    ::SegmentRoutingHeader::operator=(other);
    copy(other);
    return *this;
}

void Packet::copy(const Packet& other)
{
    this->seqNumber = other.seqNumber;
    this->localPort = other.localPort;
    this->destPort = other.destPort;
    this->ECN = other.ECN;
    this->ECE = other.ECE;
    this->packetType = other.packetType;
    this->connectionId = other.connectionId;
    this->receivedBytes = other.receivedBytes;
    this->startTime = other.startTime;
    this->transmitTime = other.transmitTime;
    this->queueTime = other.queueTime;
    this->isFlowFinished_ = other.isFlowFinished_;
}

void Packet::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::SegmentRoutingHeader::parsimPack(b);
    doParsimPacking(b,this->seqNumber);
    doParsimPacking(b,this->localPort);
    doParsimPacking(b,this->destPort);
    doParsimPacking(b,this->ECN);
    doParsimPacking(b,this->ECE);
    doParsimPacking(b,this->packetType);
    doParsimPacking(b,this->connectionId);
    doParsimPacking(b,this->receivedBytes);
    doParsimPacking(b,this->startTime);
    doParsimPacking(b,this->transmitTime);
    doParsimPacking(b,this->queueTime);
    doParsimPacking(b,this->isFlowFinished_);
}

void Packet::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::SegmentRoutingHeader::parsimUnpack(b);
    doParsimUnpacking(b,this->seqNumber);
    doParsimUnpacking(b,this->localPort);
    doParsimUnpacking(b,this->destPort);
    doParsimUnpacking(b,this->ECN);
    doParsimUnpacking(b,this->ECE);
    doParsimUnpacking(b,this->packetType);
    doParsimUnpacking(b,this->connectionId);
    doParsimUnpacking(b,this->receivedBytes);
    doParsimUnpacking(b,this->startTime);
    doParsimUnpacking(b,this->transmitTime);
    doParsimUnpacking(b,this->queueTime);
    doParsimUnpacking(b,this->isFlowFinished_);
}

int64_t Packet::getSeqNumber() const
{
    return this->seqNumber;
}

void Packet::setSeqNumber(int64_t seqNumber)
{
    this->seqNumber = seqNumber;
}

uint16_t Packet::getLocalPort() const
{
    return this->localPort;
}

void Packet::setLocalPort(uint16_t localPort)
{
    this->localPort = localPort;
}

uint16_t Packet::getDestPort() const
{
    return this->destPort;
}

void Packet::setDestPort(uint16_t destPort)
{
    this->destPort = destPort;
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

PacketType Packet::getPacketType() const
{
    return this->packetType;
}

void Packet::setPacketType(PacketType packetType)
{
    this->packetType = packetType;
}

int64_t Packet::getConnectionId() const
{
    return this->connectionId;
}

void Packet::setConnectionId(int64_t connectionId)
{
    this->connectionId = connectionId;
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
        FIELD_seqNumber,
        FIELD_localPort,
        FIELD_destPort,
        FIELD_ECN,
        FIELD_ECE,
        FIELD_packetType,
        FIELD_connectionId,
        FIELD_receivedBytes,
        FIELD_startTime,
        FIELD_transmitTime,
        FIELD_queueTime,
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

PacketDescriptor::PacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(Packet)), "SegmentRoutingHeader")
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
    return base ? 12+base->getFieldCount() : 12;
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
        FD_ISEDITABLE,    // FIELD_seqNumber
        FD_ISEDITABLE,    // FIELD_localPort
        FD_ISEDITABLE,    // FIELD_destPort
        FD_ISEDITABLE,    // FIELD_ECN
        FD_ISEDITABLE,    // FIELD_ECE
        FD_ISEDITABLE,    // FIELD_packetType
        FD_ISEDITABLE,    // FIELD_connectionId
        FD_ISEDITABLE,    // FIELD_receivedBytes
        FD_ISEDITABLE,    // FIELD_startTime
        FD_ISEDITABLE,    // FIELD_transmitTime
        FD_ISEDITABLE,    // FIELD_queueTime
        FD_ISEDITABLE,    // FIELD_isFlowFinished
    };
    return (field >= 0 && field < 12) ? fieldTypeFlags[field] : 0;
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
        "seqNumber",
        "localPort",
        "destPort",
        "ECN",
        "ECE",
        "packetType",
        "connectionId",
        "receivedBytes",
        "startTime",
        "transmitTime",
        "queueTime",
        "isFlowFinished",
    };
    return (field >= 0 && field < 12) ? fieldNames[field] : nullptr;
}

int PacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "seqNumber") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "localPort") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "destPort") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "ECN") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "ECE") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "packetType") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "connectionId") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "receivedBytes") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "startTime") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "transmitTime") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "queueTime") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "isFlowFinished") == 0) return baseIndex + 11;
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
        "int64_t",    // FIELD_seqNumber
        "uint16_t",    // FIELD_localPort
        "uint16_t",    // FIELD_destPort
        "bool",    // FIELD_ECN
        "bool",    // FIELD_ECE
        "PacketType",    // FIELD_packetType
        "int64_t",    // FIELD_connectionId
        "int64_t",    // FIELD_receivedBytes
        "double",    // FIELD_startTime
        "double",    // FIELD_transmitTime
        "double",    // FIELD_queueTime
        "bool",    // FIELD_isFlowFinished
    };
    return (field >= 0 && field < 12) ? fieldTypeStrings[field] : nullptr;
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
        case FIELD_packetType: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
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
        case FIELD_packetType:
            if (!strcmp(propertyName, "enum")) return "PacketType";
            return nullptr;
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
        case FIELD_seqNumber: return int642string(pp->getSeqNumber());
        case FIELD_localPort: return ulong2string(pp->getLocalPort());
        case FIELD_destPort: return ulong2string(pp->getDestPort());
        case FIELD_ECN: return bool2string(pp->getECN());
        case FIELD_ECE: return bool2string(pp->getECE());
        case FIELD_packetType: return enum2string(pp->getPacketType(), "PacketType");
        case FIELD_connectionId: return int642string(pp->getConnectionId());
        case FIELD_receivedBytes: return int642string(pp->getReceivedBytes());
        case FIELD_startTime: return double2string(pp->getStartTime());
        case FIELD_transmitTime: return double2string(pp->getTransmitTime());
        case FIELD_queueTime: return double2string(pp->getQueueTime());
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
        case FIELD_seqNumber: pp->setSeqNumber(string2int64(value)); break;
        case FIELD_localPort: pp->setLocalPort(string2ulong(value)); break;
        case FIELD_destPort: pp->setDestPort(string2ulong(value)); break;
        case FIELD_ECN: pp->setECN(string2bool(value)); break;
        case FIELD_ECE: pp->setECE(string2bool(value)); break;
        case FIELD_packetType: pp->setPacketType((PacketType)string2enum(value, "PacketType")); break;
        case FIELD_connectionId: pp->setConnectionId(string2int64(value)); break;
        case FIELD_receivedBytes: pp->setReceivedBytes(string2int64(value)); break;
        case FIELD_startTime: pp->setStartTime(string2double(value)); break;
        case FIELD_transmitTime: pp->setTransmitTime(string2double(value)); break;
        case FIELD_queueTime: pp->setQueueTime(string2double(value)); break;
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
        case FIELD_seqNumber: return pp->getSeqNumber();
        case FIELD_localPort: return (omnetpp::intval_t)(pp->getLocalPort());
        case FIELD_destPort: return (omnetpp::intval_t)(pp->getDestPort());
        case FIELD_ECN: return pp->getECN();
        case FIELD_ECE: return pp->getECE();
        case FIELD_packetType: return static_cast<int>(pp->getPacketType());
        case FIELD_connectionId: return pp->getConnectionId();
        case FIELD_receivedBytes: return pp->getReceivedBytes();
        case FIELD_startTime: return pp->getStartTime();
        case FIELD_transmitTime: return pp->getTransmitTime();
        case FIELD_queueTime: return pp->getQueueTime();
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
        case FIELD_seqNumber: pp->setSeqNumber(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_localPort: pp->setLocalPort(omnetpp::checked_int_cast<uint16_t>(value.intValue())); break;
        case FIELD_destPort: pp->setDestPort(omnetpp::checked_int_cast<uint16_t>(value.intValue())); break;
        case FIELD_ECN: pp->setECN(value.boolValue()); break;
        case FIELD_ECE: pp->setECE(value.boolValue()); break;
        case FIELD_packetType: pp->setPacketType(static_cast<PacketType>(value.intValue())); break;
        case FIELD_connectionId: pp->setConnectionId(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_receivedBytes: pp->setReceivedBytes(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_startTime: pp->setStartTime(value.doubleValue()); break;
        case FIELD_transmitTime: pp->setTransmitTime(value.doubleValue()); break;
        case FIELD_queueTime: pp->setQueueTime(value.doubleValue()); break;
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

namespace omnetpp {

}  // namespace omnetpp

