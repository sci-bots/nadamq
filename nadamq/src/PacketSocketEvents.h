#ifndef ___PACKET_SOCKET_EVENTS__HPP___
#define ___PACKET_SOCKET_EVENTS__HPP___

#include <string>

inline std::string event_label(uint8_t event) {
  switch (event) {

    case 'a': return "ack_packet";
    case 'd': return "data_packet";
    case 'L': return "data_too_large";
    case 'D': return "done";
    case 'i': return "initialize";
    case 'n': return "nack_packet";
    case 'Y': return "packet_found";
    case 'c': return "packet_incomplete";
    case 'N': return "packet_not_found";
    case 'r': return "packet_ready";
    case 's': return "packet_sent";
    case 'e': return "parse_error";
    case 'q': return "queued_ok";
    case 'f': return "queue_full";
    case 'F': return "no_free_packets";
    case 'I': return "rx_packet_queued";
    case 'A': return "rx_stream_available";
    case 'O': return "tx_packet_queued";

    default: return "<unknown event>";
  }
}

#endif  // #ifndef ___PACKET_SOCKET_EVENTS__HPP___

