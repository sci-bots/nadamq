#ifndef ___STREAM_PACKET_PARSER__H___
#define ___STREAM_PACKET_PARSER__H___


template <typename Parser, typename Stream>
class StreamPacketParser {
  /* # `StreamPacketParser` #
   *
   * Provide the following API described below to read full packets from a
   * stream.
   *
   * ## Accessors ##
   *
   *  - `available()`: `true` if there is data ready on the stream.
   *  - `ready()`: `true` if a packet has been parsed successfully
   *    _(accessible through the `packet()` method)_.
   *  - `error()`: Returns error code if there was an error parsing the
   *    current packet.
   *   - `L`: Data too large for buffer.
   *   - `e`: Parse error, e.g., failed checksum.
   *  - `packet(Packet &output)`: Copy current packet contents to `output`.
   *
   * ## Mutators ##
   *
   *  - `read_available()`: Parse available characters from the stream.  Stop
   *    parsing if either a full packet has been parsed, or if an error is
   *    encountered.
   *  - `reset()`: Reset state of parser.  __NB__ After parsing a complete
   *    packet or encountering an error, this method must be called before
   *    `parse_available()` will parse any new characters. */
public:
  /* Infer type of packet from `Parser` type. */
  typedef typename Parser::packet_type packet_type;

  Parser &parser_;
  Stream &stream_;

  StreamPacketParser(Parser &parser, Stream &stream)
    : parser_(parser), stream_(stream) {}

  virtual bool ready() { return parser_.message_completed_; }
  virtual uint8_t error() {
    if (parser_.parse_error_) {
      return 'e';
    }
    return 0;
  }
  virtual void reset() { parser_.reset(); }
  virtual void reset(packet_type *packet) { parser_.reset(packet); }
  virtual packet_type const &packet() const { return *parser_.packet_; }

  virtual int available() { return stream_.available(); }

  virtual int parse_available() {
    /* We have encountered an error, or we have parsed a full packet
     * successfully.  The state of the parser must be explicitly reset using
     * the `reset()` method before we continue parsing. */
    if (ready() || error()) { return 0; }

    int bytes_read = 0;
    while (stream_.available() > 0) {
      uint8_t byte = stream_.read();
      parser_.parse_byte(&byte);
      bytes_read++;
      /* Continue parsing until we have encountered an error, or we have parsed
       * a full packet successfully. */
      if (ready() || error()) { break; }
    }
    return bytes_read;
  }
};


#endif  // #ifndef ___STREAM_PACKET_PARSER__H___
