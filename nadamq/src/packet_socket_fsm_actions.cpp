
#line 1 "packet_socket_fsm_actions.rl"

#line 29 "packet_socket_fsm_actions.rl"


#include <stdio.h>
#include <string.h>
#include "PacketSocket.h"


#line 13 "packet_socket_fsm_actions.cpp"
static const int packet_socket_fsm_start = 9;
static const int packet_socket_fsm_first_final = 9;
static const int packet_socket_fsm_error = 0;

static const int packet_socket_fsm_en_main = 9;


#line 36 "packet_socket_fsm_actions.rl"

void PacketSocket::reset() {
  
#line 25 "packet_socket_fsm_actions.cpp"
	{
	cs = packet_socket_fsm_start;
	}

#line 39 "packet_socket_fsm_actions.rl"
}


void PacketSocket::parse_byte(uint8_t *byte) {
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

  
#line 49 "packet_socket_fsm_actions.cpp"
	{
	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	switch ( cs ) {
case 9:
	if ( (*p) == 105 )
		goto tr22;
	goto tr1;
case 0:
	goto _out;
case 1:
	switch( (*p) ) {
		case 65: goto tr0;
		case 68: goto tr2;
		case 73: goto tr3;
		case 79: goto tr4;
	}
	goto tr1;
case 2:
	switch( (*p) ) {
		case 76: goto tr5;
		case 99: goto tr6;
		case 101: goto tr7;
		case 102: goto tr8;
		case 114: goto tr9;
	}
	goto tr1;
case 3:
	switch( (*p) ) {
		case 76: goto tr10;
		case 101: goto tr11;
		case 102: goto tr12;
	}
	goto tr1;
case 4:
	switch( (*p) ) {
		case 78: goto tr6;
		case 97: goto tr13;
		case 100: goto tr14;
		case 110: goto tr15;
	}
	goto tr1;
case 5:
	switch( (*p) ) {
		case 78: goto tr16;
		case 89: goto tr17;
	}
	goto tr1;
case 6:
	if ( (*p) == 113 )
		goto tr18;
	goto tr1;
case 7:
	switch( (*p) ) {
		case 76: goto tr19;
		case 78: goto tr16;
	}
	if ( 101 <= (*p) && (*p) <= 102 )
		goto tr20;
	goto tr1;
case 8:
	switch( (*p) ) {
		case 78: goto tr6;
		case 115: goto tr21;
	}
	goto tr1;
	}

	tr1: cs = 0; goto _again;
	tr6: cs = 1; goto _again;
	tr9: cs = 1; goto f7;
	tr10: cs = 1; goto f8;
	tr11: cs = 1; goto f9;
	tr12: cs = 1; goto f10;
	tr16: cs = 1; goto f14;
	tr17: cs = 1; goto f15;
	tr18: cs = 1; goto f16;
	tr19: cs = 1; goto f17;
	tr20: cs = 1; goto f18;
	tr21: cs = 1; goto f19;
	tr22: cs = 1; goto f20;
	tr0: cs = 2; goto f0;
	tr5: cs = 3; goto f4;
	tr7: cs = 3; goto f5;
	tr8: cs = 3; goto f6;
	tr3: cs = 4; goto f2;
	tr13: cs = 5; goto f11;
	tr14: cs = 6; goto f12;
	tr15: cs = 7; goto f13;
	tr4: cs = 8; goto f3;
	tr2: cs = 9; goto f1;

f1:
#line 5 "packet_socket_fsm_actions.rl"
	{}
	goto _again;
f4:
#line 6 "packet_socket_fsm_actions.rl"
	{}
	goto _again;
f14:
#line 7 "packet_socket_fsm_actions.rl"
	{}
	goto _again;
f11:
#line 8 "packet_socket_fsm_actions.rl"
	{ handle_ack_packet(); }
	goto _again;
f12:
#line 9 "packet_socket_fsm_actions.rl"
	{ handle_data_packet(); }
	goto _again;
f13:
#line 10 "packet_socket_fsm_actions.rl"
	{ handle_nack_packet(); }
	goto _again;
f20:
#line 11 "packet_socket_fsm_actions.rl"
	{ idle_state_ = cs; }
	goto _again;
f5:
#line 12 "packet_socket_fsm_actions.rl"
	{}
	goto _again;
f6:
#line 15 "packet_socket_fsm_actions.rl"
	{}
	goto _again;
f8:
#line 16 "packet_socket_fsm_actions.rl"
	{}
	goto _again;
f9:
#line 17 "packet_socket_fsm_actions.rl"
	{}
	goto _again;
f10:
#line 18 "packet_socket_fsm_actions.rl"
	{}
	goto _again;
f0:
#line 19 "packet_socket_fsm_actions.rl"
	{}
	goto _again;
f2:
#line 20 "packet_socket_fsm_actions.rl"
	{ process_rx_packet(); }
	goto _again;
f3:
#line 22 "packet_socket_fsm_actions.rl"
	{}
	goto _again;
f19:
#line 23 "packet_socket_fsm_actions.rl"
	{}
	goto _again;
f7:
#line 25 "packet_socket_fsm_actions.rl"
	{}
	goto _again;
f15:
#line 13 "packet_socket_fsm_actions.rl"
	{}
#line 7 "packet_socket_fsm_actions.rl"
	{}
	goto _again;
f16:
#line 14 "packet_socket_fsm_actions.rl"
	{}
#line 7 "packet_socket_fsm_actions.rl"
	{}
	goto _again;
f18:
#line 21 "packet_socket_fsm_actions.rl"
	{}
#line 7 "packet_socket_fsm_actions.rl"
	{}
	goto _again;
f17:
#line 24 "packet_socket_fsm_actions.rl"
	{}
#line 7 "packet_socket_fsm_actions.rl"
	{}
	goto _again;

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

#line 57 "packet_socket_fsm_actions.rl"
}
