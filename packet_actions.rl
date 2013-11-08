%%{

machine packet_grammar;

action startflag_received {
    // We're starting to process a new packet, so reset completed status.
    this->packet_completed_ = false;
}

action command_received {
    packet->command_ = *p;
}

action endflag_received {
    if (packet->payload_length_ == this->payload_bytes_received_) {
        /* We've received an entire packet with the expected payload size, so
         * update the completed status accordingly. */
        this->packet_completed_ = true;
        if ((crc_start != NULL) && (p - crc_start) == 2) {
            /* A CRC was included in the packet. */
            packet->has_crc_ = true;
        }
    }
}

action packet_err {
#ifndef AVR
    /*
     * Assume STL libraries are not available on AVR devices, so don't include
     * methods using them when targeting AVR architectures.
     * */

    cerr << "parse error near byte[" << (p - start) << "]: "
         << std::string((char *)start, (p - start) + 1) << endl;
#endif // ifndef AVR
}

action payloadlength_start {
    // Reset received-bytes counter.
    packet->payload_length_ = 0;
}

action payloadlength_msb {
    /* Received first octet of a two-octet payload-length.  Clear the top bit.
     * Clear the top bit and shift over 8-bits, since this is just the
     * _Most-Significant-Byte (MSB)_.
     *
     * See also: `payloadlength_lsb`.
     */
    packet->payload_length_ = (int)(((*p) & 0x7F)) << 8;
}

action payloadlength_lsb {
    /* Received _Least-Significant-Byte (i.e., LSB)_ of a two-octet
     * payload-length, so add value to previous MSB
     *
     * See also: `payloadlength_msb`.
     */
    packet->payload_length_ += (int)(*p);
}

action payloadlength_single {
    /* Received single-octet payload-length. */
    packet->payload_length_ = (int)(*p);
}

action payload_start {
    // Reset received-bytes counter.
    this->payload_bytes_received_ = 0;
    packet->payload_buffer_ = p;
}

action payload_byte_received {
    /* We received another payload octet, so:
     *
     *   - Update CRC checksum.
     *   - Increment received count.
     *   - Check if we've received all expected octets. */
    crc = update_crc(crc, *p);
    if (++this->payload_bytes_received_ == packet->payload_length_) {
        /* We've received the expected number of payload octets. */
        fret;
    }
}

action crc_start {
    crc_start = p;
    packet->crc_ = (*p) << 8;
}

action crc_received {
    packet->crc_ += *p;
}

include "packet.rl";

}%%

#include <stdio.h>
#include <string.h>
#include "PacketParser.hpp"

%% write data;


bool PacketParser::parse(char* buffer, uint16_t buffer_len, Packet *packet) {
    /*
     * Attempt to parse a packet from a buffer with length `buffer_len`.
     *
     * If successful, return `true` and set:
     *
     *  - `packet.command_`
     *  - `packet.payload_buffer_`
     *  - `packet.payload_length_`
     *
     * __NB__ No data is copied from the input `buffer` to
     * `packet.payload_buffer_`.  Instead, `packet.payload_buffer_` is set to
     * the location in `bufffer` where the payload is found during parsing,
     * while `packet.payload_length_` is set to the length of the
     * payload.  This means that the pointer `packet.payload_buffer_` is only
     * valid as long as `buffer` is valid.
     *
     * If unsuccessful, return `false`.  In the case of an unsuccessful parse
     * attempt, the state of attributes the attributes of `packet` are
     * _undefined_.
     */
    int cs;
    unsigned char *start, *p, *pe, *eof;
    unsigned char *crc_start = NULL;
    int stack[4];
    int top;

    /* Initialize value of CRC checksum, which is updated using the
     * `update_crc` method. */
    //uint16_t crc = 0xFFFF;
    uint16_t &crc = this->crc_;
    crc = crc_init();

    start = (unsigned char *)buffer;
    p = (unsigned char *)start;
    pe = (unsigned char *)(p + buffer_len);
    eof = pe;

    %% write init;
    %% write exec;

    crc = crc_finalize(crc);

    if (packet->has_crc_ && (packet->crc_ != crc)) {
        /* Packet parse was not completed successfully, due to either:
         *
         *   - A parsing error.
         *   - A CRC checksum mismatch.
         */
        this->packet_completed_ = false;
        cerr << "[CRC mismatch] " << endl
             << "  reported: " << packet->crc_ << endl
             << "  computed: " << crc << endl;
    }

    if (!this->packet_completed_) {
        packet->command_ = 0x00;
    }

    return this->packet_completed_;
}

uint16_t update_crc(uint16_t crc, uint8_t data) {
#ifdef AVR
    crc = _crc16_update(crc, data);
#else
    crc = crc_update_byte(crc, data);
#endif
    return crc;
}
