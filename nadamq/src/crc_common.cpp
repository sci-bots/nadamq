#include "crc_common.h"

uint16_t update_crc(uint16_t crc, uint8_t data) {
#ifdef AVR
    crc = _crc16_update(crc, data);
#else
    crc = crc_update_byte(crc, data);
#endif
    return crc;
}

uint16_t finalize_crc(uint16_t crc) {
#ifndef AVR
    crc = crc_finalize(crc);
#endif
    return crc;
}
