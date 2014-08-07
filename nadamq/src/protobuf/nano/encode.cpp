#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "simple.pb.h"
using namespace std;


#if 0
void dump_request(const CommandRequest& request) {
  cout << "Size: " << sizeof(request) << endl;
  cout << "Type: " << request.type() << endl;

  CommandResponse response;

  union {
    EchoResponse *echo;
    RAMSizeResponse *ram_size;
    RAMDataSizeResponse *ram_data_size;
    RAMBSSSizeResponse *ram_bss_size;
    RAMHeapSizeResponse *ram_heap_size;
    RAMStackSizeResponse *ram_stack_size;
    RAMFreeResponse *ram_free;
  } response_ptr;

  ofstream output("response.dat");

  switch (request.type()) {
    case CommandRequest::ECHO:
      cout << "Echo request" << endl;
      response.set_type(CommandResponse::ECHO);
      response_ptr.echo = new EchoResponse();
      response.set_allocated_echo(response_ptr.echo);
      response.SerializeToOstream(&output);
      break;
    case CommandRequest::RAM_SIZE:
      cout << "RAMSizeRequest request" << endl;
      response.set_type(CommandResponse::RAM_SIZE);
      response_ptr.ram_size = new RAMSizeResponse();
      response_ptr.ram_size->set_result(1234);
      response.set_allocated_ram_size(response_ptr.ram_size);
      response.SerializeToOstream(&output);
      break;
    case CommandRequest::RAM_DATA_SIZE:
      cout << "RAMDataSizeRequest request" << endl;
      response.set_type(CommandResponse::RAM_DATA_SIZE);
      response_ptr.ram_data_size = new RAMDataSizeResponse();
      response_ptr.ram_data_size->set_result(1234);
      response.set_allocated_ram_data_size(response_ptr.ram_data_size);
      response.SerializeToOstream(&output);
      break;
    case CommandRequest::RAM_BSS_SIZE:
      cout << "RAMBSSSizeRequest request" << endl;
      response.set_type(CommandResponse::RAM_BSS_SIZE);
      response_ptr.ram_bss_size = new RAMBSSSizeResponse();
      response_ptr.ram_bss_size->set_result(1234);
      response.set_allocated_ram_bss_size(response_ptr.ram_bss_size);
      response.SerializeToOstream(&output);
      break;
    case CommandRequest::RAM_HEAP_SIZE:
      cout << "RAMHeapSizeRequest request" << endl;
      response.set_type(CommandResponse::RAM_HEAP_SIZE);
      response_ptr.ram_heap_size = new RAMHeapSizeResponse();
      response_ptr.ram_heap_size->set_result(1234);
      response.set_allocated_ram_heap_size(response_ptr.ram_heap_size);
      response.SerializeToOstream(&output);
      break;
    case CommandRequest::RAM_STACK_SIZE:
      cout << "RAMStackSizeRequest request" << endl;
      response.set_type(CommandResponse::RAM_STACK_SIZE);
      response_ptr.ram_stack_size = new RAMStackSizeResponse();
      response_ptr.ram_stack_size->set_result(1234);
      response.set_allocated_ram_stack_size(response_ptr.ram_stack_size);
      response.SerializeToOstream(&output);
      break;
    case CommandRequest::RAM_FREE:
      cout << "RAMFreeRequest request" << endl;
      response.set_type(CommandResponse::RAM_FREE);
      response_ptr.ram_free = new RAMFreeResponse();
      response_ptr.ram_free->set_result(1234);
      response.set_allocated_ram_free(response_ptr.ram_free);
      response.SerializeToOstream(&output);
      break;
    default:
      break;
  }
}
#endif


/* This function is the core of the union encoding process. It handles the
 * top-level pb_field_t array manually, in order to encode a correct field tag
 * before the message. The pointer to MsgType_fields array is used as an unique
 * identifier for the message type. */
bool encode_submessage(pb_ostream_t *stream,
                       const pb_field_t super_message_type[],
                       const pb_field_t sub_message_type[],
                       const void *message) {
  const pb_field_t *field;
  for (field = super_message_type; field->tag != 0; field++) {
    if (field->ptr == sub_message_type) {
      /* This is our field, encode the message using it. */
      if (!pb_encode_tag_for_field(stream, field)) {
        return false;
      }

      return pb_encode_submessage(stream, sub_message_type, message);
    }
  }

  /* Didn't find the field for messagetype */
  return false;
}

// Main function:  Reads the entire address book from a file and prints all
//   the information inside.
int main(int argc, char* argv[]) {
  uint8_t buffer[256];

  //if (argc != 2) {
    //cerr << "Usage:  " << argv[0] << " COMMAND_FILE" << endl;
    //return -1;
  //}
  if (argc != 2) {
    cerr << "Usage:  " << argv[0] << " <output file>" << endl;
    return -1;
  }

  bool status = false;
  /* Create a stream that will write to our buffer. */
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  /* Encode our message */
  {
    /* Allocate space on the stack to store the message data.
     *
     * Nanopb generates simple struct definitions for all the messages.
     * - check out the contents of simple.pb.h! */
#if 0
    CommandRequest request;

    /* Fill in the lucky number */
    request.type = CommandType_ECHO;

    EchoRequest echo;
    status = encode_submessage(&stream, CommandRequest_fields,
                               EchoRequest_fields, &echo);
#endif
    CommandResponse response;
    response.type = CommandType_RAM_FREE;
    response.has_ram_free = true;
    response.ram_free.result = 1234;
    status = pb_encode(&stream, CommandResponse_fields, &response);
  }
  if (!status) {
    cerr << "Encoding error." << endl;
    return -1;
  }
  cout << "status: " << status << endl;
  cout << "stream byte count: " << stream.bytes_written << endl;
  ofstream output(argv[1]);
  output << string((char *)&buffer[0], stream.bytes_written);
  return 0;
}
