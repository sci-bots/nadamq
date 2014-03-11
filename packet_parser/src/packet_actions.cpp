
#line 1 "packet_actions.rl"

#line 142 "packet_actions.rl"


#include <stdio.h>
#include <string.h>
#include "PacketParser.hpp"


#line 13 "packet_actions.cpp"
static const char _packet_grammar_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 9, 1, 10, 1, 12, 2, 
	4, 11, 2, 5, 6, 2, 7, 13, 
	2, 9, 10, 3, 4, 11, 2, 3, 
	5, 8, 13
};

static const char _packet_grammar_key_offsets[] = {
	0, 0, 1, 1, 3, 4, 4, 5, 
	5, 5, 5, 5
};

static const unsigned char _packet_grammar_trans_keys[] = {
	126u, 128u, 255u, 126u, 126u, 0
};

static const char _packet_grammar_single_lengths[] = {
	0, 1, 0, 0, 1, 0, 1, 0, 
	0, 0, 0, 0
};

static const char _packet_grammar_range_lengths[] = {
	0, 0, 0, 1, 0, 0, 0, 0, 
	0, 0, 0, 0
};

static const char _packet_grammar_index_offsets[] = {
	0, 0, 2, 3, 5, 7, 8, 10, 
	11, 12, 13, 14
};

static const char _packet_grammar_trans_targs[] = {
	2, 0, 3, 7, 4, 9, 5, 6, 
	8, 0, 4, 0, 6, 11, 11, 0
};

static const char _packet_grammar_trans_actions[] = {
	1, 7, 3, 18, 31, 27, 15, 13, 
	5, 7, 21, 7, 13, 24, 11, 0
};

static const char _packet_grammar_eof_actions[] = {
	0, 7, 7, 7, 7, 7, 7, 7, 
	0, 0, 9, 0
};

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

  
#line 100 "packet_actions.cpp"
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

  
#line 125 "packet_actions.cpp"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const unsigned char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _packet_grammar_trans_keys + _packet_grammar_key_offsets[cs];
	_trans = _packet_grammar_index_offsets[cs];

	_klen = _packet_grammar_single_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _packet_grammar_range_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	cs = _packet_grammar_trans_targs[_trans];

	if ( _packet_grammar_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _packet_grammar_actions + _packet_grammar_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 5 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[startflag_received]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  // We're starting to process a new packet, so reset completed status.
  message_completed_ = false;
}
	break;
	case 1:
#line 13 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[command_received]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  packet_->command(*p);
}
	break;
	case 2:
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
	break;
	case 3:
#line 44 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[packet_err]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  parse_error_ = true;
}
	break;
	case 4:
#line 51 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[payload_end] received: " << payload_bytes_received_ << "/"
            << payload_bytes_expected_ << std::endl;
#endif  // #ifdef VERBOSE_STATES
}
	break;
	case 5:
#line 58 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[payloadlength_start]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  // Reset received-bytes counter.
  payload_bytes_expected_ = 0;
}
	break;
	case 6:
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
	break;
	case 7:
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
	break;
	case 8:
#line 91 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[payloadlength_single]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  /* Received single-octet payload-length. */
  payload_bytes_expected_ = (int)(*p);
}
	break;
	case 9:
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
	case 10:
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
    {cs = stack[--top]; goto _again;}
  }
}
	break;
	case 11:
#line 129 "packet_actions.rl"
	{
  crc_ = crc_init();
  crc_byte_count_ = 1;
  packet_->crc_ = (*p) << 8;
}
	break;
	case 12:
#line 135 "packet_actions.rl"
	{
  packet_->crc_ += *p;
  crc_byte_count_++;
}
	break;
	case 13:
#line 47 "packet.rl"
	{ {stack[top++] = cs; cs = 10; goto _again;} }
	break;
#line 363 "packet_actions.cpp"
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	const char *__acts = _packet_grammar_actions + _packet_grammar_eof_actions[cs];
	unsigned int __nacts = (unsigned int) *__acts++;
	while ( __nacts-- > 0 ) {
		switch ( *__acts++ ) {
	case 3:
#line 44 "packet_actions.rl"
	{
#ifdef VERBOSE_STATES
  std::cout << "[packet_err]" << std::endl;
#endif  // #ifdef VERBOSE_STATES
  parse_error_ = true;
}
	break;
	case 9:
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
#line 401 "packet_actions.cpp"
		}
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
