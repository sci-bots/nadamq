#ifndef ___STREAM__HPP___
#define ___STREAM__HPP___

#include <algorithm>  // `std::copy`


template <typename Stream, size_t buffer_size>
struct StreamWrapper {
  /* Wrap a STL-compatible stream to expose the following Arduino-style API:
   *
   *  - obj.available(): Return number of characters available.
   *  - obj.read(): Return next available character.
   *
   * This wrapper is useful for host testing of code targeting Arduino APIs,
   * such as `Serial`.
   *
   * Notes
   * =====
   *
   * This implementation is _not_ very efficient, since it copies the contents
   * of most of the buffer on _every_ call to `read`.  However, the
   * implementation is quite simple to follow and should be efficient enough
   * for testing, since instances should only be instantiated on a host
   * computer, where the overhead of copying the buffer should be insignificant
   * when dealing with small buffer sizes, such as those used when testing code
   * targeting embedded systems like Arduino. */
  char buffer[buffer_size];
  int byte_count;
  Stream &stream_;

  StreamWrapper(Stream &stream) : byte_count(0), stream_(stream) {}

  size_t available() {
    size_t space_available = buffer_size - byte_count;
    if (space_available > 0) {
      /* We still have space in our buffer, so try to read more bytes to fill
       * up the buffer. */
      stream_.read(buffer, space_available);
      byte_count += stream_.gcount();
    }
    return byte_count;
  }

  char read() {
    /* # `read` #
     *
     * Return next byte from stream. */
    /*  - Read first byte into temporary variable. */
    char byte = buffer[0];
    /*  - Update the number of bytes left in the buffer. */
    byte_count--;
    /*  - Shift remaining buffer contents one byte to the left. */
    std::copy(&buffer[1], &buffer[0] + buffer_size, &buffer[0]);
    return byte;
  }
};


#endif  // #ifndef ___STREAM__HPP___
