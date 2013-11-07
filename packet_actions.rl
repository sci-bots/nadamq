%%{

machine packet_grammar;

action startflag_received {
    // We're starting to process a new packet, so reset completed status.
    this->packet_completed_ = false;
}

action command_received {
    this->command_ = *p;
}

action endflag_received {
    if (this->payload_length_ == this->payload_bytes_received_) {
        /* We've received an entire packet with the expected payload size, so
         * update the completed status accordingly. */
        this->packet_completed_ = true;
    }
}

action packet_err {
    cout << "parse error near byte[" << (p - start) << "]: "
         << std::string((char *)start, (p - start) + 1) << endl;
}

action payloadlength_start {
    // Reset received-bytes counter.
    this->payload_length_ = 0;
}

action payloadlength_msb {
    /* Received first octet of a two-octet payload-length.  Clear the top bit.
     * Clear the top bit and shift over 8-bits, since this is just the
     * _Most-Significant-Byte (MSB)_.
     *
     * See also: `payloadlength_lsb`.
     */
    this->payload_length_ = (int)(((*p) & 0x7F)) << 8;
}

action payloadlength_lsb {
    /* Received _Least-Significant-Byte (i.e., LSB)_ of a two-octet
     * payload-length, so add value to previous MSB
     *
     * See also: `payloadlength_msb`.
     */
    this->payload_length_ += (int)(*p);
}

action payloadlength_single {
    /* Received single-octet payload-length. */
    this->payload_length_ = (int)(*p);
}

action payload_start {
    // Reset received-bytes counter.
    this->payload_bytes_received_ = 0;
}

action payload_byte_received {
    /* We received another payload octet, so increment received count and check
     * if we've received all expected octets. */
    if (++this->payload_bytes_received_ == this->payload_length_) {
        /* We've received the expected number of payload octets */
        fret;
    }
}

include "packet.rl";

}%%


#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include "PacketParser.hpp"
using namespace std;

%% write data;

bool PacketParser::parse(char* string, uint8_t &command) {
  int cs;
  unsigned char *start, *p, *pe, *eof;
  int stack[4];
  int top;

  start = (unsigned char *)string;
  p = (unsigned char *)start;
  pe = (unsigned char *)(p + strlen(string));
  eof = pe;

  %% write init;
  %% write exec;

  if (!this->packet_completed_) {
    this->command_ = 0x00;
  }
  command = this->command_;

  return this->packet_completed_;
}
