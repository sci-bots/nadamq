%%{

machine packet_socket_fsm;

action cleanup {}
action data_too_large {}
action handle_ack {}
action handle_data {}
action handle_nack {}
action initialize {}
action parse_error {}
action pop_packet {}
action queue_ack {}
action queue_full {}
action queue_nack_data_too_large {}
action queue_nack_parse_error {}
action queue_nack_queue_full {}
action read_stream {}
action recv {}
action requeue_packet {}
action send {}
action sent {}
action split_and_queue_packet {}
action update_rx_queue {}

include "packet_socket_fsm.rl";

}%%

#include <stdio.h>
#include <string.h>
#include "PacketSocket.h"

%% write data;

void PacketSocket::reset() {
  %% write init;
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

  %% write exec;
}
