#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include "PacketWriter.h"


int main(int argc, const char *argv[]) {
  if (argc < 4 || argc > 5) {
    std::cerr << "usage: " << argv[0] << "<output file> <packet iuid> [dna] "
              << "[payload for d]" << std::endl;
    return -1;
  }
  uint16_t iuid = atoi(argv[2]);
  uint8_t type = argv[3][0];

  std::ofstream outfile(argv[1], std::ofstream::binary);
  OStreamPacketWriter<Packet, std::ostream> packet_writer(outfile);

  Packet test_packet;
  test_packet.iuid_ = iuid;
  test_packet.type(type);
  if (type == 'd' && argc > 4) {
    size_t len = strlen(argv[4]);
    test_packet.payload_buffer_ = (uint8_t *)calloc(len + 1, sizeof(uint8_t));
    test_packet.buffer_size_ = len + 1;
    test_packet.payload_length_ = len;
    strcpy(reinterpret_cast<char *>(test_packet.payload_buffer_), argv[4]);
  }
  packet_writer.write(test_packet);
  if (test_packet.payload_buffer_ != NULL) {
    free(test_packet.payload_buffer_);
  }

  return 0;
}
