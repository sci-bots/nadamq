/* [Circular buffer][1] implementation written by [Mahir Atmis][2].
 *
 * [1]: http://en.wikipedia.org/wiki/Circular_buffer
 * [2]: http://atmismahir.blogspot.com.tr/2014/03/generic-circular-buffer.html */

#ifndef MCP_CIRCULAR_BUFFER_GENERIC_H_
#define MCP_CIRCULAR_BUFFER_GENERIC_H_

#include <stdlib.h>
#include <iostream>


template<class T>
class CircularBuffer {
public:
  // Creates a buffer with 'slots' slots.
  explicit CircularBuffer(size_t slot_count);
  // Destructor.
  ~CircularBuffer();
  // Writes 'value' to the next available slot. It may overwrite
  // values that were not yet read out of the buffer.
  bool write(const T & value);
  // Returns the next value available for reading, in the order they
  // were written, and marks slot as read. If the buffer is empty returns -1.
  bool read(T* val);
  void reset();
  // returns true if the Circular buffer is empty, false otherwise
  bool isEmpty() const;
  bool isFull() const;
  int read_index() const { return read_index_; }
  int write_index() const { return write_index_; }

private:
  //array of integers
  T* data_;
  // the size of the buffer
  int slot_count_;
  //index to read the next integer from buffer
  int read_index_;
  //index to write a new integer to buffer
  int write_index_;
  // the size of the buffer
  int occupied_count_;
  // Non-copyable, non-assignable.
  CircularBuffer(CircularBuffer&);
  CircularBuffer& operator=(const CircularBuffer&);
};

template <class T>
CircularBuffer<T>::CircularBuffer(size_t slot_count)
    : slot_count_(slot_count) {
  data_ = (T *)calloc(slot_count, sizeof(T));
  reset();
}

template <class T>
CircularBuffer<T>::~CircularBuffer() {
  free(data_);
}

template <class T>
bool CircularBuffer<T>::write(const T & value) {
  if (!isFull()) {
    data_[write_index_] = value;
    if (read_index_ == -1) {
      //if buffer is empty, set the read index to the
      //current write index. because that will be the first
      //slot to be read later.
      read_index_ = write_index_;
    }
    write_index_ = (write_index_ + 1) % slot_count_;
    occupied_count_++;
  }
}

template <class T>
bool CircularBuffer<T>::read(T* val) {
  if (!isEmpty()) {  // if buffer is not empty
    *val = data_[read_index_];
    read_index_ = (read_index_ + 1) % slot_count_;
    occupied_count_--;
    if (isEmpty()) {
      /* All available data is read, now buffer is empty. */
      read_index_ = -1;
    }
  }
}

template <class T>
void CircularBuffer<T>::reset() {
  read_index_ = -1; /* buffer empty */
  write_index_ = 0; /* first time writing to that buffer*/
  occupied_count_ = 0;
}

template <class T>
bool CircularBuffer<T>::isEmpty() const {
  return (occupied_count_ == 0);
}

template <class T>
bool CircularBuffer<T>::isFull() const {
  return (occupied_count_ == slot_count_);
}

#endif  // MCP_CIRCULAR_BUFFER_GENERIC_H_
