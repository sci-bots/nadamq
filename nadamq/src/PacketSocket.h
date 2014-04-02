#ifndef ___PACKET_SOCKET__HPP___
#define ___PACKET_SOCKET__HPP___

#include <stdint.h>
#include <stdlib.h>
#include "CircularBuffer.hpp"

#ifndef AVR
/* Assume STL libraries are not available on AVR devices, so don't include
 * methods using them when targeting AVR architectures. */
#include <iostream>
#include <iomanip>
using namespace std;
#endif // ifndef AVR

#include "PacketAllocator.h"


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
  virtual void read_stream() = 0;
  virtual void update_rx_queue() = 0;
};


template <typename StreamPacketParser,
          typename Allocator=PacketAllocator<typename StreamPacketParser::packet_type> >
class StreamPacketSocket : public PacketSocket {
public:
  typedef PacketSocket base_type;
  typedef typename StreamPacketParser::packet_type packet_type;
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
    event_queue_.push(rx_packet_.type());
  }

  virtual void handle_data_packet() {
    std::cout << std::setw(23) << "data payload : " << "'"
              << std::string((char *)rx_packet_.payload_buffer_,
                             rx_packet_.payload_length_) << "'" << std::endl;
    /* Need to deallocate buffer here until we transfer ownership to somewhere
     * else, e.g., packet-stream. */
    /* TODO
     * ====
     *
     * We probably need to assign a packet allocator to the socket, which can
     * allocate and free packets. */
    allocator_->free_packet_buffer(rx_packet_);
    event_queue_.push('q');
  }

  virtual void handle_ack_packet() {
    allocator_->free_packet_buffer(rx_packet_);
    event_queue_.push('Y');
  }

  virtual void handle_nack_packet() {
    allocator_->free_packet_buffer(rx_packet_);
    event_queue_.push('N');
  }
public:
  StreamPacketParser &parser_;
  Allocator *allocator_;
  CircularBuffer<packet_type> rx_queue_;
  CircularBuffer<packet_type> tx_queue_;
  CircularBuffer<uint8_t> event_queue_;
  packet_type parser_packet_;
  /* Complete packet that is currently being processed. */
  packet_type rx_packet_;
  /* Complete packet that is currently being processed. */
  packet_type tx_packet_;

  StreamPacketSocket(StreamPacketParser &parser, Allocator *allocator,
                     size_t event_queue_length, size_t rx_queue_length,
                     size_t tx_queue_length)
    : PacketSocket(), parser_(parser), allocator_(allocator),
       rx_queue_(rx_queue_length), tx_queue_(tx_queue_length),
       event_queue_(event_queue_length),
       parser_packet_(allocator->create_packet()) {
    /* Assign the packet instance we've allocated for receiving to the packet
     * parser.  The packet parser will parse the incoming byte stream and fill
     * the receiving packet accordingly. */
    parser_.reset(&parser_packet_);
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
      } else if (parser_.available() > 0) {
        /* There is data ready on the input stream. Push `rx_stream_available`
         * event onto queue to trigger the set of available bytes to be read
         * from the stream. */
        push_event('A');  // `A` -> `rx_stream_available` event
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

  bool push_rx_packet(packet_type packet) {
    /* Add a packet to the incoming queue. */
    bool result = rx_queue_.push(packet);
    return result;
  }

  virtual void read_stream() {
    /* This method is triggered by the `rx_stream_available` event. */
    parser_.parse_available();
    if (parser_.ready()) {
      if (rx_queue_.isFull()) {
        /* A full packet has been parsed successfully, but there is no room in
         * the input queue. Push `queue_full` event onto queue to trigger
         * handling of queue full error. */
        push_event('f');  // `f` -> `queue_full`
      } else {
        /* A full packet has been parsed successfully.  Push `packet_ready` event
         * onto queue to trigger update of receiving packet queue. */
        push_event('r');  // `r` -> `packet_ready`
      }
    } else if (parser_.error() != 0) {
      /* An error occurred while parsing from the input stream.  Push an event
       * corresponding to the returned error-code onto the event queue to
       * process the error as necessary. */
      switch (parser_.error()) {
        case 'e':
          push_event('e');
          break;
        case 'L':
          push_event('L');
          break;
        default:
          push_event('e');
          break;
      }
    }
  }

  virtual void update_rx_queue() {
    /* The stream parser has a full packet ready.  Push the full packet onto
     * the receiving queue. */
    push_rx_packet(parser_packet_);

    /* Allocate a new packet for the stream parser to use and assign it to the
     * stream parser to prepare for parsing the next packet. */
    parser_packet_ = allocator_->create_packet();
    parser_.reset(&parser_packet_);
  }
};

#endif  // #ifndef ___PACKET_SOCKET__HPP___
