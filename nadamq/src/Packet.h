#ifndef ___PACKET__HPP___
#define ___PACKET__HPP___

#ifndef AVR
/* Assume STL libraries are not available on AVR devices, so don't include
 * methods using them when targeting AVR architectures. */
#include <stdexcept>
#include <string>
using namespace std;
#endif // ifndef AVR

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "crc_common.h"


class PacketBase {
public:
  struct packet_type { enum EnumType {NONE, ACK='a', NACK='n', DATA='d'}; };

  /* Interface unique identifier. */
  uint16_t iuid_;
  packet_type::EnumType type_;
  uint16_t payload_length_;
  uint16_t buffer_size_;
  uint8_t *payload_buffer_;
  uint16_t crc_;

  PacketBase() : iuid_(0), type_(packet_type::NONE), payload_length_(0),
                 buffer_size_(0), payload_buffer_(NULL), crc_(0xFFFF) {}

  template <typename ConvertibleType>
  void type(ConvertibleType type_byte) {
    type_ = (packet_type::EnumType)type_byte;
  }

  packet_type::EnumType type() const { return type_; }

  void clear_buffer() {
    /* This method is intended for instances created using the copy
     * constructor, where we would like to copy all fields from the source
     * packet _except for the buffer_.
     *
     * __NB__ This method does _not_ deallocate the buffer, so may lead to
     * memory leaks if used improperly.  Only use this method if you really
     * understand what it does! */
    payload_buffer_ = NULL;
    payload_length_ = 0;
    buffer_size_ = 0;
  }

  void reset() {
    /* Reset state of packet.
     *
     * __NB__ This method _does not_ deallocate the buffer. */
    type_ = packet_type::NONE;
    payload_length_ = 0;
    crc_ = 0xFFFF;
  }

  uint16_t compute_crc() {
    /* Compute the CRC of the packet payload. */
    crc_ = crc_init();
    for (int i = 0; i < payload_length_; i++) {
      crc_ = update_crc(crc_, payload_buffer_[i]);
    }
    crc_ = finalize_crc(crc_);
    return crc_;
  }

#ifndef AVR
  /*
   * Assume STL libraries are not available on AVR devices, so don't include
   * methods using them when targeting AVR architectures.
   * */
  string data() const {
    if (this->payload_buffer_ != NULL) {
      return std::string((char *)this->payload_buffer_,
                         this->payload_length_);
    } else {
      throw std::runtime_error("No buffer has been set/allocated.");
    }
  }
#endif  // ifndef AVR
};


class FixedPacket : public PacketBase {
  /* # `FixedPacket` #
   *
   * This type of packet assumes no ownership over the payload buffer.  All
   * management of the buffer must be done using one of the `reset_buffer`
   * methods.
   *
   * This packet type is useful, e.g., with the `PacketAllocator`, which
   * creates packets, allocating a new payload buffer for each packet. */
public:
  void reset_buffer(uint16_t buffer_size, uint8_t *buffer) {
    /* Assign a new payload buffer _(may be empty)_. */
    buffer_size_ = buffer_size;
    payload_buffer_ = buffer;
  }

  void reset_buffer() {
    /* Reset the buffer assignment such that the packet has no payload-buffer.
     * */
    reset_buffer(0, NULL);
  }
};


class Packet : public PacketBase {
public:
  bool buffer_owner_;

  Packet() : PacketBase(), buffer_owner_(false) {}

  void assign_buffer(uint16_t buffer_size, uint8_t *buffer) {
    if (buffer_owner_) {
      /* We are the owner of the current buffer, so we must deallocate it
       * before assigning the new buffer. */
      deallocate_buffer();
    }
    buffer_size_ = buffer_size;
    buffer_owner_ = false;
    payload_buffer_ = buffer;
  }

  void reallocate_buffer(uint16_t buffer_size, bool shrink=false) {
    /* Reallocate memory for payload buffer based on specified target size.
     *
     * __NB__ If the size of the buffer is greater than the target size, do
     * nothing, unless `shrink=true`.  If `shrink=true`, shrink the buffer to
     * the target size.  By not shrinking the buffer by default, we can avoid
     * cycles of allocation/deallocation for many consecutive re-allocations,
     * at the expense of some potentially wasted memory remaining allocated
     * between invocations. */
    if ((buffer_size_ < buffer_size) || (shrink && (buffer_size_ >
                                                    buffer_size))) {
      if (buffer_owner_) {
        deallocate_buffer();
      }
      payload_buffer_ = static_cast<uint8_t *>(calloc(buffer_size,
                                                      sizeof(uint8_t)));
      if (payload_buffer_ != NULL) {
        buffer_size_ = buffer_size;
        /* Take note that we are now the owner of the buffer. */
        buffer_owner_ = true;
      }
    }
  }

  void deallocate_buffer() {
    if (payload_buffer_ != NULL) {
      free(payload_buffer_);
      buffer_size_ = 0;
    }
  }

#if 0
  ~Packet() {
    if (buffer_owner_) {
      deallocate_buffer();
    }
  }
#endif

  Packet clone() const {
    Packet packet = *this;
    packet.payload_buffer_ = NULL;
    packet.buffer_size_ = 0;
    /* `reallocate_buffer` sets the new packet as the owner of its buffer. */
    packet.reallocate_buffer(this->buffer_size_);
    strncpy((char *)packet.payload_buffer_, (char *)this->payload_buffer_,
            this->payload_length_);
    return packet;
  }
};


#endif  // #ifndef ___PACKET__HPP___
