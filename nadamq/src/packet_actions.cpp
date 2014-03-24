
#line 1 "packet_actions.rl"

#line 117 "packet_actions.rl"


#include <stdio.h>
#include <string.h>
#include "PacketParser.h"


#line 13 "packet_actions.cpp"
static const char _packet_grammar_eof_actions[] = {
	0, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 0, 9, 0
};

static const int packet_grammar_start = 1;
static const int packet_grammar_first_final = 10;
static const int packet_grammar_error = 0;

static const int packet_grammar_en_process_payload = 11;
static const int packet_grammar_en_main = 1;


#line 124 "packet_actions.rl"

void PacketParser::reset() {
  /*
   * Attempt to parse a packet from a buffer with length `buffer_len`.
   *
   * If successful, return `true` and set:
   *
   *  - `packet.type_`
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
  message_completed_ = false;
  parse_error_ = false;

  
#line 56 "packet_actions.cpp"
	{
	cs = packet_grammar_start;
	top = 0;
	}

#line 152 "packet_actions.rl"
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

  
#line 81 "packet_actions.cpp"
	{
	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	switch ( cs ) {
case 1:
	if ( (*p) == 124u )
		goto tr1;
	goto tr0;
case 0:
	goto _out;
case 2:
	if ( (*p) == 124u )
		goto tr2;
	goto tr0;
case 3:
	if ( (*p) == 124u )
		goto tr3;
	goto tr0;
case 4:
	goto tr4;
case 5:
	goto tr5;
case 6:
	goto tr6;
case 7:
	goto tr7;
case 8:
	goto tr8;
case 9:
	goto tr9;
case 10:
	goto tr0;
case 11:
	goto tr10;
case 12:
	goto tr11;
	}

	tr0: cs = 0; goto f0;
	tr1: cs = 2; goto _again;
	tr2: cs = 3; goto _again;
	tr3: cs = 4; goto f1;
	tr4: cs = 5; goto f2;
	tr5: cs = 6; goto f3;
	tr6: cs = 7; goto f4;
	tr7: cs = 8; goto f5;
	tr8: cs = 9; goto f6;
	tr9: cs = 10; goto f7;
	tr10: cs = 12; goto f9;
	tr11: cs = 12; goto f10;

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
f3:
#line 21 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[id_octet_received]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  /* Shift previous contents of interface unique packet identifier 8-bits to
   * the left, and write incoming byte as next byte of identifier. */
  packet_->iuid_ = (packet_->iuid_ << 8) | *p;
}
	goto _again;
f4:
#line 30 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[type_received]: " << static_cast<int>(*p) << std::endl;
#endif  // #ifdef VERBOSE_STATES
  packet_->type(*p);
}
	goto _again;
f10:
#line 55 "packet_actions.rl"
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
#line 86 "packet_actions.rl"
	{
  packet_->crc_ += *p;
  if (packet_->crc_ == crc_) {
    /* The CRC checksum computed based on payload contents matches the CRC
     * checksum included from the packet.  We assume the packet was
     * successfully received.
     *
     * TODO
     * ====
     *
     * The CRC checksum should be computed from _all bytes in the packet_, not
     * just the _payload_.  This will help to ensure that the identifier, type,
     * etc. are not corrupted during transmission. */
    message_completed_ = true;
    /* Update payload length, since we successfully parsed the packet. */
    packet_->payload_length_ = payload_bytes_received_;
  } else {
    /* Reset state of packet, since the parsing was not successful. */
    parse_error_ = true;
  }
}
	goto _again;
f0:
#line 108 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[packet_err]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  parse_error_ = true;
}
	goto _again;
f2:
#line 13 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[id_start]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  /* Reset the interface unique packet identifier. */
  packet_->iuid_ = 0;
}
#line 21 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[id_octet_received]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  /* Shift previous contents of interface unique packet identifier 8-bits to
   * the left, and write incoming byte as next byte of identifier. */
  packet_->iuid_ = (packet_->iuid_ << 8) | *p;
}
	goto _again;
f5:
#line 37 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[length_received]: " << static_cast<int>(*p) << std::endl;
#endif  // #ifdef VERBOSE_STATES
  payload_bytes_expected_ = *p;
}
#line 54 "packet.rl"
	{ {stack[top++] = cs; cs = 11; goto _again;} }
	goto _again;
f9:
#line 44 "packet_actions.rl"
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
#line 55 "packet_actions.rl"
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
#line 74 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[payload_end] received: " << payload_bytes_received_ << "/"
            << payload_bytes_expected_ << std::endl;
#endif  // #ifdef VERBOSE_STATES
}
#line 81 "packet_actions.rl"
	{
  crc_ = crc_init();
  packet_->crc_ = (*p) << 8;
}
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
	case 9:
#line 44 "packet_actions.rl"
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
	case 1:
#line 108 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[packet_err]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  parse_error_ = true;
}
	break;
#line 328 "packet_actions.cpp"
	}
	}

	_out: {}
	}

#line 170 "packet_actions.rl"
}


uint16_t update_crc(uint16_t crc, uint8_t data) {
#ifdef AVR
    crc = _crc16_update(crc, data);
#else
    crc = crc_update_byte(crc, data);
#endif
    return crc;
}
