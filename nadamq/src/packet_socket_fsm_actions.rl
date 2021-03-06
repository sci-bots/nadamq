%%{

machine packet_socket_fsm;

action cleanup {}
action data_too_large {}
action finished_rx {}
action handle_ack { handle_ack_packet(); }
action handle_data { handle_data_packet(); }
action handle_nack { handle_nack_packet(); }
action handle_packet { handle_packet(); }
action initialize { idle_state_ = cs; }
action no_free_packets { handle_no_free_packets(); }
action parse_error {}
action pop_packet { pop_packet(); }
action queue_ack { queue_ack(); }
action queue_full { handle_queue_full(); }
action queue_nack_data_too_large {}
action queue_nack_parse_error {}
action queue_nack_queue_full { queue_nack_queue_full(); }
action read_stream { read_stream(); }
action recv { process_rx_packet(); }
action resend_packet { resend_packet(); }
action send { process_tx_packet(); }
action sent {}
action split_and_queue_packet {}
action update_rx_queue { update_rx_queue(); }

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
