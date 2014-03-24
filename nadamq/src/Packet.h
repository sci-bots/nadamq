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
public:
  struct packet_type { enum EnumType {NONE, ACK=5, NACK=6, DATA=7}; };

  /* Interface unique identifier. */
  uint16_t iuid_;
  packet_type::EnumType type_;
  uint16_t payload_length_;
  uint16_t buffer_size_;
  uint8_t *payload_buffer_;
  bool has_crc_;
  uint16_t crc_;

  Packet() : iuid_(0), type_(packet_type::NONE), payload_length_(0),
             buffer_size_(0), payload_buffer_(NULL), has_crc_(false),
             crc_(0xFFFF) {}

  template <typename ConvertibleType>
  void type(ConvertibleType type_byte) {
    type_ = (packet_type::EnumType)type_byte;
  }

  packet_type::EnumType type() const { return type_; }

  void reset() {
    /* Reset state of packet.
     *
     * __NB__ This method _does not_ deallocate the buffer. */
    type_ = packet_type::NONE;
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
