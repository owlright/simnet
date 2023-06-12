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

Register_Enum(PacketType, (PacketType::ACK, PacketType::DATA, PacketType::AGG, PacketType::REMIND, PacketType::NOUSE));

Register_Enum(AggPolicy, (AggPolicy::ATP, AggPolicy::MTATP));

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
}

void EthernetMacHeader::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
}

void EthernetMacHeader::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
}

class EthernetMacHeaderDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
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
    return base ? 0+base->getFieldCount() : 0;
}

unsigned int EthernetMacHeaderDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    return 0;
}

const char *EthernetMacHeaderDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    return nullptr;
}

int EthernetMacHeaderDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
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
    return nullptr;
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
    return nullptr;
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

Register_Class(IpHeader)

IpHeader::IpHeader(const char *name) : ::EthernetMacHeader(name)
{
    this->setByteLength(14 + 20);

}

IpHeader::IpHeader(const IpHeader& other) : ::EthernetMacHeader(other)
{
    copy(other);
}

IpHeader::~IpHeader()
{
}

IpHeader& IpHeader::operator=(const IpHeader& other)
{
    if (this == &other) return *this;
    ::EthernetMacHeader::operator=(other);
    copy(other);
    return *this;
}

void IpHeader::copy(const IpHeader& other)
{
    this->srcAddr = other.srcAddr;
    this->destAddr = other.destAddr;
}

void IpHeader::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::EthernetMacHeader::parsimPack(b);
    doParsimPacking(b,this->srcAddr);
    doParsimPacking(b,this->destAddr);
}

void IpHeader::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::EthernetMacHeader::parsimUnpack(b);
    doParsimUnpacking(b,this->srcAddr);
    doParsimUnpacking(b,this->destAddr);
}

int64_t IpHeader::getSrcAddr() const
{
    return this->srcAddr;
}

void IpHeader::setSrcAddr(int64_t srcAddr)
{
    this->srcAddr = srcAddr;
}

int64_t IpHeader::getDestAddr() const
{
    return this->destAddr;
}

void IpHeader::setDestAddr(int64_t destAddr)
{
    this->destAddr = destAddr;
}

class IpHeaderDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_srcAddr,
        FIELD_destAddr,
    };
  public:
    IpHeaderDescriptor();
    virtual ~IpHeaderDescriptor();

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

Register_ClassDescriptor(IpHeaderDescriptor)

IpHeaderDescriptor::IpHeaderDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(IpHeader)), "EthernetMacHeader")
{
    propertyNames = nullptr;
}

IpHeaderDescriptor::~IpHeaderDescriptor()
{
    delete[] propertyNames;
}

bool IpHeaderDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<IpHeader *>(obj)!=nullptr;
}

const char **IpHeaderDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *IpHeaderDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int IpHeaderDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 2+base->getFieldCount() : 2;
}

unsigned int IpHeaderDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_srcAddr
        FD_ISEDITABLE,    // FIELD_destAddr
    };
    return (field >= 0 && field < 2) ? fieldTypeFlags[field] : 0;
}

const char *IpHeaderDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "srcAddr",
        "destAddr",
    };
    return (field >= 0 && field < 2) ? fieldNames[field] : nullptr;
}

int IpHeaderDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "srcAddr") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "destAddr") == 0) return baseIndex + 1;
    return base ? base->findField(fieldName) : -1;
}

const char *IpHeaderDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int64_t",    // FIELD_srcAddr
        "int64_t",    // FIELD_destAddr
    };
    return (field >= 0 && field < 2) ? fieldTypeStrings[field] : nullptr;
}

const char **IpHeaderDescriptor::getFieldPropertyNames(int field) const
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

const char *IpHeaderDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int IpHeaderDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    IpHeader *pp = omnetpp::fromAnyPtr<IpHeader>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void IpHeaderDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    IpHeader *pp = omnetpp::fromAnyPtr<IpHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'IpHeader'", field);
    }
}

const char *IpHeaderDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    IpHeader *pp = omnetpp::fromAnyPtr<IpHeader>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string IpHeaderDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    IpHeader *pp = omnetpp::fromAnyPtr<IpHeader>(object); (void)pp;
    switch (field) {
        case FIELD_srcAddr: return int642string(pp->getSrcAddr());
        case FIELD_destAddr: return int642string(pp->getDestAddr());
        default: return "";
    }
}

void IpHeaderDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    IpHeader *pp = omnetpp::fromAnyPtr<IpHeader>(object); (void)pp;
    switch (field) {
        case FIELD_srcAddr: pp->setSrcAddr(string2int64(value)); break;
        case FIELD_destAddr: pp->setDestAddr(string2int64(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'IpHeader'", field);
    }
}

omnetpp::cValue IpHeaderDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    IpHeader *pp = omnetpp::fromAnyPtr<IpHeader>(object); (void)pp;
    switch (field) {
        case FIELD_srcAddr: return pp->getSrcAddr();
        case FIELD_destAddr: return pp->getDestAddr();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'IpHeader' as cValue -- field index out of range?", field);
    }
}

void IpHeaderDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    IpHeader *pp = omnetpp::fromAnyPtr<IpHeader>(object); (void)pp;
    switch (field) {
        case FIELD_srcAddr: pp->setSrcAddr(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_destAddr: pp->setDestAddr(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'IpHeader'", field);
    }
}

const char *IpHeaderDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr IpHeaderDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    IpHeader *pp = omnetpp::fromAnyPtr<IpHeader>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void IpHeaderDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    IpHeader *pp = omnetpp::fromAnyPtr<IpHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'IpHeader'", field);
    }
}

