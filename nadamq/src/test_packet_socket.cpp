#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>  // `std::ostream_iterator`
#include <iomanip>  // `std::setw`

#include "PacketSocket.h"
#include "PacketSocketEvents.h"
#include "stream.hpp"


int main(int argc, const char *argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <event characters>" << std::endl;
    return -1;
  }

  std::cout << "# Packet socket test #" << std::endl << std::endl;

  std::cout << std::setw(20) << "events: " << argv[1] << std::endl;

  PacketSocket socket(128);
  socket.reset();

  std::cout << std::endl << "## State transitions ##" << std::endl;
  for (const char *event = argv[1]; *event != '\0'; event++) {
    uint8_t byte = *event;

    socket.parse_byte(&byte);

    std::stringstream out;
    out << "`" << event_label(byte) << "`" << " [`" << byte << "`]";
    std::cout << std::setw(28) << out.str() << " -> state: "
              << socket.state() << std::endl;
  }
  return 0;
}
