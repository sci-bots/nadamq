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
  /* Create a buffer with `slot_count` slots. */
  explicit CircularBuffer(size_t slot_count);
  ~CircularBuffer();
  /* Attempt to push `value` to the next available slot.
   * Return `true` if value is written, `false` if buffer is full, and, thus,
   * no push occurred. */
  bool push(T const &value);
  /* Attempt to read the next value available for reading.
   * Return `true` if a value is read, `false` if buffer is empty, and, thus,
   * no read occurred. */
  bool pop(T &val);
  void reset();
  bool isEmpty() const { return (occupied_count_ == 0); }
  bool isFull() const { return (occupied_count_ == slot_count_); }
  int pop_index() const { return pop_index_; }
  int push_index() const { return push_index_; }
  size_t available() const { return occupied_count_; }
  size_t size() const { return slot_count_; }

private:
  //array of integers
  T* data_;
  // the size of the buffer
  int slot_count_;
  //index to pop the next integer from buffer
  int pop_index_;
  //index to push a new integer to buffer
  int push_index_;
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
bool CircularBuffer<T>::push(const T & value) {
  if (!isFull()) {
    data_[push_index_] = value;
    if (pop_index_ == -1) {
      //if buffer is empty, set the pop index to the
      //current push index. because that will be the first
      //slot to be pop later.
      pop_index_ = push_index_;
    }
    push_index_ = (push_index_ + 1) % slot_count_;
    occupied_count_++;
  }
}

template <class T>
bool CircularBuffer<T>::pop(T &val) {
  if (!isEmpty()) {  // if buffer is not empty
    val = data_[pop_index_];
    pop_index_ = (pop_index_ + 1) % slot_count_;
    occupied_count_--;
    if (isEmpty()) {
      /* All available data is pop, now buffer is empty. */
      pop_index_ = -1;
    }
  }
}

template <class T>
void CircularBuffer<T>::reset() {
  pop_index_ = -1; /* buffer empty */
  push_index_ = 0; /* first time writing to that buffer*/
  occupied_count_ = 0;
}

#endif  // MCP_CIRCULAR_BUFFER_GENERIC_H_