Register_Class(UdpHeader)

UdpHeader::UdpHeader(const char *name) : ::IpHeader(name)
{
    this->setByteLength(14 + 20 + 8);

}

UdpHeader::UdpHeader(const UdpHeader& other) : ::IpHeader(other)
{
    copy(other);
}

UdpHeader::~UdpHeader()
{
}

UdpHeader& UdpHeader::operator=(const UdpHeader& other)
{
    if (this == &other) return *this;
    ::IpHeader::operator=(other);
    copy(other);
    return *this;
}

void UdpHeader::copy(const UdpHeader& other)
{
    this->localPort = other.localPort;
    this->destPort = other.destPort;
}

void UdpHeader::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::IpHeader::parsimPack(b);
    doParsimPacking(b,this->localPort);
    doParsimPacking(b,this->destPort);
}

void UdpHeader::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::IpHeader::parsimUnpack(b);
    doParsimUnpacking(b,this->localPort);
    doParsimUnpacking(b,this->destPort);
}

uint16_t UdpHeader::getLocalPort() const
{
    return this->localPort;
}

void UdpHeader::setLocalPort(uint16_t localPort)
{
    this->localPort = localPort;
}

uint16_t UdpHeader::getDestPort() const
{
    return this->destPort;
}

void UdpHeader::setDestPort(uint16_t destPort)
{
    this->destPort = destPort;
}

class UdpHeaderDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_localPort,
        FIELD_destPort,
    };
  public:
    UdpHeaderDescriptor();
    virtual ~UdpHeaderDescriptor();

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

Register_ClassDescriptor(UdpHeaderDescriptor)

UdpHeaderDescriptor::UdpHeaderDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(UdpHeader)), "IpHeader")
{
    propertyNames = nullptr;
}

UdpHeaderDescriptor::~UdpHeaderDescriptor()
{
    delete[] propertyNames;
}

bool UdpHeaderDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<UdpHeader *>(obj)!=nullptr;
}

const char **UdpHeaderDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *UdpHeaderDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int UdpHeaderDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 2+base->getFieldCount() : 2;
}

unsigned int UdpHeaderDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_localPort
        FD_ISEDITABLE,    // FIELD_destPort
    };
    return (field >= 0 && field < 2) ? fieldTypeFlags[field] : 0;
}

const char *UdpHeaderDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "localPort",
        "destPort",
    };
    return (field >= 0 && field < 2) ? fieldNames[field] : nullptr;
}

int UdpHeaderDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "localPort") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "destPort") == 0) return baseIndex + 1;
    return base ? base->findField(fieldName) : -1;
}

const char *UdpHeaderDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint16_t",    // FIELD_localPort
        "uint16_t",    // FIELD_destPort
    };
    return (field >= 0 && field < 2) ? fieldTypeStrings[field] : nullptr;
}

const char **UdpHeaderDescriptor::getFieldPropertyNames(int field) const
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

const char *UdpHeaderDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int UdpHeaderDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    UdpHeader *pp = omnetpp::fromAnyPtr<UdpHeader>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void UdpHeaderDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    UdpHeader *pp = omnetpp::fromAnyPtr<UdpHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'UdpHeader'", field);
    }
}

const char *UdpHeaderDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    UdpHeader *pp = omnetpp::fromAnyPtr<UdpHeader>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string UdpHeaderDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    UdpHeader *pp = omnetpp::fromAnyPtr<UdpHeader>(object); (void)pp;
    switch (field) {
        case FIELD_localPort: return ulong2string(pp->getLocalPort());
        case FIELD_destPort: return ulong2string(pp->getDestPort());
        default: return "";
    }
}

void UdpHeaderDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    UdpHeader *pp = omnetpp::fromAnyPtr<UdpHeader>(object); (void)pp;
    switch (field) {
        case FIELD_localPort: pp->setLocalPort(string2ulong(value)); break;
        case FIELD_destPort: pp->setDestPort(string2ulong(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'UdpHeader'", field);
    }
}

omnetpp::cValue UdpHeaderDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    UdpHeader *pp = omnetpp::fromAnyPtr<UdpHeader>(object); (void)pp;
    switch (field) {
        case FIELD_localPort: return (omnetpp::intval_t)(pp->getLocalPort());
        case FIELD_destPort: return (omnetpp::intval_t)(pp->getDestPort());
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'UdpHeader' as cValue -- field index out of range?", field);
    }
}

void UdpHeaderDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    UdpHeader *pp = omnetpp::fromAnyPtr<UdpHeader>(object); (void)pp;
    switch (field) {
        case FIELD_localPort: pp->setLocalPort(omnetpp::checked_int_cast<uint16_t>(value.intValue())); break;
        case FIELD_destPort: pp->setDestPort(omnetpp::checked_int_cast<uint16_t>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'UdpHeader'", field);
    }
}

const char *UdpHeaderDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr UdpHeaderDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    UdpHeader *pp = omnetpp::fromAnyPtr<UdpHeader>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void UdpHeaderDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    UdpHeader *pp = omnetpp::fromAnyPtr<UdpHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'UdpHeader'", field);
    }
}

Register_Class(TcpLikeHeader)

TcpLikeHeader::TcpLikeHeader(const char *name) : ::IpHeader(name)
{
}

TcpLikeHeader::TcpLikeHeader(const TcpLikeHeader& other) : ::IpHeader(other)
{
    copy(other);
}

TcpLikeHeader::~TcpLikeHeader()
{
}

TcpLikeHeader& TcpLikeHeader::operator=(const TcpLikeHeader& other)
{
    if (this == &other) return *this;
    ::IpHeader::operator=(other);
    copy(other);
    return *this;
}

