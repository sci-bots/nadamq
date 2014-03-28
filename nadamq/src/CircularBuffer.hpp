/* [Circular buffer][1] implementation written by [Mahir Atmis][2].
 *
 * [1]: http://en.wikipedia.org/wiki/Circular_buffer
 * [2]: http://atmismahir.blogspot.com.tr/2014/03/generic-circular-buffer.html */

#ifndef MCP_CIRCULAR_BUFFER_GENERIC_H_
#define MCP_CIRCULAR_BUFFER_GENERIC_H_

template<class T>
class CircularBuffer {
public:
  // Creates a buffer with 'slots' slots.
  explicit CircularBuffer(int slots);
  // Destructor.
  ~CircularBuffer();
  // Writes 'value' to the next available slot. It may overwrite
  // values that were not yet read out of the buffer.
  void write(const T & value);
  // Returns the next value available for reading, in the order they
  // were written, and marks slot as read. If the buffer is empty returns -1.
  void read(T* val);
  // Removes all the elements from the buffer.
  void clear();
  // returns true if the Circular buffer is empty, false otherwise
  bool isEmpty();

private:
  //array of integers
  T* data_;
  // the size of the buffer
  int  num_of_slots_;
  //index to read the next integer from buffer
  int  read_index_;
  //index to write a new integer to buffer
  int  write_index_;
  // Non-copyable, non-assignable.
  CircularBuffer(CircularBuffer&);
  CircularBuffer& operator=(const CircularBuffer&);
};

template <class T>
CircularBuffer<T>::CircularBuffer(int slots) {
  if (slots <= 0) {
    num_of_slots_ = 10; /*pre-assigned value */
  } else {
    num_of_slots_ = slots;
  }
  clear();
}

template <class T>
CircularBuffer<T>::~CircularBuffer() {
  delete data_;
}

template <class T>
void CircularBuffer<T>::write(const T & value) {
  data_[write_index_] = value;
  if (read_index_ == -1) {
    //if buffer is empty, set the read index to the
    //current write index. because that will be the first
    //slot to be read later.
    read_index_ = write_index_;
  }
  write_index_ = (write_index_ + 1) % num_of_slots_;
}

template <class T>
void CircularBuffer<T>::read(T* val) {
  if (read_index_ != -1) {  // if buffer is not empty
    *val = data_[read_index_];
    read_index_ = (read_index_ + 1) % num_of_slots_;
    if (read_index_ == write_index_) {
      /*all available data is read, now buffer is empty*/
      read_index_ = -1;
    }
  }
}

template <class T>
void CircularBuffer<T>::clear() {
  read_index_ = -1; /* buffer empty */
  write_index_ = 0; /* first time writing to that buffer*/
  data_ = new T[num_of_slots_]; /* allocate space for buffer */
}

template <class T>
bool CircularBuffer<T>::isEmpty() {
  return read_index_ == -1;
}

#endif  // MCP_CIRCULAR_BUFFER_GENERIC_H_
