#ifndef ___PACKET_HANDLER__HPP___
#define ___PACKET_HANDLER__HPP___

#ifndef AVR
/* Assume STL libraries are not available on AVR devices, so don't include
 * methods using them when targeting AVR architectures. */
#include <iostream>
#else
#include "output_buffer.h"
#endif

#include "PacketParser.h"


template <typename Parser, typename Stream>
class PacketHandlerBase {
  public:

  /* Infer type of packet from `Parser` type. */
  typedef typename Parser::packet_type packet_type;

  Parser &parser_;
  Stream &stream_;

  PacketHandlerBase(Parser &parser, Stream &stream)
    : parser_(parser), stream_(stream) {}

  /* No default behaviour for handling a successfully parse packet.  This
   * method must be implemented in a concrete sub-class. */
  virtual void handle_packet(packet_type &packet) = 0;
  /* Enable special handling of parse errors. */
  virtual void handle_error(packet_type &packet) {}

  void parse_available() {
    while (stream_.available() > 0) {
      uint8_t byte = stream_.read();

      parser_.parse_byte(&byte);

      if(parser_.parse_error_) {
        /* Parse error */
        handle_error(*(parser_.packet_));
        parser_.reset();
      } else
#ifdef CRC_VERIFY
      if (parser_.message_completed_ &&
          (!parser_.packet_->has_crc_ || parser_.verified())) {
#else
      if (parser_.message_completed_) {
#endif  // #ifdef CRC_VERIFY / #else
        /* # Process packet #
         *
         * Do something with successfully parsed packet.
         *
         * Note that the packet should be copied if it will be needed
         * afterwards, since the packet will be reset below, before the next
         * input scan. */
        handle_packet(*(parser_.packet_));
        /* Reset parser to process next packet. */
        parser_.reset();
      }
    }
  }
};


#ifdef AVR
template <typename Parser, typename IStream, typename OStream>
class VerbosePacketHandler : public PacketHandlerBase<Parser, IStream> {
  public:

  typedef PacketHandlerBase<Parser, IStream> base_type;
  typedef typename base_type::packet_type packet_type;

  using base_type::parser_;

  OStream &ostream_;

  VerbosePacketHandler(Parser &parser, IStream &istream, OStream &ostream)
    : base_type(parser, istream), ostream_(ostream) {}

  virtual void handle_packet(packet_type &packet) {
    ostream_.println(P("# success #"));
    ostream_.println("");
    ostream_.print("type: ");
    ostream_.println(packet.type_);
    ostream_.print("command: ");
    ostream_.println(static_cast<int>(0x0FF & packet.command()));
    if (packet.has_crc_) {
      ostream_.print("CRC ok: ");
      if (parser_.verified()) {
        ostream_.println("T");
      } else {
        ostream_.println("F");
      }
    }
    ostream_.print("payload length: ");
    ostream_.println(packet.payload_length_);
    ostream_.print("payload: '");
    for (int i = 0; i < packet.payload_length_; i++) {
      ostream_.print((char)packet.payload_buffer_[i]);
    }
    ostream_.println("'");
    ostream_.println("");
  }

  virtual void handle_error(packet_type &packet) {
    ostream_.println("# ERROR #");
  }
};
#else
template <typename Parser, typename Stream>
class VerbosePacketHandler : public PacketHandlerBase<Parser, Stream> {
  public:

  typedef PacketHandlerBase<Parser, Stream> base_type;
  typedef typename base_type::packet_type packet_type;

  using base_type::parser_;

  VerbosePacketHandler(Parser &parser, Stream &stream)
    : base_type(parser, stream) {}

  virtual void handle_packet(packet_type &packet) {
    std::cout << "# Packet parsed successfully #" << std::endl;
    std::cout << std::setw(24) << "command: " << std::hex
              << static_cast<int>(0x00FF & packet.command()) << std::endl;
    if (packet.has_crc_) {
      std::cout << std::setw(24) << "CRC ok: " << parser_.verified()
                << std::endl;
    }
    std::cout << std::setw(24) << "payload length: "
              << packet.payload_length_ << std::endl;
    std::cout << std::setw(24) << "payload: " << "'";
    std::copy(packet.payload_buffer_, packet.payload_buffer_ +
              packet.payload_length_,
              std::ostream_iterator<uint8_t>(std::cout, ""));
    std::cout << "'" << std::endl;
  }

  virtual void handle_error(packet_type &packet) {
    std::cout << "# ERROR: Parse error #" << std::endl;
  }
};
#endif  // #ifdef AVR


#endif  // #ifndef ___PACKET_HANDLER__HPP___
