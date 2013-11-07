%%{

machine packet_grammar;

action notify_startflag { cout << "got startflag: " << *(p - 1) << endl; }
action notify_command { cout << "got command: " << *(p - 1) << endl; }
action notify_crc { cout << "got crc: " << *(p - 1) << endl; }
action notify_endflag { cout << "got endflag: " << *(p - 1) << endl; }
action packet_err {
    cout << "parse error at byte[" << (p - start) << "]: "
         << (unsigned char)(*(p - 1)) << endl;
}

action notify_payloadlength { cout << "got payloadlength: " << *(p - 1) << endl; }


action payloadlength_start {
    cout << "start payloadlength" << endl;
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

action payloadlength_received {
    cout << "payloadlength: " << this->payload_length_ << endl;
}

action payload_start {
    cout << "start payload" << endl;
    // Reset received-bytes counter.
    this->payload_bytes_received_ = 0;
}

action payload_byte_received {
    cout << "received payload byte #" << ++(this->payload_bytes_received_)
         << endl;
}

action notify_payload { cout << "got payload: " << *(p - 1) << endl; }

include "packet.rl";

}%%


#include <iostream>
#include <stdio.h>
#include <string.h>
#include "PacketParser.hpp"
using namespace std;

%% write data;

int PacketParser::parse(char* string)
{
  int cs;
  int res=0;
  unsigned char *start, *p, *pe, *eof;

  start = (unsigned char *)string;
  p = (unsigned char *)start;
  pe = (unsigned char *)(p + strlen(string));
  eof = pe;

  %% write init;
  %% write exec;

  return res;
}
