#include "Memory.h"
#include "PacketParser.h"
#include "SimpleCommand.h"
#include "packet_handler.h"


uint8_t protobuf[128];
#define PACKET_SIZE   64
uint8_t packet_buffer[PACKET_SIZE];
char output_buffer[128];

typedef CommandPacketHandler<Stream, CommandProcessor> Handler;
typedef PacketReactor<PacketParser<FixedPacket>, Stream, Handler> Reactor;

CommandProcessor command_processor;
FixedPacket packet;
/* `reactor` maintains parse state for a packet, and updates state one-byte
 * at-a-time. */
PacketParser<FixedPacket> parser;
/* `handler` processes complete packets and sends response as necessary. */
Handler handler(Serial, command_processor);
/* `reactor` uses `parser` to parse packets from input stream and passes
 * complete packets to `handler` for processing. */
Reactor reactor(parser, Serial, handler);

void setup() {
  packet.reset_buffer(PACKET_SIZE, &packet_buffer[0]);
  parser.reset(&packet);
  Serial.begin(115200);
  Serial.print("# ready (");
  Serial.print(packet.buffer_size_);
  Serial.println(") #");
  Serial.print("ram_size="); Serial.println(ram_size());
  Serial.print("data_size="); Serial.println(data_size());
  Serial.print("bss_size="); Serial.println(bss_size());
  Serial.print("heap_size="); Serial.println(heap_size());
  Serial.print("stack_size="); Serial.println(stack_size());
  Serial.print("free_memory="); Serial.println(free_memory());
}

void loop() {
  reactor.parse_available();
  delay(50);
}
