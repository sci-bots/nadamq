#ifndef ___SIMPLE_COMMAND__H___
#define ___SIMPLE_COMMAND__H___


#include "PacketWriter.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "simple.pb.h"


class CommandProcessor {
  /* # `CommandProcessor` #
   *
   * Each call to this functor processes a single command.
   *
   * All arguments are passed by reference, such that they may be used to form
   * a response.  If the integer return value of the call is zero, the call is assumed to have
   * no response required.  Otherwise, the arguments contain must contain response values. */
public:
  int operator () (CommandRequest &request, uint16_t buffer_size,
                   uint8_t *buffer) {
    return process_command(request, buffer_size, buffer);
  }

  int process_command(CommandRequest &request, uint16_t buffer_size,
                      uint8_t *buffer) {
    /* ## Call operator ##
     *
     * Arguments:
     *
     *  - `command`: Command code, designating the command to be executed.
     *  - `count`: The number of bytes in the arguments buffer.
     *  - `data`: The arguments buffer.
     *
     * __NB__ It is currently the responsibility */
    CommandResponse response;
    response.type = request.type;
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);
    switch (request.type) {
      case CommandType_ECHO:
        response.has_echo = true;
        break;
      case CommandType_RAM_SIZE:
        response.has_ram_size = true;
        response.ram_size.result = ram_size();
        break;
      case CommandType_RAM_DATA_SIZE:
        response.has_ram_data_size = true;
        response.ram_data_size.result = data_size();
        break;
      case CommandType_RAM_BSS_SIZE:
        response.has_ram_bss_size = true;
        response.ram_bss_size.result = bss_size();
        break;
      case CommandType_RAM_HEAP_SIZE:
        response.has_ram_heap_size = true;
        response.ram_heap_size.result = heap_size();
        break;
      case CommandType_RAM_STACK_SIZE:
        response.has_ram_stack_size = true;
        response.ram_stack_size.result = stack_size();
        break;
      case CommandType_RAM_FREE:
        response.has_ram_free = true;
        response.ram_free.result = free_memory();
        break;
      default:
        break;
    }
    bool status = pb_encode(&stream, CommandResponse_fields, &response);
    if (status) {
      return stream.bytes_written;
    } else {
      return 0;
    }
  }
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
      CommandRequest request;
      bool status = false;
      pb_istream_t stream = pb_istream_from_buffer(packet.payload_buffer_,
                                                   packet.payload_length_);
      status = pb_decode(&stream, CommandRequest_fields, &request);
      if (!status) {
        packet.type(Packet::packet_type::NACK);
      } else {
        packet.payload_length_ = command_processor_.process_command(
            request, packet.buffer_size_, packet.payload_buffer_);
      }
    }
    write_packet(ostream_, packet);
  }
};


#endif  // #ifndef ___SIMPLE_COMMAND__H___
