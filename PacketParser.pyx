from libcpp.string cimport string


cdef extern from "PacketParser.hpp":
    cdef cppclass PacketParser:
        int payload_length_
        int payload_bytes_received_
        bint packet_completed_


        PacketParser()
        bint parse(char *string, unsigned char command)


cdef class cPacketParser:
    cdef PacketParser *thisptr

    def __cinit__(self):
        self.thisptr = new PacketParser()

    def __dealloc__(self):
        del self.thisptr

    def parse(self, data):
        cdef string data_ = data
        cdef unsigned char command
        cdef bint result = self.thisptr.parse(<char *>data_.c_str(), command)
        if result:
            return command
        raise RuntimeError, 'Error parsing packet: %s' % data
