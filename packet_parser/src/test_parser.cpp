#include <iostream>
#include <fstream>
#include <iomanip>  // `std::setw`

#include "PacketParser.hpp"
#include "../../../stream_parser/src/stream.hpp"


int main(int argc, const char *argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <packet file>" << std::endl;
    return -1;
  }

  std::cout << "# Packet parse test #" << std::endl << std::endl;

  std::cout << std::setw(24) << "packet file: " << argv[1] << std::endl;

  Packet packet;
  //PacketParser parser;
  Parser parser;
  parser.reset();

  std::ifstream input(argv[1], std::ifstream::binary);
  if (input) {
    StreamWrapper<std::ifstream, 128> wrapper(input);
    size_t byte_count = wrapper.available();

    std::cout << std::setw(24) << "bytes available: " << byte_count
              << std::endl;
    while (wrapper.available() > 0) {
      uint8_t byte = wrapper.read();
      parser.parse_byte(&byte);
      std::cout << std::setw(15) << "parse: ";
      if (byte >= 32 && byte < 127) {
        std::cout << "'" << byte << "'";
      } else {
        std::cout << std::hex << (0x00FF & static_cast<int>(byte));
      }
      std::cout << " -> " << parser.parse_error_ << ", " << parser.state()
                << std::endl;
    }
    std::cout << std::setw(15) << parser.message_completed_ << std::endl;
  }
  return 0;
}
