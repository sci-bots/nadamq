#include <iostream>
#include "CircularBuffer.hpp"

using namespace std;

int main() {
  CircularBuffer<unsigned int> cb(15);
  unsigned int val;

  std::cout << "# Fill buffer with 10 elements #" << std::endl << std::endl;

  for (unsigned int i=1; i <= 10; i++) {
    if (cb.push(i)) {
      cout << "    " << i << " (occupancy: " << cb.available() << "/" << cb.size() << ")"
           << std::endl;
    } else {
      cout << "    Could not push " << i << " - buffer full." << std::endl;
    }
  }

  std::cout << std::endl << "# pop 5 elements #" << std::endl << std::endl;

  for (unsigned int i=1; i <= 5; i++) {
    cb.pop(val);
    cout << "    " << val << " (occupancy: " << cb.available() << "/" << cb.size() << ")"
         << std::endl;
  }

  std::cout << std::endl << "# Fill buffer with 10 elements #" << std::endl << std::endl;

  for (unsigned int i=10; i <= 23; i++) {
    if (cb.push(i)) {
      cout << "    " << i << " (occupancy: " << cb.available() << "/" << cb.size() << ")"
           << std::endl;
    } else {
      cout << "    Could not push " << i << " - buffer full." << std::endl;
    }
  }

  std::cout << std::endl << "# pop buffer #" << std::endl << std::endl;
  while(!cb.isEmpty()) {
    cb.pop(val);
    cout << "    " << val << " (occupancy: " << cb.available() << "/" << cb.size() << ")"
         << std::endl;
  }

  unsigned int i = 100;
  while(!cb.isFull()) {
    cb.push(i);
    cout << "    " << i << " (occupancy: " << cb.available() << "/" << cb.size() << ")"
         << std::endl;
    i--;
  }

  return 0;
}
