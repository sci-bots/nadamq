#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>  // `std::ostream_iterator`
#include <iomanip>  // `std::setw`

#include "PacketSocket.h"
#include "Packet.h"
#include "PacketHandler.h"
#include "PacketSocketEvents.h"
#include "stream.hpp"


template <size_t Size>
struct DummyPacket {
  uint8_t type;
  uint8_t uid;
  uint8_t length;
  char message[Size];

  DummyPacket() : length(Size) {}
  explicit DummyPacket(uint8_t type) : type(type), length(Size) {}
};


int main(int argc, const char *argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <event characters>" << std::endl;
    return -1;
  }

  unsigned int running_packet_id;

  std::cout << "# Packet socket test #" << std::endl << std::endl;

  std::cout << std::setw(20) << "events: " << argv[1] << std::endl;

  Packet packet;
  PacketParser parser;
  parser.reset(&packet);

  typedef StreamWrapper<std::ifstream, 128> Stream;

  std::ifstream input(argv[1], std::ifstream::binary);
  if (input) {
    Stream wrapper(input);
    StreamPacketParser<PacketParser, Stream> stream_parser(parser, wrapper);
    StreamPacketSocket<StreamPacketParser<PacketParser, Stream> >
      socket(stream_parser, 128, 10, 10);
    socket.reset();

    std::cout << std::endl << "## State transitions ##" << std::endl;

    while (socket.available() > 0) {
      uint8_t event = socket.pop_event();

      std::stringstream out;
      out << "`" << event_label(event) << "`" << " [`" << event << "`]";
      std::cout << std::setw(28) << out.str() << " -> state: "
                << socket.state() << std::endl;
    }
  }
  return 0;
}
