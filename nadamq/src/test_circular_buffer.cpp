#include <iostream>
#include "CircularBuffer.hpp"

using namespace std;

int main() {
  CircularBuffer<unsigned int> cb(15);
  unsigned int val;

  std::cout << "# Fill buffer with 10 elements #" << std::endl << std::endl;

  for (unsigned int i=1; i <= 10; i++) {
    if (cb.write(i)) {
      cout << "    " << i << ", r" << cb.read_index() << ", w"
           << cb.write_index() << endl;
    } else {
      cout << "    Could not write " << i << " - buffer full." << std::endl;
    }
  }

  std::cout << std::endl << "# Read 5 elements #" << std::endl << std::endl;

  for (unsigned int i=1; i <= 5; i++) {
    cb.read(&val);
    cout << "    " << val << ", r" << cb.read_index() << ", w" << cb.write_index() << endl;
  }

  std::cout << std::endl << "# Fill buffer with 10 elements #" << std::endl << std::endl;

  for (unsigned int i=10; i <= 23; i++) {
    if (cb.write(i)) {
      cout << "    " << i << ", r" << cb.read_index() << ", w"
           << cb.write_index() << endl;
    } else {
      cout << "    Could not write " << i << " - buffer full." << std::endl;
    }
  }

  std::cout << std::endl << "# Read buffer #" << std::endl << std::endl;
  while(!cb.isEmpty()) {
    if (!cb.read(&val)) {
      cout << "    " << val << ", r" << cb.read_index() << ", w" << cb.write_index() << endl;
    } else {
      cout << "    Could not read - buffer empty." << std::endl;
    }
  }
  return 0;
}
