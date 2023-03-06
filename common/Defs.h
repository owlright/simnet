#ifndef COMMON_DEFS_H_
#define COMMON_DEFS_H_

typedef enum
{
    INITSTAGE_LOCAL,
    INITSTAGE_REPORT,
    INITSTAGE_CONTROLL,
    NUM_INIT_STAGES
} Stage;
typedef enum
{
    ACK,
    DATA,
    LAST
} PacketType;

#endif /* COMMON_DEFS_H_ */
