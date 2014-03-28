#ifndef ___PACKET_SOCKET__HPP___
#define ___PACKET_SOCKET__HPP___

#include <stdint.h>
#include <stdlib.h>

#ifndef AVR
/* Assume STL libraries are not available on AVR devices, so don't include
 * methods using them when targeting AVR architectures. */
#include <iostream>
using namespace std;
#endif // ifndef AVR


class PacketSocket {
protected:
  uint8_t *buffer_;
  int cs;
  uint8_t *p, *pe;
  uint8_t *eof;
  uint8_t *start;

public:
  PacketSocket(size_t buffer_size)
    : buffer_((uint8_t *)malloc(buffer_size)) {}

  int state() const { return cs; }
  void state(int value) { cs = value; }

  void reset();

  void parse_byte(uint8_t *byte);

  ~PacketSocket() {
    if (buffer_ != NULL) {
      free(buffer_);
    }
  }
};

#endif  // #ifndef ___PACKET_SOCKET__HPP___
