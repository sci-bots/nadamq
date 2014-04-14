#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>  // `std::ostream_iterator`
#include <iomanip>  // `std::setw`

#include "PacketSocket.h"
#include "Packet.h"
#include "PacketAllocator.h"
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

  typedef FixedPacket packet_type;
  typedef StreamWrapper<std::ifstream, 128> Stream;
  typedef PacketParser<packet_type> packet_parser_type;
  typedef StreamPacketParser<packet_parser_type, Stream> stream_parser_type;
  typedef PacketAllocator<packet_type, FixedSizeBufferPool<128, 1> > allocator_type;
  typedef StreamPacketSocket<stream_parser_type, allocator_type> socket_type;

  allocator_type packet_allocator;
  packet_parser_type parser;

  std::ifstream input(argv[1], std::ifstream::binary);
  if (input) {
    Stream wrapper(input);
    stream_parser_type stream_parser(parser, wrapper);
    socket_type socket(stream_parser, &packet_allocator, 128, 10, 10);
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
