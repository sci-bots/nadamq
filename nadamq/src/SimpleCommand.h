#ifndef ___SIMPLE_COMMAND__H___
#define ___SIMPLE_COMMAND__H___


#include "PacketWriter.h"

struct SimpleType {
  float a;
  uint32_t b;
  int16_t c;
};


class CommandProcessor {
  /* # `CommandProcessor` #
   *
   * Each call to this functor processes a single command.
   *
   * All arguments are passed by reference, such that they may be used to form
   * a response.  If the integer return value of the call is zero, the call is assumed to have
   * no response required.  Otherwise, the arguments contain must contain response values. */
public:
  static const uint8_t CMD_ECHO                   = 0x10;
  static const uint8_t CMD_SYSTEM__RAM_SIZE       = 0x11;
  static const uint8_t CMD_SYSTEM__RAM_DATA_SIZE  = 0x12;
  static const uint8_t CMD_SYSTEM__RAM_BSS_SIZE   = 0x13;
  static const uint8_t CMD_SYSTEM__RAM_HEAP_SIZE  = 0x14;
  static const uint8_t CMD_SYSTEM__RAM_STACK_SIZE = 0x15;
  static const uint8_t CMD_SYSTEM__RAM_FREE       = 0x16;
  static const uint8_t CMD_SYSTEM__SIMPLE_TYPE    = 0x17;

#ifdef AVR
  int operator () (uint8_t &command, uint16_t &count, uint8_t *data) {
    /* ## Call operator ##
     *
     * Arguments:
     *
     *  - `command`: Command code, designating the command to be executed.
     *  - `count`: The number of bytes in the arguments buffer.
     *  - `data`: The arguments buffer.
     *
     * __NB__ It is currently the responsibility */
    uint32_t *u32_result = reinterpret_cast<uint32_t *>(data);
    uint16_t *u16_result = reinterpret_cast<uint16_t *>(data);
    float *f32_result = reinterpret_cast<float *>(data);
    SimpleType *simple_type_result = reinterpret_cast<SimpleType *>(data);
    switch (command) {
      case CMD_ECHO:
        break;
      case CMD_SYSTEM__RAM_SIZE:
        u32_result[0] = ram_size();
        count = sizeof(u32_result[0]);
        break;
      case CMD_SYSTEM__RAM_DATA_SIZE:
        u32_result[0] = data_size();
        count = sizeof(u32_result[0]);
        break;
      case CMD_SYSTEM__RAM_BSS_SIZE:
        u32_result[0] = bss_size();
        count = sizeof(u32_result[0]);
        break;
      case CMD_SYSTEM__RAM_HEAP_SIZE:
        u32_result[0] = heap_size();
        count = sizeof(u32_result[0]);
        break;
      case CMD_SYSTEM__RAM_STACK_SIZE:
        u32_result[0] = stack_size();
        count = sizeof(u32_result[0]);
        break;
      case CMD_SYSTEM__RAM_FREE:
        u32_result[0] = free_memory();
        count = sizeof(u32_result[0]);
        break;
      case CMD_SYSTEM__SIMPLE_TYPE:
        simple_type_result[0].a = 1.234;
        simple_type_result[0].b = 87;
        simple_type_result[0].c = -9876;
        count = sizeof(simple_type_result[0]);
        break;
      default:
        break;
    }
    return 0;
  }
#endif  // #ifdef AVR
};


template <typename OStream, typename CommandProcessor>
class CommandPacketHandler {
  /* # `CommandPacketHandler` #
   *
   * This class extracts a command _(with a corresponding data buffer, if
   * applicable)_ from a `Packet`.  This is performed by having calling code
   * call the `process_packet` method, passing in the packet to be processed.
   *
   * __NB__ The packet state is intended to be updated by the `process_packet`
   * command to contain the response to send via the `OStream` interface.  For
   * example, a command may return a result value/structure to the source of
   * the request by writing data to the `packet.payload_buffer_`, provided the
   * data is no longer than `packet.buffer_size_`.
   *
   * If no response should be sent, the type of the packet must be set to
   * `Packet::packet_type::NONE`. */
  public:

  OStream &ostream_;
  CommandProcessor &command_processor_;

  CommandPacketHandler(OStream &ostream, CommandProcessor &command_processor)
    : ostream_(ostream), command_processor_(command_processor) {}

  template <typename Packet>
  void process_packet(Packet &packet) {
    /* # `process_packet` #
     *
     * For now, just echo the received packet. */
    uint16_t payload_bytes_to_process = packet.payload_length_;
    if (packet.type() == Packet::packet_type::DATA &&
        payload_bytes_to_process > 0) {
      uint8_t *proxy = packet.payload_buffer_;
      /* Interpret first byte of payload as command byte. */
      uint8_t &command = proxy[0];
      proxy++; payload_bytes_to_process--;
      /* Call `process_command` to execute handling code based on the command.
       * We pass `proxy`, which points to the start of the data for the
       * command.
       *
       * __NB__ It is currently the responsibility of `process_command` to
       * de-serialize any arguments from `proxy` as necessary. */
      command_processor_(command, payload_bytes_to_process, proxy);
      packet.payload_length_ = payload_bytes_to_process + 1;
    }
    write_packet(ostream_, packet);
  }
};


#endif  // #ifndef ___SIMPLE_COMMAND__H___
