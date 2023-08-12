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

Register_Enum(AggPolicy, (AggPolicy::NOINC, AggPolicy::INC));

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
    this->jobId = other.jobId;
    this->aggSeqNumber = other.aggSeqNumber;
    this->workerNumber = other.workerNumber;
    this->treeCost = other.treeCost;
    this->distance = other.distance;
    this->isAck_ = other.isAck_;
    this->receivedNumber = other.receivedNumber;
    this->PSAddr = other.PSAddr;
// cplusplus {{
    this->workerRecord = other.workerRecord;
// }}
}

void AggPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::Packet::parsimPack(b);
    doParsimPacking(b,this->aggPolicy);
    doParsimPacking(b,this->jobId);
    doParsimPacking(b,this->aggSeqNumber);
    doParsimPacking(b,this->workerNumber);
    doParsimPacking(b,this->treeCost);
    doParsimPacking(b,this->distance);
    doParsimPacking(b,this->isAck_);
    // field workerRecord is abstract or custom -- please do packing in customized class
    doParsimPacking(b,this->receivedNumber);
    doParsimPacking(b,this->PSAddr);
}

void AggPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::Packet::parsimUnpack(b);
    doParsimUnpacking(b,this->aggPolicy);
    doParsimUnpacking(b,this->jobId);
    doParsimUnpacking(b,this->aggSeqNumber);
    doParsimUnpacking(b,this->workerNumber);
    doParsimUnpacking(b,this->treeCost);
    doParsimUnpacking(b,this->distance);
    doParsimUnpacking(b,this->isAck_);
    // field workerRecord is abstract or custom -- please do unpacking in customized class
    doParsimUnpacking(b,this->receivedNumber);
    doParsimUnpacking(b,this->PSAddr);
}

AggPolicy AggPacket::getAggPolicy() const
{
    return this->aggPolicy;
}

void AggPacket::setAggPolicy(AggPolicy aggPolicy)
{
    this->aggPolicy = aggPolicy;
}

int64_t AggPacket::getJobId() const
{
    return this->jobId;
}

void AggPacket::setJobId(int64_t jobId)
{
    this->jobId = jobId;
}

int64_t AggPacket::getAggSeqNumber() const
{
    return this->aggSeqNumber;
}

void AggPacket::setAggSeqNumber(int64_t aggSeqNumber)
{
    this->aggSeqNumber = aggSeqNumber;
}

int AggPacket::getWorkerNumber() const
{
    return this->workerNumber;
}

void AggPacket::setWorkerNumber(int workerNumber)
{
    this->workerNumber = workerNumber;
}

int AggPacket::getTreeCost() const
{
    return this->treeCost;
}

void AggPacket::setTreeCost(int treeCost)
{
    this->treeCost = treeCost;
}

int AggPacket::getDistance() const
{
    return this->distance;
}

void AggPacket::setDistance(int distance)
{
    this->distance = distance;
}

bool AggPacket::isAck() const
{
    return this->isAck_;
}

void AggPacket::setIsAck(bool isAck)
{
    this->isAck_ = isAck;
}

int AggPacket::getReceivedNumber() const
{
    return this->receivedNumber;
}

void AggPacket::setReceivedNumber(int receivedNumber)
{
    this->receivedNumber = receivedNumber;
}

int64_t AggPacket::getPSAddr() const
{
    return this->PSAddr;
}

void AggPacket::setPSAddr(int64_t PSAddr)
{
    this->PSAddr = PSAddr;
}

class AggPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_aggPolicy,
        FIELD_jobId,
        FIELD_aggSeqNumber,
        FIELD_workerNumber,
        FIELD_treeCost,
        FIELD_distance,
        FIELD_isAck,
        FIELD_workerRecord,
        FIELD_receivedNumber,
        FIELD_PSAddr,
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
    return base ? 10+base->getFieldCount() : 10;
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
        FD_ISEDITABLE,    // FIELD_jobId
        FD_ISEDITABLE,    // FIELD_aggSeqNumber
        FD_ISEDITABLE,    // FIELD_workerNumber
        FD_ISEDITABLE,    // FIELD_treeCost
        FD_ISEDITABLE,    // FIELD_distance
        FD_ISEDITABLE,    // FIELD_isAck
        FD_ISARRAY | FD_ISEDITABLE | FD_ISRESIZABLE,    // FIELD_workerRecord
        FD_ISEDITABLE,    // FIELD_receivedNumber
        FD_ISEDITABLE,    // FIELD_PSAddr
    };
    return (field >= 0 && field < 10) ? fieldTypeFlags[field] : 0;
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
        "jobId",
        "aggSeqNumber",
        "workerNumber",
        "treeCost",
        "distance",
        "isAck",
        "workerRecord",
        "receivedNumber",
        "PSAddr",
    };
    return (field >= 0 && field < 10) ? fieldNames[field] : nullptr;
}

int AggPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "aggPolicy") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "jobId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "aggSeqNumber") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "workerNumber") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "treeCost") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "distance") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "isAck") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "workerRecord") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "receivedNumber") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "PSAddr") == 0) return baseIndex + 9;
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
        "int64_t",    // FIELD_jobId
        "int64_t",    // FIELD_aggSeqNumber
        "int",    // FIELD_workerNumber
        "int",    // FIELD_treeCost
        "int",    // FIELD_distance
        "bool",    // FIELD_isAck
        "int64_t",    // FIELD_workerRecord
        "int",    // FIELD_receivedNumber
        "int64_t",    // FIELD_PSAddr
    };
    return (field >= 0 && field < 10) ? fieldTypeStrings[field] : nullptr;
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
        case FIELD_jobId: return int642string(pp->getJobId());
        case FIELD_aggSeqNumber: return int642string(pp->getAggSeqNumber());
        case FIELD_workerNumber: return long2string(pp->getWorkerNumber());
        case FIELD_treeCost: return long2string(pp->getTreeCost());
        case FIELD_distance: return long2string(pp->getDistance());
        case FIELD_isAck: return bool2string(pp->isAck());
        case FIELD_workerRecord: return int642string(pp->getRecord(i));
        case FIELD_receivedNumber: return long2string(pp->getReceivedNumber());
        case FIELD_PSAddr: return int642string(pp->getPSAddr());
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
        case FIELD_jobId: pp->setJobId(string2int64(value)); break;
        case FIELD_aggSeqNumber: pp->setAggSeqNumber(string2int64(value)); break;
        case FIELD_workerNumber: pp->setWorkerNumber(string2long(value)); break;
        case FIELD_treeCost: pp->setTreeCost(string2long(value)); break;
        case FIELD_distance: pp->setDistance(string2long(value)); break;
        case FIELD_isAck: pp->setIsAck(string2bool(value)); break;
        case FIELD_workerRecord: pp->setRecord(i,string2int64(value)); break;
        case FIELD_receivedNumber: pp->setReceivedNumber(string2long(value)); break;
        case FIELD_PSAddr: pp->setPSAddr(string2int64(value)); break;
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
        case FIELD_jobId: return pp->getJobId();
        case FIELD_aggSeqNumber: return pp->getAggSeqNumber();
        case FIELD_workerNumber: return pp->getWorkerNumber();
        case FIELD_treeCost: return pp->getTreeCost();
        case FIELD_distance: return pp->getDistance();
        case FIELD_isAck: return pp->isAck();
        case FIELD_workerRecord: return pp->getRecord(i);
        case FIELD_receivedNumber: return pp->getReceivedNumber();
        case FIELD_PSAddr: return pp->getPSAddr();
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
        case FIELD_jobId: pp->setJobId(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_aggSeqNumber: pp->setAggSeqNumber(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_workerNumber: pp->setWorkerNumber(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_treeCost: pp->setTreeCost(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_distance: pp->setDistance(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_isAck: pp->setIsAck(value.boolValue()); break;
        case FIELD_workerRecord: pp->setRecord(i,omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
        case FIELD_receivedNumber: pp->setReceivedNumber(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_PSAddr: pp->setPSAddr(omnetpp::checked_int_cast<int64_t>(value.intValue())); break;
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

Register_Class(AggUseIncPacket)

AggUseIncPacket::AggUseIncPacket(const char *name) : ::AggPacket(name)
{
    this->setAggPolicy(INC);

}

AggUseIncPacket::AggUseIncPacket(const AggUseIncPacket& other) : ::AggPacket(other)
{
    copy(other);
}

AggUseIncPacket::~AggUseIncPacket()
{
}

AggUseIncPacket& AggUseIncPacket::operator=(const AggUseIncPacket& other)
{
    if (this == &other) return *this;
    ::AggPacket::operator=(other);
    copy(other);
    return *this;
}

void AggUseIncPacket::copy(const AggUseIncPacket& other)
{
    this->aggregatorIndex = other.aggregatorIndex;
    this->collision = other.collision;
    this->ecn = other.ecn;
    this->overflow = other.overflow;
    this->aggCounter = other.aggCounter;
}

void AggUseIncPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::AggPacket::parsimPack(b);
    doParsimPacking(b,this->aggregatorIndex);
    doParsimPacking(b,this->collision);
    doParsimPacking(b,this->ecn);
    doParsimPacking(b,this->overflow);
    doParsimPacking(b,this->aggCounter);
}

void AggUseIncPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::AggPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->aggregatorIndex);
    doParsimUnpacking(b,this->collision);
    doParsimUnpacking(b,this->ecn);
    doParsimUnpacking(b,this->overflow);
    doParsimUnpacking(b,this->aggCounter);
}

unsigned long AggUseIncPacket::getAggregatorIndex() const
{
    return this->aggregatorIndex;
}

void AggUseIncPacket::setAggregatorIndex(unsigned long aggregatorIndex)
{
    this->aggregatorIndex = aggregatorIndex;
}

bool AggUseIncPacket::getCollision() const
{
    return this->collision;
}

void AggUseIncPacket::setCollision(bool collision)
{
    this->collision = collision;
}

bool AggUseIncPacket::getEcn() const
{
    return this->ecn;
}

void AggUseIncPacket::setEcn(bool ecn)
{
    this->ecn = ecn;
}

bool AggUseIncPacket::getOverflow() const
{
    return this->overflow;
}

void AggUseIncPacket::setOverflow(bool overflow)
{
    this->overflow = overflow;
}

int AggUseIncPacket::getAggCounter() const
{
    return this->aggCounter;
}

void AggUseIncPacket::setAggCounter(int aggCounter)
{
    this->aggCounter = aggCounter;
}

class AggUseIncPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_aggregatorIndex,
        FIELD_collision,
        FIELD_ecn,
        FIELD_overflow,
        FIELD_aggCounter,
    };
  public:
    AggUseIncPacketDescriptor();
    virtual ~AggUseIncPacketDescriptor();

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

Register_ClassDescriptor(AggUseIncPacketDescriptor)

AggUseIncPacketDescriptor::AggUseIncPacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(AggUseIncPacket)), "AggPacket")
{
    propertyNames = nullptr;
}

AggUseIncPacketDescriptor::~AggUseIncPacketDescriptor()
{
    delete[] propertyNames;
}

bool AggUseIncPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<AggUseIncPacket *>(obj)!=nullptr;
}

