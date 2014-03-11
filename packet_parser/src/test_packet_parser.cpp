#include <iostream>
#include <fstream>
#include <iterator>  // `std::ostream_iterator`
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
  std::cout << std::boolalpha;

  Packet packet;
  PacketParser parser;
  parser.reset(&packet);

  std::ifstream input(argv[1], std::ifstream::binary);
  if (input) {
    StreamWrapper<std::ifstream, 128> wrapper(input);
    size_t byte_count = wrapper.available();

    std::cout << std::setw(24) << "bytes available: " << byte_count
              << std::endl;
    while (wrapper.available() > 0) {
      uint8_t byte = wrapper.read();
#ifdef VERBOSE_PARSE_CHARACTER
      std::cout << std::setw(15) << "parse: " << "["
                << static_cast<int>(parser.state()) << "]";
      if (byte >= 32 && byte < 127) {
        std::cout << "'" << byte << "'";
      } else {
        std::cout << std::hex << (0x00FF & static_cast<int>(byte));
      }
      std::cout << std::endl;
#endif  // #ifdef VERBOSE_PARSE_CHARACTER

      parser.parse_byte(&byte);

      if (parser.parse_error_) {
        std::cerr << std::endl << "## Parse error ##" << std::endl
                  << std::endl << std::setw(15) << "state: " << parser.state()
                  << std::endl;

        std::cerr << std::endl << "## Reset parser ##" << std::endl << std::endl;
        parser.reset(&packet);
      } else if (parser.message_completed_) {
        std::cout << std::endl << "## Packet parsed successfully ##" << std::endl
                  << std::endl;
        std::cout << std::setw(24) << "command: " << std::hex
                  << static_cast<int>(0x00FF & packet.command()) << std::endl;
        if (packet.has_crc_) {
          std::cout << std::setw(24) << "CRC ok: " << parser.verified()
                    << std::endl;
        }
        std::cout << std::setw(24) << "payload length: "
                  << packet.payload_length_ << std::endl;
        std::cout << std::setw(24) << "payload: " << "'";
        std::copy(packet.payload_buffer_, packet.payload_buffer_ +
                  packet.payload_length_,
                  std::ostream_iterator<uint8_t>(std::cout, ""));
        std::cout << "'" << std::endl;
        /* Reset parser to process next packet. */
        parser.reset(&packet);
      }
    }
  }
  return 0;
}
