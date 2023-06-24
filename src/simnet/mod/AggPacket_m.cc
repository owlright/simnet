//
// Generated file, do not edit! Created by opp_msgtool 6.0 from simnet/mod/AggPacket.msg.
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
#include "AggPacket_m.h"

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

Register_Enum(AggPolicy, (AggPolicy::ATP, AggPolicy::MTATP, AggPolicy::SRAGG));

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
    this->round = other.round;
    this->aggregatorIndex = other.aggregatorIndex;
    this->jobId = other.jobId;
    this->workerNumber = other.workerNumber;
    this->overflow = other.overflow;
    this->resend = other.resend;
    this->collision = other.collision;
    this->ecn = other.ecn;
    this->isAck_ = other.isAck_;
// cplusplus {{
    this->workerRecord = other.workerRecord;
// }}
}

void AggPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::Packet::parsimPack(b);
    doParsimPacking(b,this->aggPolicy);
    doParsimPacking(b,this->round);
    doParsimPacking(b,this->aggregatorIndex);
    doParsimPacking(b,this->jobId);
    doParsimPacking(b,this->workerNumber);
    doParsimPacking(b,this->overflow);
    doParsimPacking(b,this->resend);
    doParsimPacking(b,this->collision);
    doParsimPacking(b,this->ecn);
    doParsimPacking(b,this->isAck_);
    // field workerRecord is abstract or custom -- please do packing in customized class
}

void AggPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::Packet::parsimUnpack(b);
    doParsimUnpacking(b,this->aggPolicy);
    doParsimUnpacking(b,this->round);
    doParsimUnpacking(b,this->aggregatorIndex);
    doParsimUnpacking(b,this->jobId);
    doParsimUnpacking(b,this->workerNumber);
    doParsimUnpacking(b,this->overflow);
    doParsimUnpacking(b,this->resend);
    doParsimUnpacking(b,this->collision);
    doParsimUnpacking(b,this->ecn);
    doParsimUnpacking(b,this->isAck_);
    // field workerRecord is abstract or custom -- please do unpacking in customized class
}

AggPolicy AggPacket::getAggPolicy() const
{
    return this->aggPolicy;
}

void AggPacket::setAggPolicy(AggPolicy aggPolicy)
{
    this->aggPolicy = aggPolicy;
}

int AggPacket::getRound() const
{
    return this->round;
}

void AggPacket::setRound(int round)
{
    this->round = round;
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
        FIELD_round,
        FIELD_aggregatorIndex,
        FIELD_jobId,
        FIELD_workerNumber,
        FIELD_overflow,
        FIELD_resend,
        FIELD_collision,
        FIELD_ecn,
        FIELD_isAck,
        FIELD_workerRecord,
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
    return base ? 11+base->getFieldCount() : 11;
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
        0,    // FIELD_aggPolicy
        FD_ISEDITABLE,    // FIELD_round
        FD_ISEDITABLE,    // FIELD_aggregatorIndex
        FD_ISEDITABLE,    // FIELD_jobId
        FD_ISEDITABLE,    // FIELD_workerNumber
        FD_ISEDITABLE,    // FIELD_overflow
        FD_ISEDITABLE,    // FIELD_resend
        FD_ISEDITABLE,    // FIELD_collision
        FD_ISEDITABLE,    // FIELD_ecn
        FD_ISEDITABLE,    // FIELD_isAck
        FD_ISARRAY | FD_ISEDITABLE | FD_ISRESIZABLE,    // FIELD_workerRecord
    };
    return (field >= 0 && field < 11) ? fieldTypeFlags[field] : 0;
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
        "round",
        "aggregatorIndex",
        "jobId",
        "workerNumber",
        "overflow",
        "resend",
        "collision",
        "ecn",
        "isAck",
        "workerRecord",
    };
    return (field >= 0 && field < 11) ? fieldNames[field] : nullptr;
}

int AggPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "aggPolicy") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "round") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "aggregatorIndex") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "jobId") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "workerNumber") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "overflow") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "resend") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "collision") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "ecn") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "isAck") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "workerRecord") == 0) return baseIndex + 10;
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
        "int",    // FIELD_round
        "int",    // FIELD_aggregatorIndex
        "int64_t",    // FIELD_jobId
        "int",    // FIELD_workerNumber
        "bool",    // FIELD_overflow
        "bool",    // FIELD_resend
        "bool",    // FIELD_collision
        "bool",    // FIELD_ecn
        "bool",    // FIELD_isAck
        "int64_t",    // FIELD_workerRecord
    };
    return (field >= 0 && field < 11) ? fieldTypeStrings[field] : nullptr;
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
        case FIELD_workerRecord: {
            static const char *names[] = { "custom", "sizeGetter", "sizeSetter", "getter", "setter",  nullptr };
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
        case FIELD_workerRecord:
            if (!strcmp(propertyName, "custom")) return "";
            if (!strcmp(propertyName, "sizeGetter")) return "getRecordLen";
            if (!strcmp(propertyName, "sizeSetter")) return "setRecordLen";
            if (!strcmp(propertyName, "getter")) return "getRecord";
            if (!strcmp(propertyName, "setter")) return "setRecord";
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
        case FIELD_workerRecord: return pp->getRecordLen();
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
        case FIELD_workerRecord: pp->setRecordLen(size); break;
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
        case FIELD_round: return long2string(pp->getRound());
        case FIELD_aggregatorIndex: return long2string(pp->getAggregatorIndex());
        case FIELD_jobId: return int642string(pp->getJobId());
        case FIELD_workerNumber: return long2string(pp->getWorkerNumber());
        case FIELD_overflow: return bool2string(pp->getOverflow());
        case FIELD_resend: return bool2string(pp->getResend());
        case FIELD_collision: return bool2string(pp->getCollision());
        case FIELD_ecn: return bool2string(pp->getEcn());
        case FIELD_isAck: return bool2string(pp->isAck());
        case FIELD_workerRecord: return int642string(pp->getRecord(i));
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
        case FIELD_round: pp->setRound(string2long(value)); break;
        case FIELD_aggregatorIndex: pp->setAggregatorIndex(string2long(value)); break;
        case FIELD_jobId: pp->setJobId(string2int64(value)); break;
        case FIELD_workerNumber: pp->setWorkerNumber(string2long(value)); break;
        case FIELD_overflow: pp->setOverflow(string2bool(value)); break;
        case FIELD_resend: pp->setResend(string2bool(value)); break;
        case FIELD_collision: pp->setCollision(string2bool(value)); break;
        case FIELD_ecn: pp->setEcn(string2bool(value)); break;
        case FIELD_isAck: pp->setIsAck(string2bool(value)); break;
        case FIELD_workerRecord: pp->setRecord(i,string2int64(value)); break;
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
        case FIELD_round: return pp->getRound();
        case FIELD_aggregatorIndex: return pp->getAggregatorIndex();
        case FIELD_jobId: return pp->getJobId();
        case FIELD_workerNumber: return pp->getWorkerNumber();
        case FIELD_overflow: return pp->getOverflow();
        case FIELD_resend: return pp->getResend();
        case FIELD_collision: return pp->getCollision();
        case FIELD_ecn: return pp->getEcn();
        case FIELD_isAck: return pp->isAck();
        case FIELD_workerRecord: return pp->getRecord(i);
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
        case FIELD_round: pp->setRound(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_aggregatorIndex: pp->setAggregatorIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_jobId: pp->setJobId(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_workerNumber: pp->setWorkerNumber(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_overflow: pp->setOverflow(value.boolValue()); break;
        case FIELD_resend: pp->setResend(value.boolValue()); break;
        case FIELD_collision: pp->setCollision(value.boolValue()); break;
        case FIELD_ecn: pp->setEcn(value.boolValue()); break;
        case FIELD_isAck: pp->setIsAck(value.boolValue()); break;
        case FIELD_workerRecord: pp->setRecord(i,omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
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

Register_Class(SRAggPacket)

SRAggPacket::SRAggPacket(const char *name) : ::AggPacket(name)
{
    this->setAggPolicy(SRAGG);

}

SRAggPacket::SRAggPacket(const SRAggPacket& other) : ::AggPacket(other)
{
    copy(other);
}

SRAggPacket::~SRAggPacket()
{
}

SRAggPacket& SRAggPacket::operator=(const SRAggPacket& other)
{
    if (this == &other) return *this;
    ::AggPacket::operator=(other);
    copy(other);
    return *this;
}

void SRAggPacket::copy(const SRAggPacket& other)
{
    this->timer = other.timer;
    this->aggCounter = other.aggCounter;
    this->treeCost = other.treeCost;
}

void SRAggPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::AggPacket::parsimPack(b);
    doParsimPacking(b,this->timer);
    doParsimPacking(b,this->aggCounter);
    doParsimPacking(b,this->treeCost);
}

void SRAggPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::AggPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->timer);
    doParsimUnpacking(b,this->aggCounter);
    doParsimUnpacking(b,this->treeCost);
}

int SRAggPacket::getTimer() const
{
    return this->timer;
}

void SRAggPacket::setTimer(int timer)
{
    this->timer = timer;
}

int SRAggPacket::getAggCounter() const
{
    return this->aggCounter;
}

void SRAggPacket::setAggCounter(int aggCounter)
{
    this->aggCounter = aggCounter;
}

int SRAggPacket::getTreeCost() const
{
    return this->treeCost;
}

void SRAggPacket::setTreeCost(int treeCost)
{
    this->treeCost = treeCost;
}

class SRAggPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_timer,
        FIELD_aggCounter,
        FIELD_treeCost,
    };
  public:
    SRAggPacketDescriptor();
    virtual ~SRAggPacketDescriptor();

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

Register_ClassDescriptor(SRAggPacketDescriptor)

SRAggPacketDescriptor::SRAggPacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(SRAggPacket)), "AggPacket")
{
    propertyNames = nullptr;
}

