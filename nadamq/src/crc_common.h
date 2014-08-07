#ifndef ___CRC_COMMON__H___
#define ___CRC_COMMON__H___

#include <stdint.h>

#ifndef AVR
#include "crc-16.h"
using namespace std;
#else

/* AVR headers define `_crc16_update` function. */
#include <util/crc16.h>

inline uint16_t crc_init() { return 0; }
inline void crc_finalize(...) {}
#endif // ifndef AVR


uint16_t update_crc(uint16_t crc, uint8_t data);
uint16_t finalize_crc(uint16_t crc);

#endif  // #ifndef ___CRC_COMMON__H___
