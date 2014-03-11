
#line 1 "parser_actions.rl"

#line 15 "parser_actions.rl"


#include <stdio.h>
#include <string.h>
#include "PacketParser.h"


#line 13 "parser_actions.cpp"
static const int parser_grammar_start = 1;
static const int parser_grammar_first_final = 7;
static const int parser_grammar_error = 0;

static const int parser_grammar_en_main = 1;


#line 22 "parser_actions.rl"

void Parser::reset() {
  message_completed_ = false;
  parse_error_ = false;

  
#line 28 "parser_actions.cpp"
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

  
#line 52 "parser_actions.cpp"
	{
	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	switch ( cs ) {
case 1:
	if ( 32u <= (*p) && (*p) <= 126u )
		goto tr0;
	goto tr1;
case 0:
	goto _out;
case 2:
	if ( 32u <= (*p) && (*p) <= 126u )
		goto tr2;
	goto tr1;
case 3:
	if ( 32u <= (*p) && (*p) <= 126u )
		goto tr3;
	goto tr1;
case 4:
	if ( 32u <= (*p) && (*p) <= 126u )
		goto tr4;
	goto tr1;
case 5:
	if ( 32u <= (*p) && (*p) <= 126u )
		goto tr5;
	goto tr1;
case 6:
	if ( 32u <= (*p) && (*p) <= 126u )
		goto tr6;
	goto tr1;
case 7:
	goto tr1;
	}

	tr1: cs = 0; goto _again;
	tr0: cs = 2; goto _again;
	tr2: cs = 3; goto _again;
	tr3: cs = 4; goto f0;
	tr4: cs = 5; goto _again;
	tr5: cs = 6; goto _again;
	tr6: cs = 7; goto f1;

f0:
#line 5 "parser_actions.rl"
	{
  std::cout << "[startflag_received]" << std::endl;
}
	goto _again;
f1:
#line 9 "parser_actions.rl"
	{
  std::cout << "[endflag_received]" << std::endl;
}
	goto _again;

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
