#include <iostream>
#include <iterator>
#include <fstream>
#include <iterator>  // `std::ostream_iterator`
#include <iomanip>  // `std::setw`

#include "PacketHandler.h"
#include "Packet.h"
#include "PacketAllocator.h"
#include "stream.hpp"
#include "PacketStream.h"


template <typename Parser, typename Stream, typename PacketStream>
class PacketStreamHandler : public PacketHandlerBase<Parser, Stream> {
  /* # `PacketStreamHandler` #
   *
   * This class implements packet parsing reactor, which pushes each
   * successfully parsed packet onto a `PacketStream` instance. */
  public:

  typedef PacketHandlerBase<Parser, Stream> base_type;
  typedef typename base_type::packet_type packet_type;
  typedef Parser parser_type;
  typedef Stream input_stream_type;
  typedef PacketStream packet_stream_type;

  using base_type::parser_;

  PacketStream &packet_stream_;

  PacketStreamHandler(Parser &parser, Stream &input_stream,
                      PacketStream &packet_stream)
    : base_type(parser, input_stream), packet_stream_(packet_stream) {}

  virtual void handle_packet(packet_type &packet) {
    std::cout << std::endl << "# Packet parsed successfully #" << std::endl;
    std::cout << std::setw(24) << "iuid: " << std::dec
              << static_cast<int>(0x00FFFF & packet.iuid_) << std::endl;
    std::cout << std::setw(24) << "type: " << std::hex
              << static_cast<int>(0x00FF & packet.type()) << std::endl;
    std::cout << std::setw(24) << "payload length: "
              << packet.payload_length_ << std::endl;
    std::cout << std::setw(24) << "payload: " << "'";
    std::copy(packet.payload_buffer_, packet.payload_buffer_ +
              packet.payload_length_,
              std::ostream_iterator<uint8_t>(std::cout, ""));
    std::cout << "'" << std::endl;
    packet_type queue_packet = packet_stream_.allocator()->create_packet();
    uint8_t *buffer = queue_packet.payload_buffer_;
    queue_packet = packet;
    queue_packet.payload_buffer_ = buffer;
    std::copy(packet.payload_buffer_, packet.payload_buffer_ +
              packet.payload_length_, queue_packet.payload_buffer_);
    std::cout << std::endl << "## Queue packet into packet stream ##"
              << std::endl << std::endl;
    packet_stream_.push(queue_packet);
    std::cout << " - Complete" << std::endl;
  }

  virtual void handle_error(packet_type &packet) {
    std::cout << "# ERROR: Parse error #" << std::endl;
  }
};


int main(int argc, const char *argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <packet file>" << std::endl;
    return -1;
  }

  std::cout << "# Packet stream test #" << std::endl << std::endl;

  std::cout << std::setw(24) << "packet file: " << argv[1] << std::endl;
  std::cout << std::boolalpha;

  typedef FixedPacket packet_type;
  PacketAllocator<packet_type> packet_allocator;
  PacketStream<PacketAllocator<packet_type> > packet_stream(&packet_allocator);
  packet_type packet = packet_allocator.create_packet();

  PacketParser<packet_type> parser;
  parser.reset(&packet);

  typedef StreamWrapper<std::ifstream, 1024> Stream;

  std::ifstream input(argv[1], std::ifstream::binary);
  if (input) {
    Stream wrapper(input);
    PacketStreamHandler<PacketParser<packet_type>, Stream,
                        PacketStream<PacketAllocator<packet_type> > >
      stream_handler(parser, wrapper, packet_stream);
    stream_handler.parse_available();

    std::cout << "# Read stream of payload bytes from packet stream #"
              << std::endl << std::endl;
    std::cout << "    ";
    while (packet_stream.available() > 0) {
      int value = packet_stream.read();
      if (value >= 0) {
        std::cout << static_cast<char>(value);
      }
    }
    std::cout << std::endl << std::endl << "## End of stream ##" << std::endl
              << std::endl;

    /*  - Rewind input file to try again. */
    input.clear();
    input.seekg(0);

    stream_handler.parse_available();
    std::cout << "# Read stream of payload bytes from packet stream #"
              << std::endl << std::endl;
    std::cout << "    ";
    while (packet_stream.available() > 0) {
      int value = packet_stream.read();
      if (value >= 0) {
        std::cout << static_cast<char>(value);
      }
    }
    std::cout << std::endl << std::endl << "## End of stream ##" << std::endl
              << std::endl;
  }
  return 0;
}
