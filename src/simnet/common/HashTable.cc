#include "Defs.h"
#include <random>

uint32_t crc32_le(uint32_t crc, unsigned char const* p, size_t len)
{
    while (len--) {
        crc ^= *p++;
        for (int i = 0; i < 8; i++)
            crc = (crc >> 1) ^ ((crc & 1) ? CRCPOLY_LE : 0);
    }
    return ~crc;
}

uint16_t hashAggrIndex(uint16_t appID, uint16_t index)
{
  uint8_t crc_input[] = {(uint8_t)(appID & 0xff), (uint8_t)(appID >> 8), (uint8_t)(index & 0xff), (uint8_t)(index >> 8), 0, 0};
  auto new_value = crc32_le(0xffffffff, crc_input, 6);
  return new_value % MAX_AGTR_COUNT;
}
