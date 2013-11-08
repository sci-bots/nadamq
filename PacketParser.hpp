#ifndef ___PACKET_PARSER__HPP___
#define ___PACKET_PARSER__HPP___

#include <stdint.h>

#ifndef AVR

/*
 * Assume STL libraries are not available on AVR devices, so don't include
 * methods using them when targeting AVR architectures.
 * */
#include <stdexcept>
#include <string>
#include <iostream>
using namespace std;

#include "crc-16.h"

#else

/* AVR headers define `_crc16_update` function. */
#include <util/crc16.h>

#endif // ifndef AVR


class Packet {
public:
    uint8_t command_;
    uint8_t *payload_buffer_;
    uint16_t payload_length_;
    bool has_crc_;
    uint16_t crc_;

    Packet() : command_(0), payload_length_(0), payload_buffer_(NULL),
               has_crc_(false), crc_(0xFFFF) {}

    void reset(uint8_t *payload_buffer, uint16_t payload_length) {
        this->payload_buffer_ = payload_buffer;
        this->payload_length_ = payload_length;
        this->has_crc_ = false;
        this->crc_ = 0xFFFF;
    }

#ifndef AVR
    /*
     * Assume STL libraries are not available on AVR devices, so don't include
     * methods using them when targeting AVR architectures.
     * */
    string data() const {
        if (this->payload_buffer_ != NULL) {
            return std::string((char *)this->payload_buffer_,
                               this->payload_length_);
        } else {
            throw std::runtime_error("No buffer has been set/allocated.");
        }
    }
#endif // ifndef AVR
};


uint16_t update_crc(uint16_t crc, uint8_t data);


class PacketParser {
public:
    int payload_bytes_received_;
    bool packet_completed_;
    uint16_t crc_;

    PacketParser() : payload_bytes_received_(0), packet_completed_(false) {}
    bool parse(char* buffer, uint16_t buffer_len, Packet *packet);
};

#endif