void TcpLikeHeader::copy(const TcpLikeHeader& other)
{
    this->seqNumber = other.seqNumber;
    this->localPort = other.localPort;
    this->destPort = other.destPort;
    this->ECN = other.ECN;
    this->ECE = other.ECE;
}

void TcpLikeHeader::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::IpHeader::parsimPack(b);
    doParsimPacking(b,this->seqNumber);
    doParsimPacking(b,this->localPort);
    doParsimPacking(b,this->destPort);
    doParsimPacking(b,this->ECN);
    doParsimPacking(b,this->ECE);
}

void TcpLikeHeader::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::IpHeader::parsimUnpack(b);
    doParsimUnpacking(b,this->seqNumber);
    doParsimUnpacking(b,this->localPort);
    doParsimUnpacking(b,this->destPort);
    doParsimUnpacking(b,this->ECN);
    doParsimUnpacking(b,this->ECE);
}

int64_t TcpLikeHeader::getSeqNumber() const
{
    return this->seqNumber;
}

void TcpLikeHeader::setSeqNumber(int64_t seqNumber)
{
    this->seqNumber = seqNumber;
}

uint16_t TcpLikeHeader::getLocalPort() const
{
    return this->localPort;
}

void TcpLikeHeader::setLocalPort(uint16_t localPort)
{
    this->localPort = localPort;
}

uint16_t TcpLikeHeader::getDestPort() const
{
    return this->destPort;
}

void TcpLikeHeader::setDestPort(uint16_t destPort)
{
    this->destPort = destPort;
}

bool TcpLikeHeader::getECN() const
{
    return this->ECN;
}

void TcpLikeHeader::setECN(bool ECN)
{
    this->ECN = ECN;
}

bool TcpLikeHeader::getECE() const
{
    return this->ECE;
}

void TcpLikeHeader::setECE(bool ECE)
{
    this->ECE = ECE;
}

class TcpLikeHeaderDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_seqNumber,
        FIELD_localPort,
        FIELD_destPort,
        FIELD_ECN,
        FIELD_ECE,
    };
  public:
    TcpLikeHeaderDescriptor();
    virtual ~TcpLikeHeaderDescriptor();

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

Register_ClassDescriptor(TcpLikeHeaderDescriptor)

TcpLikeHeaderDescriptor::TcpLikeHeaderDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(TcpLikeHeader)), "IpHeader")
{
    propertyNames = nullptr;
}

TcpLikeHeaderDescriptor::~TcpLikeHeaderDescriptor()
{
    delete[] propertyNames;
}

bool TcpLikeHeaderDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TcpLikeHeader *>(obj)!=nullptr;
}

const char **TcpLikeHeaderDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *TcpLikeHeaderDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int TcpLikeHeaderDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 5+base->getFieldCount() : 5;
}

unsigned int TcpLikeHeaderDescriptor::getFieldTypeFlags(int field) const
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
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
}

const char *TcpLikeHeaderDescriptor::getFieldName(int field) const
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
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int TcpLikeHeaderDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "seqNumber") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "localPort") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "destPort") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "ECN") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "ECE") == 0) return baseIndex + 4;
    return base ? base->findField(fieldName) : -1;
}

const char *TcpLikeHeaderDescriptor::getFieldTypeString(int field) const
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
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
}

const char **TcpLikeHeaderDescriptor::getFieldPropertyNames(int field) const
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

const char *TcpLikeHeaderDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int TcpLikeHeaderDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    TcpLikeHeader *pp = omnetpp::fromAnyPtr<TcpLikeHeader>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void TcpLikeHeaderDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    TcpLikeHeader *pp = omnetpp::fromAnyPtr<TcpLikeHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'TcpLikeHeader'", field);
    }
}

const char *TcpLikeHeaderDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    TcpLikeHeader *pp = omnetpp::fromAnyPtr<TcpLikeHeader>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string TcpLikeHeaderDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    TcpLikeHeader *pp = omnetpp::fromAnyPtr<TcpLikeHeader>(object); (void)pp;
    switch (field) {
        case FIELD_seqNumber: return int642string(pp->getSeqNumber());
        case FIELD_localPort: return ulong2string(pp->getLocalPort());
        case FIELD_destPort: return ulong2string(pp->getDestPort());
        case FIELD_ECN: return bool2string(pp->getECN());
        case FIELD_ECE: return bool2string(pp->getECE());
        default: return "";
    }
}

void TcpLikeHeaderDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TcpLikeHeader *pp = omnetpp::fromAnyPtr<TcpLikeHeader>(object); (void)pp;
    switch (field) {
        case FIELD_seqNumber: pp->setSeqNumber(string2int64(value)); break;
        case FIELD_localPort: pp->setLocalPort(string2ulong(value)); break;
        case FIELD_destPort: pp->setDestPort(string2ulong(value)); break;
        case FIELD_ECN: pp->setECN(string2bool(value)); break;
        case FIELD_ECE: pp->setECE(string2bool(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TcpLikeHeader'", field);
    }
}

omnetpp::cValue TcpLikeHeaderDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    TcpLikeHeader *pp = omnetpp::fromAnyPtr<TcpLikeHeader>(object); (void)pp;
    switch (field) {
        case FIELD_seqNumber: return pp->getSeqNumber();
        case FIELD_localPort: return (omnetpp::intval_t)(pp->getLocalPort());
        case FIELD_destPort: return (omnetpp::intval_t)(pp->getDestPort());
        case FIELD_ECN: return pp->getECN();
        case FIELD_ECE: return pp->getECE();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'TcpLikeHeader' as cValue -- field index out of range?", field);
    }
}

void TcpLikeHeaderDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TcpLikeHeader *pp = omnetpp::fromAnyPtr<TcpLikeHeader>(object); (void)pp;
    switch (field) {
        case FIELD_seqNumber: pp->setSeqNumber(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_localPort: pp->setLocalPort(omnetpp::checked_int_cast<uint16_t>(value.intValue())); break;
        case FIELD_destPort: pp->setDestPort(omnetpp::checked_int_cast<uint16_t>(value.intValue())); break;
        case FIELD_ECN: pp->setECN(value.boolValue()); break;
        case FIELD_ECE: pp->setECE(value.boolValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TcpLikeHeader'", field);
    }
}

const char *TcpLikeHeaderDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr TcpLikeHeaderDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    TcpLikeHeader *pp = omnetpp::fromAnyPtr<TcpLikeHeader>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void TcpLikeHeaderDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    TcpLikeHeader *pp = omnetpp::fromAnyPtr<TcpLikeHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TcpLikeHeader'", field);
    }
}

Register_Class(Packet)

Packet::Packet(const char *name) : ::TcpLikeHeader(name)
{
}

Packet::Packet(const Packet& other) : ::TcpLikeHeader(other)
{
    copy(other);
}

Packet::~Packet()
{
}

Packet& Packet::operator=(const Packet& other)
{
    if (this == &other) return *this;
    ::TcpLikeHeader::operator=(other);
    copy(other);
    return *this;
}

void Packet::copy(const Packet& other)
{
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
    ::TcpLikeHeader::parsimPack(b);
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
    ::TcpLikeHeader::parsimUnpack(b);
    doParsimUnpacking(b,this->packetType);
    doParsimUnpacking(b,this->connectionId);
    doParsimUnpacking(b,this->receivedBytes);
    doParsimUnpacking(b,this->startTime);
    doParsimUnpacking(b,this->transmitTime);
    doParsimUnpacking(b,this->queueTime);
    doParsimUnpacking(b,this->isFlowFinished_);
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

PacketDescriptor::PacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(Packet)), "TcpLikeHeader")
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
    return base ? 7+base->getFieldCount() : 7;
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
        FD_ISEDITABLE,    // FIELD_packetType
        FD_ISEDITABLE,    // FIELD_connectionId
        FD_ISEDITABLE,    // FIELD_receivedBytes
        FD_ISEDITABLE,    // FIELD_startTime
        FD_ISEDITABLE,    // FIELD_transmitTime
        FD_ISEDITABLE,    // FIELD_queueTime
        FD_ISEDITABLE,    // FIELD_isFlowFinished
    };
    return (field >= 0 && field < 7) ? fieldTypeFlags[field] : 0;
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
        "packetType",
        "connectionId",
        "receivedBytes",
        "startTime",
        "transmitTime",
        "queueTime",
        "isFlowFinished",
    };
    return (field >= 0 && field < 7) ? fieldNames[field] : nullptr;
}

int PacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "packetType") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "connectionId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "receivedBytes") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "startTime") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "transmitTime") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "queueTime") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "isFlowFinished") == 0) return baseIndex + 6;
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
        "PacketType",    // FIELD_packetType
        "int64_t",    // FIELD_connectionId
        "int64_t",    // FIELD_receivedBytes
        "double",    // FIELD_startTime
        "double",    // FIELD_transmitTime
        "double",    // FIELD_queueTime
        "bool",    // FIELD_isFlowFinished
    };
    return (field >= 0 && field < 7) ? fieldTypeStrings[field] : nullptr;
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

Register_Class(AggPacket)

AggPacket::AggPacket(const char *name) : ::Packet(name)
{
    this->setPacketType(AGG);

}

AggPacket::AggPacket(const AggPacket& other) : ::Packet(other)
{
    copy(other);
}

AggPacket::~AggPacket()
{
}

AggPacket& AggPacket::operator=(const AggPacket& other)
{
    if (this == &other) return *this;
    ::Packet::operator=(other);
    copy(other);
    return *this;
}

void AggPacket::copy(const AggPacket& other)
{
    this->aggPolicy = other.aggPolicy;
    this->aggregatorIndex = other.aggregatorIndex;
    this->jobId = other.jobId;
    this->workerNumber = other.workerNumber;
    this->overflow = other.overflow;
    this->resend = other.resend;
    this->collision = other.collision;
    this->ecn = other.ecn;
    this->isAck_ = other.isAck_;
}

void AggPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::Packet::parsimPack(b);
    doParsimPacking(b,this->aggPolicy);
    doParsimPacking(b,this->aggregatorIndex);
    doParsimPacking(b,this->jobId);
    doParsimPacking(b,this->workerNumber);
    doParsimPacking(b,this->overflow);
    doParsimPacking(b,this->resend);
    doParsimPacking(b,this->collision);
    doParsimPacking(b,this->ecn);
    doParsimPacking(b,this->isAck_);
}

void AggPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::Packet::parsimUnpack(b);
    doParsimUnpacking(b,this->aggPolicy);
    doParsimUnpacking(b,this->aggregatorIndex);
    doParsimUnpacking(b,this->jobId);
    doParsimUnpacking(b,this->workerNumber);
    doParsimUnpacking(b,this->overflow);
    doParsimUnpacking(b,this->resend);
    doParsimUnpacking(b,this->collision);
    doParsimUnpacking(b,this->ecn);
    doParsimUnpacking(b,this->isAck_);
}

