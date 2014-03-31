#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>  // `std::ostream_iterator`
#include <iomanip>  // `std::setw`

#include "PacketSocket.h"
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

  typedef DummyPacket<32> Packet;
  SerialPacketSocket<Packet> socket(128, 10, 10);
  socket.reset();

  socket.push_rx_packet(Packet('d'));
  socket.push_rx_packet(Packet('a'));
  socket.push_rx_packet(Packet('n'));
  socket.push_rx_packet(Packet('d'));
  socket.push_rx_packet(Packet('d'));
  socket.push_rx_packet(Packet('d'));

  //for (const char *event = argv[1]; *event != '\0'; event++) {
    //socket.push_event(*event);
  //}

  std::cout << std::endl << "## State transitions ##" << std::endl;

  while (socket.available() > 0) {
    uint8_t event = socket.pop_event();

    std::stringstream out;
    out << "`" << event_label(event) << "`" << " [`" << event << "`]";
    std::cout << std::setw(28) << out.str() << " -> state: "
              << socket.state() << std::endl;
  }
  return 0;
}
