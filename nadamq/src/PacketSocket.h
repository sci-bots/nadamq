#ifndef ___PACKET_SOCKET__HPP___
#define ___PACKET_SOCKET__HPP___

#include <stdint.h>
#include <stdlib.h>
#include "CircularBuffer.hpp"

#ifndef AVR
/* Assume STL libraries are not available on AVR devices, so don't include
 * methods using them when targeting AVR architectures. */
#include <iostream>
using namespace std;
#endif // ifndef AVR


class PacketSocket {
protected:
  int cs;
  uint8_t *p, *pe;
  uint8_t *eof;
  uint8_t *start;
  int idle_state_;
public:
  PacketSocket() : idle_state_(-1) {}

  int idle() const { return (idle_state_ >= 0 && cs == idle_state_); }
  int state() const { return cs; }
  void state(int value) { cs = value; }

  void reset();

  void parse_byte(uint8_t *byte);
  virtual void process_rx_packet() = 0;
  virtual void process_tx_packet() = 0;
  virtual void handle_data_packet() = 0;
  virtual void handle_ack_packet() = 0;
  virtual void handle_nack_packet() = 0;
};


template <typename Packet>
class SerialPacketSocket : public PacketSocket {
protected:
  bool push_event(uint8_t event) { return event_queue_.push(event); }

  virtual void process_tx_packet() {}

  virtual void process_rx_packet() {
    if (rx_queue_.isEmpty()) {
      /* No packet to process. */
      event_queue_.push('N');
      return;
    }
    rx_queue_.pop(rx_packet_);
    /* Packet type should be: `d` _(data)_, `a` _(ack)_, or `n` _(nack)_. */
    event_queue_.push(rx_packet_.type);
  }

  virtual void handle_data_packet() { event_queue_.push('q'); }
  virtual void handle_ack_packet() { event_queue_.push('Y'); }
  virtual void handle_nack_packet() { event_queue_.push('N'); }
public:
  CircularBuffer<Packet> rx_queue_;
  CircularBuffer<Packet> tx_queue_;
  CircularBuffer<uint8_t> event_queue_;
  Packet rx_packet_;
  Packet tx_packet_;

  SerialPacketSocket(size_t event_queue_length, size_t rx_queue_length,
                     size_t tx_queue_length)
    : PacketSocket(), event_queue_(event_queue_length),
      rx_queue_(rx_queue_length), tx_queue_(tx_queue_length) {
    /* Push initialization event. */
    push_event('i');
  }

  int available() {
    if (idle()) {
      /* We are currently in the idle state, so check packet queues and stream
       * to see if there is any data to be processed. */
      if (rx_queue_.available() > 0) {
        /* There is a packet available in the receiving buffer.  Push
         * `rx_packet_queued` event onto queue to trigger the next packet to be
         * processed. */
        push_event('I');  // `I` -> `rx_packet_queued` event
      } else if (tx_queue_.available() > 0) {
        /* There is a packet available in the transmitting buffer.  Push
         * `tx_packet_queued` event onto queue to trigger the next packet to be
         * sent. */
        push_event('O');  // `O` -> `tx_packet_queued` event
      }
    }
    return event_queue_.available();
  }

  uint8_t pop_event() {
    uint8_t event;

    event_queue_.pop(event);
    parse_byte(&event);
    return event;
  }

  bool push_rx_packet(Packet packet) {
    /* Add a packet to the incoming queue. */
    bool result = rx_queue_.push(packet);
    return result;
  }
};

#endif  // #ifndef ___PACKET_SOCKET__HPP___
