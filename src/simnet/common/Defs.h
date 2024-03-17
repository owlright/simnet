#pragma once
#include <iomanip>
#include <omnetpp.h>
#include "Print.h"
typedef int64_t IntAddress, B, IdNumber, SeqNumber;
typedef uint16_t PortNumber;

#define INVALID_PORT UINT16_MAX
#define INVALID_ADDRESS -1
#define INVALID_ID -1

#define GROUPADDR_START 10000
#define GROUPADDR_END 20000

#define CRCPOLY_LE 0xedb88320
using namespace omnetpp;
template <typename T>
using Dict = std::unordered_map<T, T>;

template <typename T>
using Mat = std::vector<std::vector<T>>;

enum Stage
{
    INITSTAGE_LOCAL, // read params from config.ini
    INITSTAGE_ASSIGN, // manager assign something to host or switch
    INITSTAGE_ASSIGNMAN, // FIXME: traffic manager needs to wait for group manager
    INITSTAGE_ACCEPT, // all parameters must be set by now
    INITSTAGE_REPORT, // host report info to manager
    INITSTAGE_COLLECT, // manager collect info from host
    INITSTAGE_LAST, // maybe delete this module? print info
    NUM_INIT_STAGES
};

[[deprecated]] uint32_t crc32_le(uint32_t crc, unsigned char const* p, size_t len);
[[deprecated]] uint16_t hashAggrIndex(uint16_t appID, uint16_t index);