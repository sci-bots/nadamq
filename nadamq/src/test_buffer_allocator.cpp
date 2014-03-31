#include "BufferAllocator.h"


int main(int argc, const char *argv[]) {
  FixedSizeBufferAllocator<8, 2> allocator;
  std::cout << std::boolalpha;
  uint8_t *buffer0 = allocator.alloc();
  uint8_t *buffer1 = allocator.alloc();
  uint8_t *buffer2 = allocator.alloc();
  if (buffer0 != NULL) {
    for (int i = 0; i < 8; i++) {
      buffer0[i] = '0';
    }
  }
  if (buffer1 != NULL) {
    for (int i = 0; i < 8; i++) {
      buffer1[i] = '1';
    }
  }
  std::cout << std::endl << std::endl;
  allocator.dump();
  allocator.free(buffer0);
  std::cout << std::endl << std::endl;
  allocator.dump();
  buffer2 = allocator.alloc();
  if (buffer2 != NULL) {
    for (int i = 0; i < 8; i++) {
      buffer2[i] = '2';
    }
  }
  std::cout << std::endl << std::endl;
  allocator.dump();
  allocator.free(buffer2);
  allocator.free(buffer1);

  buffer1 = allocator.alloc();
  if (buffer1 != NULL) {
    for (int i = 0; i < 8; i++) {
      buffer1[i] = '1';
    }
  }
  std::cout << std::endl << std::endl;
  allocator.dump();
  return 0;
}
