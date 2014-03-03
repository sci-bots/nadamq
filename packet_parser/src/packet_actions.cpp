
#line 1 "packet_actions.rl"

#line 97 "packet_actions.rl"


#include <stdio.h>
#include <string.h>
#include "PacketParser.hpp"


#line 13 "packet_actions.c"
static const char _packet_grammar_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 8, 1, 9, 1, 10, 1, 
	11, 2, 4, 5, 2, 6, 12, 2, 
	8, 9, 2, 10, 2, 3, 4, 7, 
	12
};

static const char _packet_grammar_key_offsets[] = {
	0, 0, 1, 3, 5, 6, 6, 7, 
	7, 7, 7, 7
};

static const unsigned char _packet_grammar_trans_keys[] = {
	126u, 128u, 255u, 128u, 255u, 126u, 126u, 0
};

static const char _packet_grammar_single_lengths[] = {
	0, 1, 0, 0, 1, 0, 1, 0, 
	0, 0, 0, 0
};

static const char _packet_grammar_range_lengths[] = {
	0, 0, 1, 1, 0, 0, 0, 0, 
	0, 0, 0, 0
};

static const char _packet_grammar_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 9, 11, 
	12, 13, 14, 15
};

static const char _packet_grammar_trans_targs[] = {
	2, 0, 3, 0, 7, 4, 9, 5, 
	6, 8, 0, 4, 0, 6, 11, 11, 
	0
};

static const char _packet_grammar_trans_actions[] = {
	1, 0, 3, 7, 17, 29, 26, 13, 
	15, 5, 0, 20, 0, 15, 23, 11, 
	0
};

static const char _packet_grammar_eof_actions[] = {
	0, 0, 7, 7, 7, 0, 0, 7, 
	0, 0, 9, 0
};

static const int packet_grammar_start = 1;
static const int packet_grammar_first_final = 8;
static const int packet_grammar_error = 0;

static const int packet_grammar_en_process_payload = 10;
static const int packet_grammar_en_main = 1;


#line 104 "packet_actions.rl"


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

    
#line 113 "packet_actions.c"
	{
	cs = packet_grammar_start;
	top = 0;
	}

#line 145 "packet_actions.rl"
    
#line 121 "packet_actions.c"
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
    // We're starting to process a new packet, so reset completed status.
    this->packet_completed_ = false;
}
	break;
	case 1:
#line 10 "packet_actions.rl"
	{
    packet->command_ = *p;
}
	break;
	case 2:
#line 14 "packet_actions.rl"
	{
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
	break;
	case 3:
#line 26 "packet_actions.rl"
	{
#ifndef AVR
    /*
     * Assume STL libraries are not available on AVR devices, so don't include
     * methods using them when targeting AVR architectures.
     * */

    cerr << "parse error near byte[" << (p - start) << "]: "
         << std::string((char *)start, (p - start) + 1) << endl;
#endif // ifndef AVR
}
	break;
	case 4:
#line 38 "packet_actions.rl"
	{
    // Reset received-bytes counter.
    packet->payload_length_ = 0;
}
	break;
	case 5:
#line 43 "packet_actions.rl"
	{
    /* Received first octet of a two-octet payload-length.  Clear the top bit.
     * Clear the top bit and shift over 8-bits, since this is just the
     * _Most-Significant-Byte (MSB)_.
     *
     * See also: `payloadlength_lsb`.
     */
    packet->payload_length_ = (int)(((*p) & 0x7F)) << 8;
}
	break;
	case 6:
#line 53 "packet_actions.rl"
	{
    /* Received _Least-Significant-Byte (i.e., LSB)_ of a two-octet
     * payload-length, so add value to previous MSB
     *
     * See also: `payloadlength_msb`.
     */
    packet->payload_length_ += (int)(*p);
}
	break;
	case 7:
#line 62 "packet_actions.rl"
	{
    /* Received single-octet payload-length. */
    packet->payload_length_ = (int)(*p);
}
	break;
	case 8:
#line 67 "packet_actions.rl"
	{
    // Reset received-bytes counter.
    this->payload_bytes_received_ = 0;
    packet->payload_buffer_ = p;
}
	break;
	case 9:
#line 73 "packet_actions.rl"
	{
    /* We received another payload octet, so:
     *
     *   - Update CRC checksum.
     *   - Increment received count.
     *   - Check if we've received all expected octets. */
    crc = update_crc(crc, *p);
    if (++this->payload_bytes_received_ == packet->payload_length_) {
        /* We've received the expected number of payload octets. */
        {cs = stack[--top]; goto _again;}
    }
}
	break;
	case 10:
#line 86 "packet_actions.rl"
	{
    crc_start = p;
    packet->crc_ = (*p) << 8;
}
	break;
	case 11:
#line 91 "packet_actions.rl"
	{
    packet->crc_ += *p;
}
	break;
	case 12:
#line 47 "packet.rl"
	{ {stack[top++] = cs; cs = 10; goto _again;} }
	break;
#line 312 "packet_actions.c"
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
#line 26 "packet_actions.rl"
	{
#ifndef AVR
    /*
     * Assume STL libraries are not available on AVR devices, so don't include
     * methods using them when targeting AVR architectures.
     * */

    cerr << "parse error near byte[" << (p - start) << "]: "
         << std::string((char *)start, (p - start) + 1) << endl;
#endif // ifndef AVR
}
	break;
	case 8:
#line 67 "packet_actions.rl"
	{
    // Reset received-bytes counter.
    this->payload_bytes_received_ = 0;
    packet->payload_buffer_ = p;
}
	break;
#line 350 "packet_actions.c"
		}
	}
	}

	_out: {}
	}

#line 146 "packet_actions.rl"

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