const char **AggUseIncPacketDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *AggUseIncPacketDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int AggUseIncPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 5+base->getFieldCount() : 5;
}

unsigned int AggUseIncPacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_aggregatorIndex
        FD_ISEDITABLE,    // FIELD_collision
        FD_ISEDITABLE,    // FIELD_ecn
        FD_ISEDITABLE,    // FIELD_overflow
        FD_ISEDITABLE,    // FIELD_aggCounter
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
}

const char *AggUseIncPacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "aggregatorIndex",
        "collision",
        "ecn",
        "overflow",
        "aggCounter",
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int AggUseIncPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "aggregatorIndex") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "collision") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "ecn") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "overflow") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "aggCounter") == 0) return baseIndex + 4;
    return base ? base->findField(fieldName) : -1;
}

const char *AggUseIncPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "unsigned long",    // FIELD_aggregatorIndex
        "bool",    // FIELD_collision
        "bool",    // FIELD_ecn
        "bool",    // FIELD_overflow
        "int",    // FIELD_aggCounter
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
}

const char **AggUseIncPacketDescriptor::getFieldPropertyNames(int field) const
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

const char *AggUseIncPacketDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int AggUseIncPacketDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    AggUseIncPacket *pp = omnetpp::fromAnyPtr<AggUseIncPacket>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void AggUseIncPacketDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    AggUseIncPacket *pp = omnetpp::fromAnyPtr<AggUseIncPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'AggUseIncPacket'", field);
    }
}

const char *AggUseIncPacketDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    AggUseIncPacket *pp = omnetpp::fromAnyPtr<AggUseIncPacket>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string AggUseIncPacketDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    AggUseIncPacket *pp = omnetpp::fromAnyPtr<AggUseIncPacket>(object); (void)pp;
    switch (field) {
        case FIELD_aggregatorIndex: return ulong2string(pp->getAggregatorIndex());
        case FIELD_collision: return bool2string(pp->getCollision());
        case FIELD_ecn: return bool2string(pp->getEcn());
        case FIELD_overflow: return bool2string(pp->getOverflow());
        case FIELD_aggCounter: return long2string(pp->getAggCounter());
        default: return "";
    }
}

void AggUseIncPacketDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    AggUseIncPacket *pp = omnetpp::fromAnyPtr<AggUseIncPacket>(object); (void)pp;
    switch (field) {
        case FIELD_aggregatorIndex: pp->setAggregatorIndex(string2ulong(value)); break;
        case FIELD_collision: pp->setCollision(string2bool(value)); break;
        case FIELD_ecn: pp->setEcn(string2bool(value)); break;
        case FIELD_overflow: pp->setOverflow(string2bool(value)); break;
        case FIELD_aggCounter: pp->setAggCounter(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'AggUseIncPacket'", field);
    }
}