AggPolicy AggPacket::getAggPolicy() const
{
    return this->aggPolicy;
}

void AggPacket::setAggPolicy(AggPolicy aggPolicy)
{
    this->aggPolicy = aggPolicy;
}

int AggPacket::getAggregatorIndex() const
{
    return this->aggregatorIndex;
}

void AggPacket::setAggregatorIndex(int aggregatorIndex)
{
    this->aggregatorIndex = aggregatorIndex;
}

int64_t AggPacket::getJobId() const
{
    return this->jobId;
}

void AggPacket::setJobId(int64_t jobId)
{
    this->jobId = jobId;
}

int AggPacket::getWorkerNumber() const
{
    return this->workerNumber;
}

void AggPacket::setWorkerNumber(int workerNumber)
{
    this->workerNumber = workerNumber;
}

bool AggPacket::getOverflow() const
{
    return this->overflow;
}

void AggPacket::setOverflow(bool overflow)
{
    this->overflow = overflow;
}

bool AggPacket::getResend() const
{
    return this->resend;
}

void AggPacket::setResend(bool resend)
{
    this->resend = resend;
}

bool AggPacket::getCollision() const
{
    return this->collision;
}

void AggPacket::setCollision(bool collision)
{
    this->collision = collision;
}

bool AggPacket::getEcn() const
{
    return this->ecn;
}

void AggPacket::setEcn(bool ecn)
{
    this->ecn = ecn;
}

bool AggPacket::isAck() const
{
    return this->isAck_;
}

void AggPacket::setIsAck(bool isAck)
{
    this->isAck_ = isAck;
}

class AggPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_aggPolicy,
        FIELD_aggregatorIndex,
        FIELD_jobId,
        FIELD_workerNumber,
        FIELD_overflow,
        FIELD_resend,
        FIELD_collision,
        FIELD_ecn,
        FIELD_isAck,
    };
  public:
    AggPacketDescriptor();
    virtual ~AggPacketDescriptor();

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

Register_ClassDescriptor(AggPacketDescriptor)

AggPacketDescriptor::AggPacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(AggPacket)), "Packet")
{
    propertyNames = nullptr;
}

AggPacketDescriptor::~AggPacketDescriptor()
{
    delete[] propertyNames;
}

bool AggPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<AggPacket *>(obj)!=nullptr;
}

const char **AggPacketDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *AggPacketDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int AggPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 9+base->getFieldCount() : 9;
}

unsigned int AggPacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_aggPolicy
        FD_ISEDITABLE,    // FIELD_aggregatorIndex
        FD_ISEDITABLE,    // FIELD_jobId
        FD_ISEDITABLE,    // FIELD_workerNumber
        FD_ISEDITABLE,    // FIELD_overflow
        FD_ISEDITABLE,    // FIELD_resend
        FD_ISEDITABLE,    // FIELD_collision
        FD_ISEDITABLE,    // FIELD_ecn
        FD_ISEDITABLE,    // FIELD_isAck
    };
    return (field >= 0 && field < 9) ? fieldTypeFlags[field] : 0;
}

const char *AggPacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "aggPolicy",
        "aggregatorIndex",
        "jobId",
        "workerNumber",
        "overflow",
        "resend",
        "collision",
        "ecn",
        "isAck",
    };
    return (field >= 0 && field < 9) ? fieldNames[field] : nullptr;
}

int AggPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "aggPolicy") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "aggregatorIndex") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "jobId") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "workerNumber") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "overflow") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "resend") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "collision") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "ecn") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "isAck") == 0) return baseIndex + 8;
    return base ? base->findField(fieldName) : -1;
}

const char *AggPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "AggPolicy",    // FIELD_aggPolicy
        "int",    // FIELD_aggregatorIndex
        "int64_t",    // FIELD_jobId
        "int",    // FIELD_workerNumber
        "bool",    // FIELD_overflow
        "bool",    // FIELD_resend
        "bool",    // FIELD_collision
        "bool",    // FIELD_ecn
        "bool",    // FIELD_isAck
    };
    return (field >= 0 && field < 9) ? fieldTypeStrings[field] : nullptr;
}

const char **AggPacketDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_aggPolicy: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *AggPacketDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_aggPolicy:
            if (!strcmp(propertyName, "enum")) return "AggPolicy";
            return nullptr;
        default: return nullptr;
    }
}

int AggPacketDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    AggPacket *pp = omnetpp::fromAnyPtr<AggPacket>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void AggPacketDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    AggPacket *pp = omnetpp::fromAnyPtr<AggPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'AggPacket'", field);
    }
}

const char *AggPacketDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    AggPacket *pp = omnetpp::fromAnyPtr<AggPacket>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string AggPacketDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    AggPacket *pp = omnetpp::fromAnyPtr<AggPacket>(object); (void)pp;
    switch (field) {
        case FIELD_aggPolicy: return enum2string(pp->getAggPolicy(), "AggPolicy");
        case FIELD_aggregatorIndex: return long2string(pp->getAggregatorIndex());
        case FIELD_jobId: return int642string(pp->getJobId());
        case FIELD_workerNumber: return long2string(pp->getWorkerNumber());
        case FIELD_overflow: return bool2string(pp->getOverflow());
        case FIELD_resend: return bool2string(pp->getResend());
        case FIELD_collision: return bool2string(pp->getCollision());
        case FIELD_ecn: return bool2string(pp->getEcn());
        case FIELD_isAck: return bool2string(pp->isAck());
        default: return "";
    }
}

void AggPacketDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    AggPacket *pp = omnetpp::fromAnyPtr<AggPacket>(object); (void)pp;
    switch (field) {
        case FIELD_aggPolicy: pp->setAggPolicy((AggPolicy)string2enum(value, "AggPolicy")); break;
        case FIELD_aggregatorIndex: pp->setAggregatorIndex(string2long(value)); break;
        case FIELD_jobId: pp->setJobId(string2int64(value)); break;
        case FIELD_workerNumber: pp->setWorkerNumber(string2long(value)); break;
        case FIELD_overflow: pp->setOverflow(string2bool(value)); break;
        case FIELD_resend: pp->setResend(string2bool(value)); break;
        case FIELD_collision: pp->setCollision(string2bool(value)); break;
        case FIELD_ecn: pp->setEcn(string2bool(value)); break;
        case FIELD_isAck: pp->setIsAck(string2bool(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'AggPacket'", field);
    }
}

omnetpp::cValue AggPacketDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    AggPacket *pp = omnetpp::fromAnyPtr<AggPacket>(object); (void)pp;
    switch (field) {
        case FIELD_aggPolicy: return static_cast<int>(pp->getAggPolicy());
        case FIELD_aggregatorIndex: return pp->getAggregatorIndex();
        case FIELD_jobId: return pp->getJobId();
        case FIELD_workerNumber: return pp->getWorkerNumber();
        case FIELD_overflow: return pp->getOverflow();
        case FIELD_resend: return pp->getResend();
        case FIELD_collision: return pp->getCollision();
        case FIELD_ecn: return pp->getEcn();
        case FIELD_isAck: return pp->isAck();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'AggPacket' as cValue -- field index out of range?", field);
    }
}

void AggPacketDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    AggPacket *pp = omnetpp::fromAnyPtr<AggPacket>(object); (void)pp;
    switch (field) {
        case FIELD_aggPolicy: pp->setAggPolicy(static_cast<AggPolicy>(value.intValue())); break;
        case FIELD_aggregatorIndex: pp->setAggregatorIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_jobId: pp->setJobId(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_workerNumber: pp->setWorkerNumber(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_overflow: pp->setOverflow(value.boolValue()); break;
        case FIELD_resend: pp->setResend(value.boolValue()); break;
        case FIELD_collision: pp->setCollision(value.boolValue()); break;
        case FIELD_ecn: pp->setEcn(value.boolValue()); break;
        case FIELD_isAck: pp->setIsAck(value.boolValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'AggPacket'", field);
    }
}

const char *AggPacketDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr AggPacketDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    AggPacket *pp = omnetpp::fromAnyPtr<AggPacket>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void AggPacketDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    AggPacket *pp = omnetpp::fromAnyPtr<AggPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'AggPacket'", field);
    }
}

Register_Class(ATPPacket)

ATPPacket::ATPPacket(const char *name) : ::AggPacket(name)
{
    this->setAggPolicy(ATP);
    this->setByteLength(16 + 8);

}

ATPPacket::ATPPacket(const ATPPacket& other) : ::AggPacket(other)
{
    copy(other);
}

ATPPacket::~ATPPacket()
{
}

ATPPacket& ATPPacket::operator=(const ATPPacket& other)
{
    if (this == &other) return *this;
    ::AggPacket::operator=(other);
    copy(other);
    return *this;
}

void ATPPacket::copy(const ATPPacket& other)
{
    this->bitmap0 = other.bitmap0;
    this->bitmap1 = other.bitmap1;
    this->fanIndegree0 = other.fanIndegree0;
    this->fanIndegree1 = other.fanIndegree1;
    this->switchIdentifier = other.switchIdentifier;
}

void ATPPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::AggPacket::parsimPack(b);
    doParsimPacking(b,this->bitmap0);
    doParsimPacking(b,this->bitmap1);
    doParsimPacking(b,this->fanIndegree0);
    doParsimPacking(b,this->fanIndegree1);
    doParsimPacking(b,this->switchIdentifier);
}

void ATPPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::AggPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->bitmap0);
    doParsimUnpacking(b,this->bitmap1);
    doParsimUnpacking(b,this->fanIndegree0);
    doParsimUnpacking(b,this->fanIndegree1);
    doParsimUnpacking(b,this->switchIdentifier);
}

uint32_t ATPPacket::getBitmap0() const
{
    return this->bitmap0;
}

void ATPPacket::setBitmap0(uint32_t bitmap0)
{
    this->bitmap0 = bitmap0;
}

uint32_t ATPPacket::getBitmap1() const
{
    return this->bitmap1;
}

void ATPPacket::setBitmap1(uint32_t bitmap1)
{
    this->bitmap1 = bitmap1;
}

int ATPPacket::getFanIndegree0() const
{
    return this->fanIndegree0;
}

void ATPPacket::setFanIndegree0(int fanIndegree0)
{
    this->fanIndegree0 = fanIndegree0;
}

int ATPPacket::getFanIndegree1() const
{
    return this->fanIndegree1;
}

void ATPPacket::setFanIndegree1(int fanIndegree1)
{
    this->fanIndegree1 = fanIndegree1;
}

int ATPPacket::getSwitchIdentifier() const
{
    return this->switchIdentifier;
}

void ATPPacket::setSwitchIdentifier(int switchIdentifier)
{
    this->switchIdentifier = switchIdentifier;
}

class ATPPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_bitmap0,
        FIELD_bitmap1,
        FIELD_fanIndegree0,
        FIELD_fanIndegree1,
        FIELD_switchIdentifier,
    };
  public:
    ATPPacketDescriptor();
    virtual ~ATPPacketDescriptor();

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

Register_ClassDescriptor(ATPPacketDescriptor)

