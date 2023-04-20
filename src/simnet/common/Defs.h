#pragma once
typedef int64_t IntAddress, B, IdNumber, SeqNumber;
typedef uint16_t PortNumber;

// using IntAddress = int64_t;
// using PortNumber = uint16_t;
// using IdNumber = uint64_t;
// using B = int64_t;
#define INVALID_PORT UINT16_MAX
#define INVALID_ADDRESS -1
#define INVALID_ID -1

inline omnetpp::cEnvir *getActiveSimulationOrEnvir() { return omnetpp::cSimulation::getActiveEnvir(); }

enum Stage
{
    INITSTAGE_LOCAL,
    INITSTAGE_REPORT,
    INITSTAGE_CONTROLL,
    NUM_INIT_STAGES
};

// enum PacketType
// {
//     ACK,
//     DATA,
//     LAST
// } ;


