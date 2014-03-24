%%{

machine packet_grammar;

action startflag_received {
#ifdef VERBOSE_STATES
  std::cout << "[startflag_received]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  // We're starting to process a new packet, so reset completed status.
  message_completed_ = false;
}

action command_received {
#ifdef VERBOSE_STATES
  std::cout << "[command_received]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  packet_->command(*p);
}

action endflag_received {
#ifdef VERBOSE_STATES
  std::cout << "[endflag_received]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  uint16_t init_crc = crc_init();
  if (crc_ == init_crc || crc_byte_count_ == 2) {
    /* Either packet contains no CRC or we've received two CRC bytes. */
    if (payload_bytes_expected_ == payload_bytes_received_) {
      /* We've received an entire packet with the expected payload size, so
       * update the completed status accordingly. */
      message_completed_ = true;
      if (crc_byte_count_ >= 2) {
        /* A CRC was included in the packet. */
        packet_->has_crc_ = true;
      }
      /* Update payload length, since we successfully parsed the packet. */
      packet_->payload_length_ = payload_bytes_received_;
    } else {
      /* Reset state of packet, since the parsing was not successful. */
      parse_error_ = true;
    }
  }
}

action packet_err {
#ifdef VERBOSE_STATES
  std::cout << "[packet_err]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  parse_error_ = true;
}

action payload_end {
#ifdef VERBOSE_STATES
  std::cout << "[payload_end] received: " << payload_bytes_received_ << "/"
            << payload_bytes_expected_ << std::endl;
#endif  // #ifdef VERBOSE_STATES
}

action payloadlength_start {
#ifdef VERBOSE_STATES
  std::cout << "[payloadlength_start]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  // Reset received-bytes counter.
  payload_bytes_expected_ = 0;
}

action payloadlength_msb {
#ifdef VERBOSE_STATES
  std::cout << "[payloadlength_msb]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  /* Received first octet of a two-octet payload-length.  Clear the top bit.
   * Clear the top bit and shift over 8-bits, since this is just the
   * _Most-Significant-Byte (MSB)_.
   *
   * See also: `payloadlength_lsb`.
   */
  payload_bytes_expected_ = (int)(((*p) & 0x7F)) << 8;
}

action payloadlength_lsb {
#ifdef VERBOSE_STATES
  std::cout << "[payloadlength_lsb]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  /* Received _Least-Significant-Byte (i.e., LSB)_ of a two-octet
   * payload-length, so add value to previous MSB
   *
   * See also: `payloadlength_msb`.
   */
  payload_bytes_expected_ += (int)(*p);
}

action payloadlength_single {
#ifdef VERBOSE_STATES
  std::cout << "[payloadlength_single]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  /* Received single-octet payload-length. */
  payload_bytes_expected_ = (int)(*p);
}

action payload_start {
#ifdef VERBOSE_STATES
  std::cout << "[payload_start] expected size: " << payload_bytes_expected_
            << std::endl;
#endif  // #ifdef VERBOSE_STATES
  /* Resize the payload buffer to fit the expected payload size. */
  packet_->reallocate_buffer(payload_bytes_expected_);
  // Reset received-bytes counter.
  payload_bytes_received_ = 0;
}

action payload_byte_received {
#ifdef VERBOSE_STATES
  std::cout << "[payload_byte_received] byte: " << payload_bytes_received_
            << std::endl;
#endif  // #ifdef VERBOSE_STATES
  /* We received another payload octet, so:
   *
   *   - Update CRC checksum.
   *   - Copy octet _(i.e. byte)_ to payload buffer of packet.
   *   - Increment received count.
   *   - Check if we've received all expected octets. */
  crc_ = update_crc(crc_, *p);
  packet_->payload_buffer_[payload_bytes_received_++] = *p;
  if (payload_bytes_received_ == payload_bytes_expected_) {
    /* We've received the expected number of payload octets. */
    fret;
  }
}

action crc_start {
  crc_ = crc_init();
  crc_byte_count_ = 1;
  packet_->crc_ = (*p) << 8;
}

action crc_received {
  packet_->crc_ += *p;
  crc_byte_count_++;
}

include "packet.rl";

}%%

#include <stdio.h>
#include <string.h>
#include "PacketParser.h"

%% write data;

void PacketParser::reset() {
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
  packet_->reset();
  crc_ = 0;
  crc_byte_count_ = 0;
  message_completed_ = false;
  parse_error_ = false;

  %% write init;
}


void PacketParser::parse_byte(uint8_t *byte) {
  uint8_t dummy_byte;

  if (byte == NULL) {
    /* If no byte is available _(i.e., `NULL` byte pointer was provided)_, set
     * Ragel parser pointers to trigger end-of-file actions. */
    p = &dummy_byte;
    pe = p;
    eof = p;
  } else {
    p = byte;
    pe = p + 1;
  }

  %% write exec;
}


uint16_t update_crc(uint16_t crc, uint8_t data) {
#ifdef AVR
    crc = _crc16_update(crc, data);
#else
    crc = crc_update_byte(crc, data);
#endif
    return crc;
}
