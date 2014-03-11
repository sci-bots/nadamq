
#line 1 "packet_actions.rl"

#line 142 "packet_actions.rl"


#include <stdio.h>
#include <string.h>
#include "PacketParser.hpp"


#line 13 "packet_actions.cpp"
static const int packet_grammar_start = 1;
static const int packet_grammar_first_final = 8;
static const int packet_grammar_error = 0;

static const int packet_grammar_en_process_payload = 10;
static const int packet_grammar_en_main = 1;


#line 149 "packet_actions.rl"

void PacketParser::reset(Packet *packet) {
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
  packet_ = packet;
  packet_->reset();
  crc_ = 0;
  crc_byte_count_ = 0;
  message_completed_ = false;
  parse_error_ = false;

  
#line 53 "packet_actions.cpp"
	{
	cs = packet_grammar_start;
	top = 0;
	}

#line 179 "packet_actions.rl"
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

  
#line 78 "packet_actions.cpp"
	{
	if ( p == pe )
		goto _test_eof;
	goto _resume;

_again:
	switch ( cs ) {
		case 1: goto st1;
		case 0: goto st0;
		case 2: goto st2;
		case 3: goto st3;
		case 4: goto st4;
		case 5: goto st5;
		case 6: goto st6;
		case 8: goto st8;
		case 9: goto st9;
		case 7: goto st7;
		case 10: goto st10;
		case 11: goto st11;
	default: break;
	}

	if ( ++p == pe )
		goto _test_eof;
_resume:
	switch ( cs )
	{
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
	if ( (*p) == 126u )
		goto tr1;
	goto tr0;
tr0:
#line 44 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[packet_err]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  parse_error_ = true;
}
	goto st0;
#line 122 "packet_actions.cpp"
st0:
cs = 0;
	goto _out;
tr1:
#line 5 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[startflag_received]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  // We're starting to process a new packet, so reset completed status.
  message_completed_ = false;
}
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 140 "packet_actions.cpp"
	goto tr2;
tr2:
#line 13 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[command_received]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  packet_->command(*p);
}
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 155 "packet_actions.cpp"
	if ( 128u <= (*p) )
		goto tr4;
	goto tr3;
tr3:
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
	{ {stack[top++] = 4; goto st10;} }
	goto st4;
tr9:
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
	{ {stack[top++] = 4; goto st10;} }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 199 "packet_actions.cpp"
	if ( (*p) == 126u )
		goto tr6;
	goto tr5;
tr5:
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
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 222 "packet_actions.cpp"
	goto tr7;
tr7:
#line 135 "packet_actions.rl"
	{
  packet_->crc_ += *p;
  crc_byte_count_++;
}
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 235 "packet_actions.cpp"
	if ( (*p) == 126u )
		goto tr8;
	goto tr0;
tr8:
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
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 269 "packet_actions.cpp"
	goto tr0;
tr6:
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
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 314 "packet_actions.cpp"
	goto tr7;
tr4:
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
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 343 "packet_actions.cpp"
	goto tr9;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	goto tr10;
tr10:
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
	goto st11;
tr11:
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
	goto st11;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
#line 407 "packet_actions.cpp"
	goto tr11;
	}
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 1: 
	case 2: 
	case 3: 
	case 4: 
	case 5: 
	case 6: 
	case 7: 
#line 44 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[packet_err]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  parse_error_ = true;
}
	break;
	case 10: 
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
#line 454 "packet_actions.cpp"
	}
	}

	_out: {}
	}

#line 197 "packet_actions.rl"
}


uint16_t update_crc(uint16_t crc, uint8_t data) {
#ifdef AVR
    crc = _crc16_update(crc, data);
#else
    crc = crc_update_byte(crc, data);
#endif
    return crc;
}
