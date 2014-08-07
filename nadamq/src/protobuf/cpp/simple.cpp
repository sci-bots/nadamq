#include <iostream>
#include <fstream>
#include <string>
#include "simple.pb.h"
using namespace std;


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
    case ECHO:
      cout << "Echo request" << endl;
      response.set_type(ECHO);
      response_ptr.echo = new EchoResponse();
      response.set_allocated_echo(response_ptr.echo);
      response.SerializeToOstream(&output);
      break;
    case RAM_SIZE:
      cout << "RAMSizeRequest request" << endl;
      response.set_type(RAM_SIZE);
      response_ptr.ram_size = new RAMSizeResponse();
      response_ptr.ram_size->set_result(1234);
      response.set_allocated_ram_size(response_ptr.ram_size);
      response.SerializeToOstream(&output);
      break;
    case RAM_DATA_SIZE:
      cout << "RAMDataSizeRequest request" << endl;
      response.set_type(RAM_DATA_SIZE);
      response_ptr.ram_data_size = new RAMDataSizeResponse();
      response_ptr.ram_data_size->set_result(1234);
      response.set_allocated_ram_data_size(response_ptr.ram_data_size);
      response.SerializeToOstream(&output);
      break;
    case RAM_BSS_SIZE:
      cout << "RAMBSSSizeRequest request" << endl;
      response.set_type(RAM_BSS_SIZE);
      response_ptr.ram_bss_size = new RAMBSSSizeResponse();
      response_ptr.ram_bss_size->set_result(1234);
      response.set_allocated_ram_bss_size(response_ptr.ram_bss_size);
      response.SerializeToOstream(&output);
      break;
    case RAM_HEAP_SIZE:
      cout << "RAMHeapSizeRequest request" << endl;
      response.set_type(RAM_HEAP_SIZE);
      response_ptr.ram_heap_size = new RAMHeapSizeResponse();
      response_ptr.ram_heap_size->set_result(1234);
      response.set_allocated_ram_heap_size(response_ptr.ram_heap_size);
      response.SerializeToOstream(&output);
      break;
    case RAM_STACK_SIZE:
      cout << "RAMStackSizeRequest request" << endl;
      response.set_type(RAM_STACK_SIZE);
      response_ptr.ram_stack_size = new RAMStackSizeResponse();
      response_ptr.ram_stack_size->set_result(1234);
      response.set_allocated_ram_stack_size(response_ptr.ram_stack_size);
      response.SerializeToOstream(&output);
      break;
    case RAM_FREE:
      cout << "RAMFreeRequest request" << endl;
      response.set_type(RAM_FREE);
      response_ptr.ram_free = new RAMFreeResponse();
      response_ptr.ram_free->set_result(1234);
      response.set_allocated_ram_free(response_ptr.ram_free);
      response.SerializeToOstream(&output);
      break;
    default:
      break;
  }
}

// Main function:  Reads the entire address book from a file and prints all
//   the information inside.
int main(int argc, char* argv[]) {
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  if (argc != 2) {
    cerr << "Usage:  " << argv[0] << " COMMAND_FILE" << endl;
    return -1;
  }

  CommandRequest request;

  {
    // Read the existing address book.
    fstream input(argv[1], ios::in | ios::binary);
    if (!request.ParseFromIstream(&input)) {
      cerr << "Failed to parse command request." << endl;
      return -1;
    }
  }

  dump_request(request);

  // Optional:  Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
