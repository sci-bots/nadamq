
#line 1 "parser_actions.rl"

#line 15 "parser_actions.rl"


#include <stdio.h>
#include <string.h>
#include "PacketParser.hpp"


#line 13 "parser_actions.cpp"
static const char _parser_grammar_actions[] = {
	0, 1, 0, 1, 1
};

static const char _parser_grammar_key_offsets[] = {
	0, 0, 2, 4, 6, 8, 10, 12
};

static const unsigned char _parser_grammar_trans_keys[] = {
	32u, 126u, 32u, 126u, 32u, 126u, 32u, 126u, 
	32u, 126u, 32u, 126u, 0
};

static const char _parser_grammar_single_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0
};

static const char _parser_grammar_range_lengths[] = {
	0, 1, 1, 1, 1, 1, 1, 0
};

static const char _parser_grammar_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 10, 12
};

static const char _parser_grammar_trans_targs[] = {
	2, 0, 3, 0, 4, 0, 5, 0, 
	6, 0, 7, 0, 0, 0
};

static const char _parser_grammar_trans_actions[] = {
	0, 0, 0, 0, 1, 0, 0, 0, 
	0, 0, 3, 0, 0, 0
};

static const int parser_grammar_start = 1;
static const int parser_grammar_first_final = 7;
static const int parser_grammar_error = 0;

static const int parser_grammar_en_main = 1;


#line 22 "parser_actions.rl"

void Parser::reset() {
  message_completed_ = false;
  parse_error_ = false;

  
#line 63 "parser_actions.cpp"
	{
	cs = parser_grammar_start;
	}

#line 28 "parser_actions.rl"
}


void Parser::parse_byte(uint8_t *byte) {
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

  
#line 87 "parser_actions.cpp"
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
	_keys = _parser_grammar_trans_keys + _parser_grammar_key_offsets[cs];
	_trans = _parser_grammar_index_offsets[cs];

	_klen = _parser_grammar_single_lengths[cs];
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

	_klen = _parser_grammar_range_lengths[cs];
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
	cs = _parser_grammar_trans_targs[_trans];

	if ( _parser_grammar_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _parser_grammar_actions + _parser_grammar_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 5 "parser_actions.rl"
	{
  std::cout << "[startflag_received]" << std::endl;
}
	break;
	case 1:
#line 9 "parser_actions.rl"
	{
  std::cout << "[endflag_received]" << std::endl;
}
	break;
#line 172 "parser_actions.cpp"
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

#line 46 "parser_actions.rl"
}
