#include <iostream>
#include "Deque.h"


int main(int argc, const char *argv[])
{
  BoundedDeque<int> test(5);

  std::cout << "# Push, pop tail _(i.e., FIFO)_ #" << std::endl;
  for (int i = 0; i < 10; i++) {
    test.push(i);
  }
  while (!test.empty()) {
    std::cout << "    " << test.pop_tail() << " (" << test.size() << ")"
              << std::endl;
  }
  std::cout << std::endl;

  std::cout << "# Push, pop head _(i.e., LIFO)_ #" << std::endl;
  for (int i = 0; i < 10; i++) {
    test.push(i);
  }
  while (!test.empty()) {
    std::cout << "    " << test.pop_head() << " (" << test.size() << ")"
              << std::endl;
  }
  std::cout << std::endl;

  std::cout << "# Append, pop head _(i.e., alternative FIFO)_ #" << std::endl;
  for (int i = 0; i < 10; i++) {
    test.append(i);
  }
  while (!test.empty()) {
    std::cout << "    " << test.pop_head() << " (" << test.size() << ")"
              << std::endl;
  }
  std::cout << std::endl;

  std::cout << "# Append, pop tail _(i.e., alternative LIFO)_ #" << std::endl;
  for (int i = 0; i < 10; i++) {
    test.append(i);
  }
  while (!test.empty()) {
    std::cout << "    " << test.pop_tail() << " (" << test.size() << ")"
              << std::endl;
  }
  return 0;
}
