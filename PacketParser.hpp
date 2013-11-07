#ifndef ___PACKET_PARSER__HPP___
#define ___PACKET_PARSER__HPP___

class PacketParser {
public:
    int payload_length_;
    int payload_bytes_received_;

    PacketParser() : payload_bytes_received_(0) {}
    int parse(char* string);
};

#endif
