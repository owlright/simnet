#pragma once
typedef int64_t IntAddress, B, IdNumber;
typedef uint16_t PortNumber;

// using IntAddress = int64_t;
// using PortNumber = uint16_t;
// using IdNumber = uint64_t;
// using B = int64_t;
#define MAX_PORT_NUMBER UINT16_MAX
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


