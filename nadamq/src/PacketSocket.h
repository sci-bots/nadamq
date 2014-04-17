#ifndef ___PACKET_SOCKET__HPP___
#define ___PACKET_SOCKET__HPP___

#include <stdint.h>
#include <stdlib.h>
#include "CircularBuffer.hpp"
#include "Deque.h"

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
  virtual void handle_queue_full() = 0;
  virtual void queue_nack_queue_full() = 0;
  virtual void handle_no_free_packets() = 0;
  virtual void handle_packet() = 0;
  virtual void resend_packet() = 0;
  virtual void pop_packet() = 0;
  virtual void queue_ack() = 0;
};


template <typename StreamPacketParser,
          typename Allocator=PacketAllocator<typename StreamPacketParser::packet_type> >
class StreamPacketSocket : public PacketSocket {
public:
  typedef PacketSocket base_type;
  typedef typename StreamPacketParser::packet_type packet_type;
protected:
  bool push_event(uint8_t event) { return event_queue_.push(event); }

  virtual void process_tx_packet() {
    if (tx_queue_.size() == 0) {
      /* No packet to process. */
      event_queue_.push('N');
      return;
    }
    tx_packet_ = tx_queue_.pop_tail();
#ifndef AVR
    std::cout << std::endl << "# TX packet info #" << std::endl
              << std::endl;
    std::cout << std::setw(23) << "uuid: " << tx_packet_.iuid_ << std::endl;
    std::cout << std::setw(23) << "type: "
              << static_cast<char>(tx_packet_.type()) << std::endl;
    if (tx_packet_.type() == packet_type::packet_type::DATA) {
      std::cout << std::setw(23) << "data payload: " << "'"
                << std::string((char *)tx_packet_.payload_buffer_,
                               tx_packet_.payload_length_) << "'" << std::endl;
    }
#endif  // #ifndef AVR
    allocator_->free_packet_buffer(tx_packet_);
    event_queue_.push('s');
  }

  virtual void process_rx_packet() {
    if (rx_queue_.size() == 0) {
      /* No packet to process. */
      event_queue_.push('N');
      return;
    }
    rx_packet_ = rx_queue_.pop_tail();
#ifndef AVR
    std::cout << std::endl << "## Data packet info ##" << std::endl << std::endl;
    std::cout << std::setw(23) << "uuid: " << rx_packet_.iuid_ << std::endl;
    std::cout << std::setw(23) << "data payload: " << "'"
              << std::string((char *)rx_packet_.payload_buffer_,
                             rx_packet_.payload_length_) << "'" << std::endl;
#endif  // #ifndef AVR
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

  virtual void handle_data_packet() {
    if (rx_queue_.full()) {
      /* A full packet has been parsed successfully, but there is no room in
       * the input queue. Push `queue_full` event onto queue to trigger
       * handling of queue full error. */
      push_event('f');  // `f` -> `queue_full`
    } else {
      event_queue_.push('r');
    }
  }

  virtual void handle_ack_packet() {
    parser_.reset(&parser_packet_);
    event_queue_.push('Y');
  }

  virtual void handle_nack_packet() {
    parser_.reset(&parser_packet_);
    event_queue_.push('N');
  }
public:
  StreamPacketParser &parser_;
  Allocator *allocator_;
  BoundedDeque<packet_type> rx_queue_;
  BoundedDeque<packet_type> tx_queue_;
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
      if (rx_queue_.size() > 0) {
        /* There is a packet available in the receiving buffer.  Push
         * `rx_packet_queued` event onto queue to trigger the next packet to be
         * processed. */
        push_event('I');  // `I` -> `rx_packet_queued` event
      } else if (tx_queue_.size() > 0) {
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
    if (parser_packet_.payload_buffer_ == NULL) {
      /* The parser-packet had its buffer cleared, so we need to try to
       * allocate a new buffer.
       *
       * __NB__ If a new buffer cannot be allocated, the payload buffer of
       * `parser_packet_` will still be `NULL` on the next pass through this
       * method.  The intention is that we can keep attempting to allocate
       * until we succeed _(i.e., until a buffer is deallocated somewhere down
       * the line)_.  Therefore, we push a packet incomplete event onto the
       * event queue to signal that parsing should continue. */
      parser_packet_ = allocator_->create_packet();
      parser_.reset(&parser_packet_);
      push_event('c');
    } else {
      /* This method is triggered by the `rx_stream_available` event. */
      parser_.parse_available();
      if (parser_.ready()) {
        push_event('r');
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
      } else {
        /* There is no packet ready yet, and there has not been a parsing
         * error.  Push a packet-incomplete event to signal that parsing should
         * be resumed. */
        push_event('c');  // `c` -> `packet_incomplete`
      }
    }
  }

  size_t free_packets_available() const {
    /* Return the number of empty packets that are available for allocation. */
    return allocator_->available();
  }

  virtual void update_rx_queue() {
    /* The stream parser has a full packet ready. */
    /*  - Push the full packet onto the receiving queue. */
    push_rx_packet(parser_packet_);

    /*  - Allocate a new packet for the stream parser to use and assign it to
     *   the stream parser to prepare for parsing the next packet. */
    //parser_packet_ = allocator_->create_packet();
    parser_packet_.clear_buffer();
    //parser_.reset(&parser_packet_);
  }

  virtual void handle_no_free_packets() {
    /* Although a full packet has been parsed at this point, there are no more
     * packets available for allocation.
     *
     * If the packet is a data packet , we must discard it and send a `NACK` to
     * indicate that our packet queue is full.  This will trigger the sender to
     * resend the packet, at which point we will hopefully have a free packet
     * available.
     *
     * If the packet is an `ACK` packet, there is no payload, so we can push a
     * packet struct onto the `rx_queue`, but with a payload-buffer size of
     * zero.  This should not be a problem, since `ACK` packets have no payload
     * data.
     *
     * If the packet is a `NACK` packet, there _might_ be a payload.  For now,
     * we will treat `NACK` packets in the same way as we are handling
     * packet struct onto the `rx_queue`, but with a payload-buffer size of
     * zero.  This should not be a problem, since `ACK` packets have no payload
     * data.
     *
     * Note
     * ====
     *
     * Encountering this situation is not ideal, since it involves resending
     * data.  However, it is expected that a full queue should be the
     * exception, rather than typical. In systems where a full packet queue
     * is common, the allocator buffer count could be increased to avoid this
     * scenario.
     * */
#ifndef AVR
    std::cout << "# `handle_no_free_packets()` #" << std::endl;
#endif  // #ifndef AVR
  }

  virtual void handle_queue_full() {
#ifndef AVR
    std::cout << "# `handle_queue_full()` #" << std::endl;
#endif  // #ifndef AVR
  }

  virtual void queue_nack_queue_full() {
#ifndef AVR
    std::cout << "# `queue_nack_queue_full()` #" << std::endl;
#endif  // #ifndef AVR
  }

  virtual void handle_packet() {
    /* # `handle_packet` #
     *
     * Process a fully parsed packet. */
#ifndef AVR
    std::cout << "# `handle_packet()` #" << std::endl;
#endif  // #ifndef AVR
    /* A full packet has been parsed successfully.  Push `packet_ready` event
     * onto queue to trigger update of receiving packet queue. */
    /* Packet type should be: `d` _(data)_, `a` _(ack)_, or `n` _(nack)_. */
    event_queue_.push(parser_packet_.type());
  }

  virtual void resend_packet() {
    /* # `resend_packet()` #
     *
     * We received a `NACK`, so push a sending packet-queued event to trigger
     * the most-recently sent packet to be resent. */
    event_queue_.push('O');
  }

  virtual void pop_packet() {
    /* We received an `ACK`, so we assume that the most-recent packet that we
     * sent was successfully received by the recipient.  Therefore, we can pop
     * the successfully sent packet from our transmission queue. */

    //allocator_->free_packet_buffer(tx_packet_);
  }

  virtual void queue_ack() {
    /* Create an `ACK` packet to signal the successful receipt of the most
     * recently received data packet.  Note that we construct the `ACK` packet
     * as a copy of the received packet to inherit the respective `iuid`, but
     * we clear the buffer, since the `ACK` packet does not carry any payload.
     * */
    packet_type ack = rx_packet_;
    ack.type(packet_type::packet_type::ACK);
    ack.clear_buffer();
    tx_queue_.push(ack);
  }
};

#endif  // #ifndef ___PACKET_SOCKET__HPP___
