#ifndef ___DEQUE__H___
#define ___DEQUE__H___

#include <stdlib.h>

template <typename T>
class DequeNode {
public:
  DequeNode<T> *prev;
  DequeNode<T> *next;
  T value;

  DequeNode(T value) : value(value), prev(NULL), next(NULL) {}
};


template <typename T>
class Deque {
protected:
  DequeNode<T> *head_;
  DequeNode<T> *tail_;
  size_t item_count_;
public:
  Deque() : head_(NULL), tail_(NULL), item_count_(0) {}
  void append(T item);
  void push(T item);
  T pop_tail();
  T pop_head();
  T &head() const { return head_->value; }
  T &tail() const { return tail_->value; }
  bool empty() const { return (item_count_ == 0); }
  size_t size() const { return item_count_; }
};


template <typename T>
void Deque<T>::append(T item) {
  DequeNode<T> *node = new DequeNode<T>(item);
  if (empty()) {
    head_ = node;
    tail_ = node;
  } else {
    tail_->next = node;
    node->prev = tail_;
    tail_ = node;
  }
  item_count_++;
}


template <typename T>
void Deque<T>::push(T item) {
  DequeNode<T> *node = new DequeNode<T>(item);
  if (empty()) {
    head_ = node;
    tail_ = node;
  } else {
    head_->prev = node;
    node->next = head_;
    head_ = node;
  }
  item_count_++;
}


template <typename T>
T Deque<T>::pop_tail() {
  item_count_--;
  DequeNode<T> *node = tail_;
  if (!empty()) {
    tail_ = node->prev;
    tail_->next = NULL;
  }
  return node->value;
}


template <typename T>
T Deque<T>::pop_head() {
  item_count_--;
  DequeNode<T> *node = head_;
  if (!empty()) {
    head_ = node->next;
    head_->prev = NULL;
  }
  return node->value;
}


template <typename T>
class BoundedDeque : public Deque<T> {
public:
  typedef Deque<T> base_type;
protected:
  size_t max_size_;
  using base_type::item_count_;
public:
  BoundedDeque(size_t max_size) : base_type(), max_size_(max_size) {}
  bool append(T item) {
    if (!full()) {
      base_type::append(item);
      return true;
    } else {
      return false;
    }
  }
  bool push(T item) {
    if (!full()) {
      base_type::push(item);
      return true;
    } else {
      return false;
    }
  }
  bool full() const { return (item_count_ == max_size_); }
};


#endif  // #ifndef ___DEQUE__H___
