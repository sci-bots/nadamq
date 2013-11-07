#ifndef ___PACKET_PARSER__HPP___
#define ___PACKET_PARSER__HPP___

#include <stdint.h>

class PacketParser {
public:
    int payload_length_;
    int payload_bytes_received_;
    uint8_t command_;
    bool packet_completed_;

    PacketParser() : payload_length_(0), payload_bytes_received_(0),
                     command_(0), packet_completed_(false) {}
    bool parse(char* string, uint8_t &command);
};

#endif
