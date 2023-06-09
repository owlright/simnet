#pragma once
#include <iomanip>
#include <omnetpp.h>
typedef int64_t IntAddress, B, IdNumber, SeqNumber;
typedef uint16_t PortNumber;

#define INVALID_PORT UINT16_MAX
#define INVALID_ADDRESS -1
#define INVALID_ID -1

#define GROUPADDR_START 10000
#define GROUPADDR_END 20000

#define CRCPOLY_LE 0xedb88320
#define MAX_AGTR_COUNT 40000
inline omnetpp::cEnvir *getActiveSimulationOrEnvir() { return omnetpp::cSimulation::getActiveEnvir(); }

enum Stage
{
    INITSTAGE_LOCAL,
    INITSTAGE_COLLECT, // host and switch report to global manager
    INITSTAGE_ASSIGN, // host and switch ask global manager
    INITSTAGE_LAST,
    NUM_INIT_STAGES
};

uint32_t crc32_le(uint32_t crc, unsigned char const* p, size_t len);
uint16_t hashAggrIndex(uint16_t appID, uint16_t index);