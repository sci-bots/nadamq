#ifndef ___PACKET_ALLOCATOR__H___
#define ___PACKET_ALLOCATOR__H___

#include <stdint.h>
#ifndef AVR
#include <iostream>
#include <iomanip>
#include <iterator>
#include <algorithm>
#endif  // #ifndef AVR
#include "BufferAllocator.h"


template <typename Packet, typename Allocator=FixedSizeBufferPool<128, 10> >
class PacketAllocator {
  /* # `PacketAllocator` #
   *
   * Provide factory for packets, which allocates packet buffers using the
   * provided buffer allocator.  This is particularly helpful in embedded
   * environments, where it is useful to bound memory usage.  In such a
   * scenario, a specialized allocator may be used to specialize memory
   * allocation and management _(e.g., `FixedSizeBufferPool`)_. */
protected:
  Allocator *buffer_allocator_;
public:
  typedef Packet packet_type;
  typedef Allocator allocator_type;

  /* Buffer format
   * =============
   * |<--size_t->||<--buffer size-->|
   * [ buffer ID ][ buffer contents ]
   * |<---- padded_buffer_size ---->|
   */
  PacketAllocator() : buffer_allocator_(new Allocator()) {}
  PacketAllocator(Allocator *allocator) : buffer_allocator_(allocator) {}

  Packet create_packet() {
    Packet packet;
    /* Allocate new, fixed-size packet buffer using buffer-allocator. */
    uint8_t *buffer = buffer_allocator_->alloc();
    size_t buffer_size;
    if (buffer != NULL) {
      /* Buffer was allocated successfully. */
      buffer_size = buffer_allocator_->buffer_size();
    } else {
      /* Could not allocate buffer _(likely because allocator ran out of
       * memory)_. */
      buffer_size = 0;
    }
    /* Assign buffer to packet. */
    packet.reset_buffer(buffer_size, buffer);
#ifndef AVR
    std::cout << "# `PacketAllocator::create_packet()` (" << available()
              << "/" << buffer_allocator_->buffer_count() << ") now available #"
              << std::endl << std::endl;
#endif  // #ifndef AVR
    return packet;
  }

  void free_packet_buffer(Packet &packet) {
    /* Free the buffer of a packet that was allocated during a call to
     * `create_packet`.  Note that behaviour is _undefined_ if the buffer was
     * allocated some other way. */
    if (packet.payload_buffer_ != NULL) {
      buffer_allocator_->free(packet.payload_buffer_);
    }
#ifndef AVR
    std::cout << "# `PacketAllocator::free_packet_buffer()` (" << available()
              << "/" << buffer_allocator_->buffer_count() << ") now available #" << std::endl
              << std::endl;
#endif  // #ifndef AVR
  }

  size_t available() const {
    return buffer_allocator_->available();
  }
};


#endif  // #ifndef ___PACKET_ALLOCATOR__H___
