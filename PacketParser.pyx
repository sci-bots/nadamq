from libcpp.string cimport string
from libc.stdint cimport uint16_t
import numpy as np


ctypedef unsigned char uchar


cdef extern from "PacketParser.hpp":
    cdef cppclass Packet:
        uchar command_
        uint16_t payload_length_
        uchar *payload_buffer_

        Packet()
        Packet(uchar *payload_buffer, uint16_t payload_length)
        string data() except +

    cdef cppclass PacketParser:
        int payload_bytes_received_
        bint packet_completed_

        PacketParser()
        bint parse(char *buffer, uint16_t buffer_length, Packet *packet)


cdef class cPacket:
    cdef Packet *thisptr

    def __cinit__(self):
        self.thisptr = new Packet()

    def data(self):
        return self.thisptr.data()

    def __dealloc__(self):
        del self.thisptr

    def data_ptr(self):
        return <size_t>self.thisptr.payload_buffer_


cdef class cPacketParser:
    cdef PacketParser *thisptr

    def __cinit__(self):
        self.thisptr = new PacketParser()

    def __dealloc__(self):
        del self.thisptr

    def parse(self, uchar [:] packet_buffer):
        packet = cPacket()
        cdef bint result = self.thisptr.parse(<char *>&packet_buffer[0],
                                              len(packet_buffer),
                                              packet.thisptr)
        if result:
            return packet
        raise RuntimeError, ('Error parsing packet: %s' %
                             np.asarray(packet_buffer).tostring())
