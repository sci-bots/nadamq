#ifndef ___PACKET_WRITER__H___
#define ___PACKET_WRITER__H___

#ifndef AVR
/* Assume STL libraries are not available on AVR devices, so don't include
 * methods using them when targeting AVR architectures. */
#include <iostream>
using namespace std;
#endif // ifndef AVR
#include <stdint.h>
#include <stdlib.h>

#include "Packet.h"


template <typename Stream, typename T>
void serialize_any(Stream &output, T const &value) {
  const char* proxy = reinterpret_cast<const char *>(&value);
  /* Arduino and Intel x86 processors store [low byte at lower address][1]
   * _(i.e., little-endian)_, so we must traverse the bytes in reverse order
   * when serializing to [network-byte-order][2].
   *
   * >     test.c:
   * >     static int order __attribute__((unused)) = 0xAAFF;
   * >
   * >     avr-gcc test.c -c
   * >     avr-objdump -D test.o
   * >     test.o:     file format elf32-avr
   * >
   * >     Disassembly of section .data:
   * >
   * >     00000000 <order>:
   * >        0:   ff aa           std     Y+55, r15       ; 0x37
   *
   * [1]: https://diigo.com/01r4yj
   * [2]: http://en.wikipedia.org/wiki/Endianness#Endianness_in_networking */
  for (int i = sizeof(T) - 1; i >= 0; i--) {
    output.write(&proxy[i], 1);
  }
}


template <typename Packet>
inline void write_packet(std::ostream &output, Packet const &packet) {
  Packet to_send = packet;
  /* Set the CRC checksum of the packet based on the contents of the payload.
   * */
  to_send.compute_crc();

  char startflag[] = "|||";
  output.write(startflag, 3);
  serialize_any(output, to_send.iuid_);
  uint8_t type_ = static_cast<uint8_t>(to_send.type());
  serialize_any(output, type_);
  if (type_ == Packet::packet_type::DATA) {
    serialize_any(output, static_cast<uint8_t>(to_send.payload_length_));
    if (to_send.payload_length_ > 0) {
      output.write((char*)to_send.payload_buffer_, to_send.payload_length_);
    }
    serialize_any(output, to_send.crc_);
  }
}


template <typename Packet>
class PacketWriter {
public:
  typedef Packet packet_type;

  PacketWriter() {}

  /* Write the provided packet data based on the following packet [ABNF][1]
   * grammar:
   *
   *     packet = startflag iuid type length payload crc
   *     startflag = 3%x7C
   *     iuid = 2OCTET
   *     type = OCTET
   *     length = OCTET
   *     payload = *OCTET
   *     crc = 2OCTET
   *
   * [1]: http://en.wikipedia.org/wiki/Augmented_Backus%E2%80%93Naur_Form
   */
  virtual void write(packet_type const &packet) = 0;
};


template <typename Packet, typename Stream>
class StreamPacketWriter : public PacketWriter<Packet> {
public:
  typedef PacketWriter<Packet> base_type;
  typedef typename base_type::packet_type packet_type;

  Stream &output_;

#if 0
  StreamPacketWriter(Stream &output) : output_(output) {}
  virtual void write(packet_type const &packet) {
    packet_type to_send = packet;
    /* Set the CRC checksum of the packet based on the contents of the payload.
     * */
    to_send.compute_crc();
    output_.write("~~~");
    output_.write((char*)&(to_send.iuid_), 2);
    uint8_t type_ = static_cast<uint8_t>to_send.type();
    output_.write(type_);
    output_.write(static_cast<uint8_t>(to_send.payload_length_));
    if (to_send.payload_length_ > 0) {
      output_.write((char*)to_send.payload_buffer_, to_send.payload_length_);
    }
    output_.write((char*)&(to_send.crc_), 2);
  }
#endif
};

template <typename Packet, typename Stream>
class OStreamPacketWriter : public PacketWriter<Packet> {
public:
  typedef PacketWriter<Packet> base_type;
  typedef typename base_type::packet_type packet_type;

  Stream &output_;

  OStreamPacketWriter(Stream &output) : output_(output) {}

  virtual void write(packet_type const &packet) {
    write_packet(output_, packet);
  }
};


#endif  // #ifndef ___PACKET_WRITER__H___