ATPPacketDescriptor::ATPPacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(ATPPacket)), "AggPacket")
{
    propertyNames = nullptr;
}

ATPPacketDescriptor::~ATPPacketDescriptor()
{
    delete[] propertyNames;
}

bool ATPPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ATPPacket *>(obj)!=nullptr;
}

const char **ATPPacketDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ATPPacketDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ATPPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 5+base->getFieldCount() : 5;
}

unsigned int ATPPacketDescriptor::getFieldTypeFlags(int field) const
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
        FD_ISEDITABLE,    // FIELD_switchIdentifier
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
}

const char *ATPPacketDescriptor::getFieldName(int field) const
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
        "switchIdentifier",
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int ATPPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "bitmap0") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "bitmap1") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "fanIndegree0") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "fanIndegree1") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "switchIdentifier") == 0) return baseIndex + 4;
    return base ? base->findField(fieldName) : -1;
}

const char *ATPPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint32_t",    // FIELD_bitmap0
        "uint32_t",    // FIELD_bitmap1
        "int",    // FIELD_fanIndegree0
        "int",    // FIELD_fanIndegree1
        "int",    // FIELD_switchIdentifier
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
}

const char **ATPPacketDescriptor::getFieldPropertyNames(int field) const
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

const char *ATPPacketDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int ATPPacketDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ATPPacket *pp = omnetpp::fromAnyPtr<ATPPacket>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void ATPPacketDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ATPPacket *pp = omnetpp::fromAnyPtr<ATPPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ATPPacket'", field);
    }
}

const char *ATPPacketDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ATPPacket *pp = omnetpp::fromAnyPtr<ATPPacket>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ATPPacketDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ATPPacket *pp = omnetpp::fromAnyPtr<ATPPacket>(object); (void)pp;
    switch (field) {
        case FIELD_bitmap0: return ulong2string(pp->getBitmap0());
        case FIELD_bitmap1: return ulong2string(pp->getBitmap1());
        case FIELD_fanIndegree0: return long2string(pp->getFanIndegree0());
        case FIELD_fanIndegree1: return long2string(pp->getFanIndegree1());
        case FIELD_switchIdentifier: return long2string(pp->getSwitchIdentifier());
        default: return "";
    }
}

void ATPPacketDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ATPPacket *pp = omnetpp::fromAnyPtr<ATPPacket>(object); (void)pp;
    switch (field) {
        case FIELD_bitmap0: pp->setBitmap0(string2ulong(value)); break;
        case FIELD_bitmap1: pp->setBitmap1(string2ulong(value)); break;
        case FIELD_fanIndegree0: pp->setFanIndegree0(string2long(value)); break;
        case FIELD_fanIndegree1: pp->setFanIndegree1(string2long(value)); break;
        case FIELD_switchIdentifier: pp->setSwitchIdentifier(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ATPPacket'", field);
    }
}

omnetpp::cValue ATPPacketDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ATPPacket *pp = omnetpp::fromAnyPtr<ATPPacket>(object); (void)pp;
    switch (field) {
        case FIELD_bitmap0: return (omnetpp::intval_t)(pp->getBitmap0());
        case FIELD_bitmap1: return (omnetpp::intval_t)(pp->getBitmap1());
        case FIELD_fanIndegree0: return pp->getFanIndegree0();
        case FIELD_fanIndegree1: return pp->getFanIndegree1();
        case FIELD_switchIdentifier: return pp->getSwitchIdentifier();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ATPPacket' as cValue -- field index out of range?", field);
    }
}

void ATPPacketDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ATPPacket *pp = omnetpp::fromAnyPtr<ATPPacket>(object); (void)pp;
    switch (field) {
        case FIELD_bitmap0: pp->setBitmap0(omnetpp::checked_int_cast<uint32_t>(value.intValue())); break;
        case FIELD_bitmap1: pp->setBitmap1(omnetpp::checked_int_cast<uint32_t>(value.intValue())); break;
        case FIELD_fanIndegree0: pp->setFanIndegree0(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_fanIndegree1: pp->setFanIndegree1(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_switchIdentifier: pp->setSwitchIdentifier(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ATPPacket'", field);
    }
}

const char *ATPPacketDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr ATPPacketDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ATPPacket *pp = omnetpp::fromAnyPtr<ATPPacket>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ATPPacketDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ATPPacket *pp = omnetpp::fromAnyPtr<ATPPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ATPPacket'", field);
    }
}

Register_Class(MTATPPacket)

MTATPPacket::MTATPPacket(const char *name) : ::AggPacket(name)
{
    this->setAggPolicy(MTATP);
    this->setByteLength(16 + 4);

    std::fill(this->flags, this->flags + 5, false);
}

MTATPPacket::MTATPPacket(const MTATPPacket& other) : ::AggPacket(other)
{
    copy(other);
}

MTATPPacket::~MTATPPacket()
{
}

MTATPPacket& MTATPPacket::operator=(const MTATPPacket& other)
{
    if (this == &other) return *this;
    ::AggPacket::operator=(other);
    copy(other);
    return *this;
}

void MTATPPacket::copy(const MTATPPacket& other)
{
    this->timer = other.timer;
    this->aggCounter = other.aggCounter;
    for (size_t i = 0; i < 5; i++) {
        this->flags[i] = other.flags[i];
    }
    this->treeCost = other.treeCost;
}

void MTATPPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::AggPacket::parsimPack(b);
    doParsimPacking(b,this->timer);
    doParsimPacking(b,this->aggCounter);
    doParsimArrayPacking(b,this->flags,5);
    doParsimPacking(b,this->treeCost);
}

void MTATPPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::AggPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->timer);
    doParsimUnpacking(b,this->aggCounter);
    doParsimArrayUnpacking(b,this->flags,5);
    doParsimUnpacking(b,this->treeCost);
}

int MTATPPacket::getTimer() const
{
    return this->timer;
}

void MTATPPacket::setTimer(int timer)
{
    this->timer = timer;
}

int MTATPPacket::getAggCounter() const
{
    return this->aggCounter;
}

void MTATPPacket::setAggCounter(int aggCounter)
{
    this->aggCounter = aggCounter;
}

size_t MTATPPacket::getFlagsArraySize() const
{
    return 5;
}

bool MTATPPacket::getFlags(size_t k) const
{
    if (k >= 5) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)5, (unsigned long)k);
    return this->flags[k];
}

void MTATPPacket::setFlags(size_t k, bool flags)
{
    if (k >= 5) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)5, (unsigned long)k);
    this->flags[k] = flags;
}

int MTATPPacket::getTreeCost() const
{
    return this->treeCost;
}

void MTATPPacket::setTreeCost(int treeCost)
{
    this->treeCost = treeCost;
}

class MTATPPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_timer,
        FIELD_aggCounter,
        FIELD_flags,
        FIELD_treeCost,
    };
  public:
    MTATPPacketDescriptor();
    virtual ~MTATPPacketDescriptor();

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

Register_ClassDescriptor(MTATPPacketDescriptor)

MTATPPacketDescriptor::MTATPPacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(MTATPPacket)), "AggPacket")
{
    propertyNames = nullptr;
}

MTATPPacketDescriptor::~MTATPPacketDescriptor()
{
    delete[] propertyNames;
}

bool MTATPPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MTATPPacket *>(obj)!=nullptr;
}

const char **MTATPPacketDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *MTATPPacketDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int MTATPPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 4+base->getFieldCount() : 4;
}

unsigned int MTATPPacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_timer
        FD_ISEDITABLE,    // FIELD_aggCounter
        FD_ISARRAY | FD_ISEDITABLE,    // FIELD_flags
        FD_ISEDITABLE,    // FIELD_treeCost
    };
    return (field >= 0 && field < 4) ? fieldTypeFlags[field] : 0;
}

const char *MTATPPacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "timer",
        "aggCounter",
        "flags",
        "treeCost",
    };
    return (field >= 0 && field < 4) ? fieldNames[field] : nullptr;
}

int MTATPPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "timer") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "aggCounter") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "flags") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "treeCost") == 0) return baseIndex + 3;
    return base ? base->findField(fieldName) : -1;
}

const char *MTATPPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_timer
        "int",    // FIELD_aggCounter
        "bool",    // FIELD_flags
        "int",    // FIELD_treeCost
    };
    return (field >= 0 && field < 4) ? fieldTypeStrings[field] : nullptr;
}

const char **MTATPPacketDescriptor::getFieldPropertyNames(int field) const
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

const char *MTATPPacketDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int MTATPPacketDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    MTATPPacket *pp = omnetpp::fromAnyPtr<MTATPPacket>(object); (void)pp;
    switch (field) {
        case FIELD_flags: return 5;
        default: return 0;
    }
}

void MTATPPacketDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    MTATPPacket *pp = omnetpp::fromAnyPtr<MTATPPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'MTATPPacket'", field);
    }
}

const char *MTATPPacketDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    MTATPPacket *pp = omnetpp::fromAnyPtr<MTATPPacket>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MTATPPacketDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    MTATPPacket *pp = omnetpp::fromAnyPtr<MTATPPacket>(object); (void)pp;
    switch (field) {
        case FIELD_timer: return long2string(pp->getTimer());
        case FIELD_aggCounter: return long2string(pp->getAggCounter());
        case FIELD_flags: return bool2string(pp->getFlags(i));
        case FIELD_treeCost: return long2string(pp->getTreeCost());
        default: return "";
    }
}

void MTATPPacketDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    MTATPPacket *pp = omnetpp::fromAnyPtr<MTATPPacket>(object); (void)pp;
    switch (field) {
        case FIELD_timer: pp->setTimer(string2long(value)); break;
        case FIELD_aggCounter: pp->setAggCounter(string2long(value)); break;
        case FIELD_flags: pp->setFlags(i,string2bool(value)); break;
        case FIELD_treeCost: pp->setTreeCost(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'MTATPPacket'", field);
    }
}

omnetpp::cValue MTATPPacketDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    MTATPPacket *pp = omnetpp::fromAnyPtr<MTATPPacket>(object); (void)pp;
    switch (field) {
        case FIELD_timer: return pp->getTimer();
        case FIELD_aggCounter: return pp->getAggCounter();
        case FIELD_flags: return pp->getFlags(i);
        case FIELD_treeCost: return pp->getTreeCost();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'MTATPPacket' as cValue -- field index out of range?", field);
    }
}

void MTATPPacketDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    MTATPPacket *pp = omnetpp::fromAnyPtr<MTATPPacket>(object); (void)pp;
    switch (field) {
        case FIELD_timer: pp->setTimer(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_aggCounter: pp->setAggCounter(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_flags: pp->setFlags(i,value.boolValue()); break;
        case FIELD_treeCost: pp->setTreeCost(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'MTATPPacket'", field);
    }
}

const char *MTATPPacketDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr MTATPPacketDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    MTATPPacket *pp = omnetpp::fromAnyPtr<MTATPPacket>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void MTATPPacketDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    MTATPPacket *pp = omnetpp::fromAnyPtr<MTATPPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'MTATPPacket'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