omnetpp::cValue AggUseIncPacketDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    AggUseIncPacket *pp = omnetpp::fromAnyPtr<AggUseIncPacket>(object); (void)pp;
    switch (field) {
        case FIELD_aggregatorIndex: return omnetpp::checked_int_cast<omnetpp::intval_t>(pp->getAggregatorIndex());
        case FIELD_collision: return pp->getCollision();
        case FIELD_ecn: return pp->getEcn();
        case FIELD_overflow: return pp->getOverflow();
        case FIELD_aggCounter: return pp->getAggCounter();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'AggUseIncPacket' as cValue -- field index out of range?", field);
    }
}

void AggUseIncPacketDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    AggUseIncPacket *pp = omnetpp::fromAnyPtr<AggUseIncPacket>(object); (void)pp;
    switch (field) {
        case FIELD_aggregatorIndex: pp->setAggregatorIndex(omnetpp::checked_int_cast<unsigned long>(value.intValue())); break;
        case FIELD_collision: pp->setCollision(value.boolValue()); break;
        case FIELD_ecn: pp->setEcn(value.boolValue()); break;
        case FIELD_overflow: pp->setOverflow(value.boolValue()); break;
        case FIELD_aggCounter: pp->setAggCounter(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'AggUseIncPacket'", field);
    }
}

const char *AggUseIncPacketDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr AggUseIncPacketDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    AggUseIncPacket *pp = omnetpp::fromAnyPtr<AggUseIncPacket>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void AggUseIncPacketDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    AggUseIncPacket *pp = omnetpp::fromAnyPtr<AggUseIncPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'AggUseIncPacket'", field);
    }
}

Register_Class(AggNoIncPacket)

AggNoIncPacket::AggNoIncPacket(const char *name) : ::AggPacket(name)
{
    this->setAggPolicy(NOINC);
}

AggNoIncPacket::AggNoIncPacket(const AggNoIncPacket& other) : ::AggPacket(other)
{
    copy(other);
}

AggNoIncPacket::~AggNoIncPacket()
{
}

AggNoIncPacket& AggNoIncPacket::operator=(const AggNoIncPacket& other)
{
    if (this == &other) return *this;
    ::AggPacket::operator=(other);
    copy(other);
    return *this;
}

void AggNoIncPacket::copy(const AggNoIncPacket& other)
{
}

void AggNoIncPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::AggPacket::parsimPack(b);
}

void AggNoIncPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::AggPacket::parsimUnpack(b);
}

class AggNoIncPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
    };
  public:
    AggNoIncPacketDescriptor();
    virtual ~AggNoIncPacketDescriptor();

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

Register_ClassDescriptor(AggNoIncPacketDescriptor)

AggNoIncPacketDescriptor::AggNoIncPacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(AggNoIncPacket)), "AggPacket")
{
    propertyNames = nullptr;
}

AggNoIncPacketDescriptor::~AggNoIncPacketDescriptor()
{
    delete[] propertyNames;
}

bool AggNoIncPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<AggNoIncPacket *>(obj)!=nullptr;
}

const char **AggNoIncPacketDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *AggNoIncPacketDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int AggNoIncPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 0+base->getFieldCount() : 0;
}

unsigned int AggNoIncPacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    return 0;
}

const char *AggNoIncPacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    return nullptr;
}

int AggNoIncPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->findField(fieldName) : -1;
}

const char *AggNoIncPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    return nullptr;
}

const char **AggNoIncPacketDescriptor::getFieldPropertyNames(int field) const
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

const char *AggNoIncPacketDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int AggNoIncPacketDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    AggNoIncPacket *pp = omnetpp::fromAnyPtr<AggNoIncPacket>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void AggNoIncPacketDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    AggNoIncPacket *pp = omnetpp::fromAnyPtr<AggNoIncPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'AggNoIncPacket'", field);
    }
}

const char *AggNoIncPacketDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    AggNoIncPacket *pp = omnetpp::fromAnyPtr<AggNoIncPacket>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string AggNoIncPacketDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    AggNoIncPacket *pp = omnetpp::fromAnyPtr<AggNoIncPacket>(object); (void)pp;
    switch (field) {
        default: return "";
    }
}

void AggNoIncPacketDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    AggNoIncPacket *pp = omnetpp::fromAnyPtr<AggNoIncPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'AggNoIncPacket'", field);
    }
}

omnetpp::cValue AggNoIncPacketDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    AggNoIncPacket *pp = omnetpp::fromAnyPtr<AggNoIncPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'AggNoIncPacket' as cValue -- field index out of range?", field);
    }
}

void AggNoIncPacketDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    AggNoIncPacket *pp = omnetpp::fromAnyPtr<AggNoIncPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'AggNoIncPacket'", field);
    }
}

const char *AggNoIncPacketDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    return nullptr;
}

omnetpp::any_ptr AggNoIncPacketDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    AggNoIncPacket *pp = omnetpp::fromAnyPtr<AggNoIncPacket>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void AggNoIncPacketDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    AggNoIncPacket *pp = omnetpp::fromAnyPtr<AggNoIncPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'AggNoIncPacket'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

