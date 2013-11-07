from libcpp.string cimport string


cdef extern from "PacketParser.hpp":
    cdef cppclass PacketParser:
        int payload_length_
        int payload_bytes_received_

        PacketParser()
        int parse(char *string)


cdef class cPacketParser:
    cdef PacketParser *thisptr

    def __cinit__(self):
        self.thisptr = new PacketParser()

    def __dealloc__(self):
        del self.thisptr

    def parse(self, data):
        cdef string data_ = data
        return self.thisptr.parse(<char *>data_.c_str())
