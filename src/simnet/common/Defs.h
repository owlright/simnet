#pragma once
#include <omnetpp.h>
typedef int64_t IntAddress, B, IdNumber, SeqNumber;
typedef uint16_t PortNumber;

#define INVALID_PORT UINT16_MAX
#define INVALID_ADDRESS -1
#define INVALID_ID -1

inline omnetpp::cEnvir *getActiveSimulationOrEnvir() { return omnetpp::cSimulation::getActiveEnvir(); }

enum Stage
{
    INITSTAGE_LOCAL,
    INITSTAGE_COLLECT,
    INITSTAGE_ASSIGN,
    NUM_INIT_STAGES
};

