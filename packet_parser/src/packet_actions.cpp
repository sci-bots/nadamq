
#line 1 "packet_actions.rl"

#line 142 "packet_actions.rl"


#include <stdio.h>
#include <string.h>
#include "PacketParser.hpp"


#line 13 "packet_actions.cpp"
static const char _packet_grammar_eof_actions[] = {
	0, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 0, 0, 11, 0
};

static const int packet_grammar_start = 1;
static const int packet_grammar_first_final = 12;
static const int packet_grammar_error = 0;

static const int packet_grammar_en_process_payload = 14;
static const int packet_grammar_en_main = 1;


#line 149 "packet_actions.rl"

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

  
#line 57 "packet_actions.cpp"
	{
	cs = packet_grammar_start;
	top = 0;
	}

#line 178 "packet_actions.rl"
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

  
#line 82 "packet_actions.cpp"
	{
	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	switch ( cs ) {
case 1:
	if ( (*p) == 126u )
		goto tr1;
	goto tr0;
case 0:
	goto _out;
case 2:
	if ( (*p) == 126u )
		goto tr2;
	goto tr0;
case 3:
	if ( (*p) == 115u )
		goto tr3;
	goto tr0;
case 4:
	if ( (*p) == 126u )
		goto tr4;
	goto tr0;
case 5:
	if ( (*p) == 126u )
		goto tr5;
	goto tr0;
case 6:
	goto tr6;
case 7:
	if ( 128u <= (*p) )
		goto tr8;
	goto tr7;
case 8:
	if ( (*p) == 33u )
		goto tr10;
	goto tr9;
case 9:
	goto tr11;
case 10:
	if ( (*p) == 33u )
		goto tr12;
	goto tr0;
case 12:
	goto tr0;
case 13:
	goto tr11;
case 11:
	goto tr13;
case 14:
	goto tr14;
case 15:
	goto tr15;
	}

	tr0: cs = 0; goto f0;
	tr1: cs = 2; goto _again;
	tr2: cs = 3; goto _again;
	tr3: cs = 4; goto _again;
	tr4: cs = 5; goto _again;
	tr5: cs = 6; goto f1;
	tr6: cs = 7; goto f2;
	tr7: cs = 8; goto f3;
	tr13: cs = 8; goto f9;
	tr9: cs = 9; goto f5;
	tr11: cs = 10; goto f7;
	tr8: cs = 11; goto f4;
	tr12: cs = 12; goto f8;
	tr10: cs = 13; goto f6;
	tr14: cs = 15; goto f11;
	tr15: cs = 15; goto f12;

f1:
#line 5 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[startflag_received]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  // We're starting to process a new packet, so reset completed status.
  message_completed_ = false;
}
	goto _again;
f2:
#line 13 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[command_received]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  packet_->command(*p);
}
	goto _again;
f8:
#line 20 "packet_actions.rl"
	{
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
	goto _again;
f0:
#line 44 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[packet_err]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  parse_error_ = true;
}
	goto _again;
f12:
#line 110 "packet_actions.rl"
	{
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
    {cs = stack[--top];goto _again;}
  }
}
	goto _again;
f7:
#line 135 "packet_actions.rl"
	{
  packet_->crc_ += *p;
  crc_byte_count_++;
}
	goto _again;
f5:
#line 51 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[payload_end] received: " << payload_bytes_received_ << "/"
            << payload_bytes_expected_ << std::endl;
#endif  // #ifdef VERBOSE_STATES
}
#line 129 "packet_actions.rl"
	{
  crc_ = crc_init();
  crc_byte_count_ = 1;
  packet_->crc_ = (*p) << 8;
}
	goto _again;
f4:
#line 58 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[payloadlength_start]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  // Reset received-bytes counter.
  payload_bytes_expected_ = 0;
}
#line 66 "packet_actions.rl"
	{
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
	goto _again;
f9:
#line 79 "packet_actions.rl"
	{
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
#line 47 "packet.rl"
	{ {stack[top++] = cs; cs = 14; goto _again;} }
	goto _again;
f11:
#line 99 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[payload_start] expected size: " << payload_bytes_expected_
            << std::endl;
#endif  // #ifdef VERBOSE_STATES
  /* Resize the payload buffer to fit the expected payload size. */
  packet_->reallocate_buffer(payload_bytes_expected_);
  // Reset received-bytes counter.
  payload_bytes_received_ = 0;
}
#line 110 "packet_actions.rl"
	{
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
    {cs = stack[--top];goto _again;}
  }
}
	goto _again;
f6:
#line 51 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[payload_end] received: " << payload_bytes_received_ << "/"
            << payload_bytes_expected_ << std::endl;
#endif  // #ifdef VERBOSE_STATES
}
#line 129 "packet_actions.rl"
	{
  crc_ = crc_init();
  crc_byte_count_ = 1;
  packet_->crc_ = (*p) << 8;
}
#line 20 "packet_actions.rl"
	{
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
	goto _again;
f3:
#line 58 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[payloadlength_start]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  // Reset received-bytes counter.
  payload_bytes_expected_ = 0;
}
#line 91 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[payloadlength_single]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  /* Received single-octet payload-length. */
  payload_bytes_expected_ = (int)(*p);
}
#line 47 "packet.rl"
	{ {stack[top++] = cs; cs = 14; goto _again;} }
	goto _again;

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	switch ( _packet_grammar_eof_actions[cs] ) {
	case 1:
#line 44 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[packet_err]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  parse_error_ = true;
}
	break;
	case 11:
#line 99 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[payload_start] expected size: " << payload_bytes_expected_
            << std::endl;
#endif  // #ifdef VERBOSE_STATES
  /* Resize the payload buffer to fit the expected payload size. */
  packet_->reallocate_buffer(payload_bytes_expected_);
  // Reset received-bytes counter.
  payload_bytes_received_ = 0;
}
	break;
#line 416 "packet_actions.cpp"
	}
	}

	_out: {}
	}

#line 196 "packet_actions.rl"
}


uint16_t update_crc(uint16_t crc, uint8_t data) {
#ifdef AVR
    crc = _crc16_update(crc, data);
#else
    crc = crc_update_byte(crc, data);
#endif
    return crc;
}
