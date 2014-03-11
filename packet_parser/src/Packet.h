#ifndef ___PACKET__HPP___
#define ___PACKET__HPP___

#ifndef AVR
/* Assume STL libraries are not available on AVR devices, so don't include
 * methods using them when targeting AVR architectures. */
#include <stdexcept>
#include <string>
using namespace std;
#endif


class Packet {
protected:
  uint8_t command_;

public:
  struct packet_type { enum EnumType {NONE, REQUEST, RESPONSE}; };

  packet_type::EnumType type_;
  uint16_t payload_length_;
  uint16_t buffer_size_;
  uint8_t *payload_buffer_;
  bool has_crc_;
  uint16_t crc_;

  Packet() : command_(0), type_(packet_type::NONE), payload_length_(0),
             buffer_size_(0), payload_buffer_(NULL), has_crc_(false),
             crc_(0xFFFF) {}

  void command(uint8_t command_with_type_msb) {
    /* # `command` #
     *
     * Set the command byte and the type _(i.e., request or response)_ of the
     * packet.
     *
     * ## Type inference from _Most-Significant-Bit (MSB)_ ##
     *
     * The MSB of command specifies whether this packet is a _request_ or a
     * _response_.  Specifically, if the MSB is `1`, the packet is a _request_
     * and if the MSB is `0`, the packet is a _response_.
     *
     * ## Stripped command ##
     *
     * When setting the `command_` byte of the packet, we strip the MSB
     * type-specifier.  In other words, response and request packets for the
     * same command will have the same `command_` byte value, but will differ
     * in the value of the `type_` attribute.
     * */
    if (0x80 & command_with_type_msb) {
      type_ = packet_type::REQUEST;
    } else {
      type_ = packet_type::RESPONSE;
    }
    /*  - Strip MSB type-specifier. */
    command_ = 0x7F & command_with_type_msb;
  }

  uint8_t command() const { return command_; }

  void reset() {
    /* Reset state of packet.
     *
     * __NB__ This method _does not_ deallocate the buffer. */
    type_ = packet_type::NONE;
    command_ = 0;
    payload_length_ = 0;
    has_crc_ = false;
    crc_ = 0xFFFF;
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
      deallocate_buffer();
      payload_buffer_ = static_cast<uint8_t *>(calloc(buffer_size,
                                                      sizeof(uint8_t)));
      if (payload_buffer_ != NULL) {
        buffer_size_ = buffer_size;
      }
    }
  }

  void deallocate_buffer() {
    if (payload_buffer_ != NULL) {
      free(payload_buffer_);
      buffer_size_ = 0;
    }
  }

  ~Packet() {
    deallocate_buffer();
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


#endif  // #ifndef ___PACKET__HPP___
