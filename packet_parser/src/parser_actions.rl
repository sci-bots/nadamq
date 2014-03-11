%%{

machine parser_grammar;

action startflag_received {
  std::cout << "[startflag_received]" << std::endl;
}

action endflag_received {
  std::cout << "[endflag_received]" << std::endl;
}

include "parser_grammar.rl";

}%%

#include <stdio.h>
#include <string.h>
#include "PacketParser.h"

%% write data;

void Parser::reset() {
  message_completed_ = false;
  parse_error_ = false;

  %% write init;
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

  %% write exec;
}
