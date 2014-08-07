#include <iostream>
#include <fstream>
#include <iterator>  // `std::ostream_iterator`
#include <iomanip>  // `std::setw`

#include "PacketHandler.h"
#include "Packet.h"
#include "PacketAllocator.h"
#include "stream.hpp"


int main(int argc, const char *argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <packet file>" << std::endl;
    return -1;
  }

  std::cout << "# Packet parse test #" << std::endl << std::endl;

  std::cout << std::setw(24) << "packet file: " << argv[1] << std::endl;
  std::cout << std::boolalpha;

  typedef FixedPacket packet_type;
  PacketAllocator<packet_type> packet_allocator;
  packet_type packet = packet_allocator.create_packet();

  PacketParser<packet_type> parser;
  parser.reset(&packet);

  typedef StreamWrapper<std::ifstream, 128> Stream;

  std::ifstream input(argv[1], std::ifstream::binary);
  if (input) {
    Stream wrapper(input);
    VerbosePacketHandler<PacketParser<packet_type>, Stream> handler(parser,
                                                                    wrapper);
    handler.parse_available();
  }
  return 0;
}
