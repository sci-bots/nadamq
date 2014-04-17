#ifndef ___BUFFER_ALLOCATOR__H___
#define ___BUFFER_ALLOCATOR__H___

#include <stdint.h>
#ifndef AVR
#include <iostream>
#include <iomanip>
#include <iterator>
#include <algorithm>
#endif


template <size_t BufferSize, size_t Count>
class FixedSizeBufferPool {
  /* # `FixedSizeBufferPool` #
   *
   * Provide pooled allocation of fixed size buffers.  This is particularly
   * helpful in embedded environments, where it is useful to bound memory usage
   * based on types of objects.
   *
   * For example, consider a scenario where we would like to create several
   * buffers to contain data packets.  Since we likely want to be able to reuse
   * buffers, we could create a pool to contain a fixed number of buffers, then
   * allocate and free the buffers as needed using the methods provided below.
   *
   * Advantages compared to regular `malloc`:
   *
   *  - Configurable upper-bound on memory allocated.
   *  - Prevents memory fragmentation.
   *  - Easy to free all memory.
   *
   * Disadvantages:
   *
   *  - No possibility to resize buffers.
   *  - Fixed number of buffers available. */
protected:
  static const size_t padded_buffer_size = sizeof(size_t) + BufferSize;
  uint8_t super_buffer_[Count * padded_buffer_size];
  bool occupied[Count];
  uint32_t next_free;
  uint32_t free_count;
public:
  /* Buffer format
   * =============
   * |<--size_t->||<--buffer size-->|
   * [ buffer ID ][ buffer contents ]
   * |<---- padded_buffer_size ---->|
   */
  FixedSizeBufferPool() : next_free(0), free_count(Count) {
    for (int i = 0; i < padded_buffer_size * Count; i++) {
      super_buffer_[i] = 0;
    }
    for (int i = 0; i < Count; i++) {
      occupied[i] = false;
      uint8_t *padded_buffer = &super_buffer_[i * padded_buffer_size];
      *((size_t *)(padded_buffer)) = i;
    }
  }

  size_t buffer_size() const { return BufferSize; }
  size_t buffer_count() const { return Count; }

  size_t available() const {
    return free_count;
  }

  uint8_t *buffer_by_id(uint32_t id) const {
    return (uint8_t *)&super_buffer_[id * padded_buffer_size + sizeof(size_t)];
  }

  uint8_t *alloc() {
    if (available() == 0) {
      return NULL;
    } else {
      while (occupied[next_free]) {
        next_free = (next_free + 1) % Count;
      }
      uint8_t *buffer = buffer_by_id(next_free);
      occupied[next_free] = true;
      free_count--;
      return buffer;
    }
  }

  void free(uint8_t *buffer) {
    /* Look up the buffer index, which occupies the four bytes immediately
     * preceding the buffer contents in the super-buffer.  Use the buffer index
     * to mark the buffer as unoccupied and increase the free count by one. */
    size_t buffer_id = *((size_t *)(buffer - sizeof(size_t)));
    if (buffer_id < Count && occupied[buffer_id]) {
      occupied[buffer_id] = false;
      free_count++;
    }
  }

#ifndef AVR
  void dump() const {
    std::cout << std::hex;
    for (int i = 0; i < Count; i++) {
      if (occupied[i]) {
        char *buffer = (char *)buffer_by_id(i);

        std::cout << std::setw(15) << "[" << i << "]: '";
        std::copy(buffer, buffer + BufferSize,
                  std::ostream_iterator<char>(std::cout, ""));
        std::cout << "'" << std::endl;
      }
    }
  }
#endif  // #ifndef AVR
};


#endif  // #ifndef ___BUFFER_ALLOCATOR__H___
