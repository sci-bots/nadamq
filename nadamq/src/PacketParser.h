#ifndef ___PACKET_PARSER__HPP___
#define ___PACKET_PARSER__HPP___

#include <stdint.h>
#include <stdlib.h>
#include "crc_common.h"

#ifndef AVR
/* Assume STL libraries are not available on AVR devices, so don't include
 * methods using them when targeting AVR architectures. */
#include <iostream>
using namespace std;
#endif // ifndef AVR

#include "Packet.h"


class Parser {
public:
  bool message_completed_;
  bool parse_error_;

protected:
  int cs;
  uint8_t *p, *pe;
  uint8_t *eof;
  uint8_t *start;

public:
  Parser() : message_completed_(false), parse_error_(false) {}

  int state() const { return cs; }

  void reset();

  void parse_byte(uint8_t *byte);
};


template <typename Packet>
class PacketParser : public Parser {
public:
  typedef Parser base_type;
  typedef Packet packet_type;

  int payload_bytes_expected_;
  int payload_bytes_received_;
  bool message_completed_;
  bool parse_error_;
  uint16_t crc_;
  Packet *packet_;

protected:
  int stack[4];
  int top;
  int length_bytes_received_;

  using base_type::cs;
  using base_type::p;
  using base_type::pe;
  using base_type::eof;
  using base_type::start;

public:
  using base_type::state;

  PacketParser() : base_type(), payload_bytes_expected_(0),
                   payload_bytes_received_(0), crc_(0), packet_(NULL),
                   length_bytes_received_(0) {}

  void reset();
  void reset(Packet *packet) {
    packet_ = packet;
    reset();
  }

  void parse_byte(uint8_t *byte);

  template <typename Stream>
  int8_t parse(Stream &stream, Packet &packet) {
    /* Return codes
     * ============
     *
     *  - `0`: No error, but packet was not completed in this pass.
     *  - `1`: Packet parsing was completed successfully in this pass.
     *  - `-1`: Parse error was encountered in this pass. */
    uint16_t &crc = crc_;

    if (stream.available() > 0) {
      /* Read next available byte from stream. */
      uint8_t byte = stream.read();

      /* Process byte using Ragel parser. */
      parse_byte(&byte);
    }

    if (parse_error_) {
      payload_bytes_expected_ = 0;
      length_bytes_received_ = 0;
      return -1;
    } else if (message_completed_) {
      /* Trigger end-of-file actions. */
      parse_byte(NULL);
      crc_ = crc_finalize(crc_);
      payload_bytes_expected_ = 0;
      length_bytes_received_ = 0;
      return 1;
    } else {
      return 0;
    }
  }
};

#endif