SRAggPacketDescriptor::~SRAggPacketDescriptor()
{
    delete[] propertyNames;
}

bool SRAggPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<SRAggPacket *>(obj)!=nullptr;
}

const char **SRAggPacketDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *SRAggPacketDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int SRAggPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int SRAggPacketDescriptor::getFieldTypeFlags(int field) const
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
        FD_ISEDITABLE,    // FIELD_treeCost
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *SRAggPacketDescriptor::getFieldName(int field) const
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
        "treeCost",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int SRAggPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "timer") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "aggCounter") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "treeCost") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *SRAggPacketDescriptor::getFieldTypeString(int field) const
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
        "int",    // FIELD_treeCost
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **SRAggPacketDescriptor::getFieldPropertyNames(int field) const
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

const char *SRAggPacketDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int SRAggPacketDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    SRAggPacket *pp = omnetpp::fromAnyPtr<SRAggPacket>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void SRAggPacketDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    SRAggPacket *pp = omnetpp::fromAnyPtr<SRAggPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'SRAggPacket'", field);
    }
}

const char *SRAggPacketDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    SRAggPacket *pp = omnetpp::fromAnyPtr<SRAggPacket>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string SRAggPacketDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    SRAggPacket *pp = omnetpp::fromAnyPtr<SRAggPacket>(object); (void)pp;
    switch (field) {
        case FIELD_timer: return long2string(pp->getTimer());
        case FIELD_aggCounter: return long2string(pp->getAggCounter());
        case FIELD_treeCost: return long2string(pp->getTreeCost());
        default: return "";
    }
}

void SRAggPacketDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    SRAggPacket *pp = omnetpp::fromAnyPtr<SRAggPacket>(object); (void)pp;
    switch (field) {
        case FIELD_timer: pp->setTimer(string2long(value)); break;
        case FIELD_aggCounter: pp->setAggCounter(string2long(value)); break;
        case FIELD_treeCost: pp->setTreeCost(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'SRAggPacket'", field);
    }
}

omnetpp::cValue SRAggPacketDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    SRAggPacket *pp = omnetpp::fromAnyPtr<SRAggPacket>(object); (void)pp;
    switch (field) {
        case FIELD_timer: return pp->getTimer();
        case FIELD_aggCounter: return pp->getAggCounter();
        case FIELD_treeCost: return pp->getTreeCost();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'SRAggPacket' as cValue -- field index out of range?", field);
    }
}

void SRAggPacketDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    SRAggPacket *pp = omnetpp::fromAnyPtr<SRAggPacket>(object); (void)pp;
    switch (field) {
        case FIELD_timer: pp->setTimer(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_aggCounter: pp->setAggCounter(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_treeCost: pp->setTreeCost(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'SRAggPacket'", field);
    }
}

const char *SRAggPacketDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr SRAggPacketDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    SRAggPacket *pp = omnetpp::fromAnyPtr<SRAggPacket>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void SRAggPacketDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    SRAggPacket *pp = omnetpp::fromAnyPtr<SRAggPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'SRAggPacket'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp
