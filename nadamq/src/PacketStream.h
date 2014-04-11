#ifndef ___PACKET_STREAM__H___
#define ___PACKET_STREAM__H___

#include "Deque.h"

template <typename PacketAllocator>
class PacketStream {
public:
  /* # `PacketStream` #
   *
   * This class implements a stream interface, which queues up packets and
   * exposes a `read_byte` method to allow a consumer to read bytes from
   * packets as an encapsulated byte stream.
   *
   *     Receive packet `P1`
   *                 ____________
   *     --->P1---> |         P1 |
   *                --------------
   *
   *     Receive packet `P2`
   *                 ____________
   *     --->P2---> |      P2,P1 |
   *                --------------
   *
   *     Receive packet `P3`
   *                 ____________
   *     --->P3---> |   P3,P2,P1 |
   *                --------------
   *
   *                                 Read byte(s) from packet `P1`
   *                                 asynchronously.
   *                 ____________
   *                |   P3,P2,P1 | ---->
   *                --------------
   *
   *                                 Packet `P1` empty, so deallocate packet
   *                                 buffer and pop `P1` from queue.
   *                 ____________
   *                |      P3,P2 |
   *                --------------
   *                            |
   *                            \--> P1 (deallocate buffer)
   *
   *     Receive packet `P4`
   *                 ____________
   *     --->P4---> |   P4,P3,P2 |
   *                --------------
   *
   * Notes
   * =====
   *
   * The `PacketAllocator` template type is necessary, since we require an
   * allocator instance to allow us to free each packet after the corresponding
   * final payload byte has been read.  This is based on the assumption that
   * this stream is the last owner of each packet, and as such, each packet
   * should be freed _(i.e., destroyed, deallocated)_ after this stream has
   * processed all of the contained payload data.
   *
   * TODO
   * ====
   *
   *  - Write testbed to push several packets, then read, e.g., test scenario
   *    from above.
   *
   *  - Implement methods from [Arduino `Stream`][1] interface:
   *
   *     - `find()`
   *     - `findUntil()`
   *     - `flush()`
   *     - `parsefloat()`
   *     - `parseInt()`
   *     - `peek()`
   *     - `readBytes()`
   *     - `readBytesUntil()`
   *     - `readString()`
   *     - `readStringUntil()`
   *     - `setTimeout()`
   *
   *  - Done:
   *
   *     - `available()`
   *     - `read()`
   *
   * [1]: http://arduino.cc/en/Reference/Stream
   */
  typedef typename PacketAllocator::packet_type packet_type;
protected:
  PacketAllocator *allocator_;
  BoundedDeque<packet_type> packet_queue_;
  uint8_t *data_;
  size_t bytes_available_;

  size_t packet_available() const {
    /* Return the number of bytes left unread in the active packet. */
    if (packet_queue_.size() == 0 ||
        packet_queue_.tail().payload_length_ == 0) {
      /* There are either no packets in the queue, or the active packet has no
       * payload data. */
      return 0;
    }
    return (packet_queue_.tail().payload_buffer_ +
            packet_queue_.tail().payload_length_) - data_;
  }

  void prepare_active_packet() {
    /* Continue popping the active packet _(i.e., the *tail*)_ from the queue
     * until we either:
     *
     *  1. Find a packet with a non-zero payload length.
     *  2. Empty the packet queue. */
    while (packet_queue_.size() > 0 && packet_available() == 0) {
      allocator_->free_packet_buffer(packet_queue_.tail());
      packet_queue_.pop_tail();
      data_ = packet_queue_.tail().payload_buffer_;
    }
  }

public:
  PacketStream(PacketAllocator *packet_allocator, size_t max_queue_length=1024)
    : allocator_(packet_allocator), packet_queue_(max_queue_length),
      data_(NULL), bytes_available_(0) {}

  PacketAllocator *allocator() const {
    return allocator_;
  }

  size_t packet_count() const {
    return packet_queue_.size();
  }

  packet_type create_packet() {
    return allocator_->create_packet();
  }

  bool push(packet_type &packet) {
    /* Add the number of bytes in the new packet to the total number of bytes
     * available. */
    bytes_available_ += packet.payload_length_;
    if (data_ == NULL) {
      data_ = packet.payload_buffer_;
    }
    return packet_queue_.push(packet);
  }

  int available() const {
    return bytes_available_;
  }

  int read() {
    /* There are no bytes available. */
    if (available() <= 0) {
      return -1;
    }

    prepare_active_packet();
    int value = *data_;
    data_++;
    bytes_available_--;

    if (packet_available() == 0) {
      /* Deallocate packet since we must have just read the last payload byte
       * and activate the next packet _(if available)_ for the next call to
       * `read()`. */
      prepare_active_packet();
    }

    return value;
  }
};


#endif  // #ifndef ___PACKET_STREAM__H___
