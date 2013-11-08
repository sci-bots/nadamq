from libcpp.string cimport string
from libc.stdint cimport uint16_t
import numpy as np


ctypedef unsigned char uchar
ctypedef uint16_t crc_t


cdef extern from "PacketParser.hpp":
    cdef cppclass Packet:
        uchar command_
        uint16_t payload_length_
        uchar *payload_buffer_
        bint has_crc_
        uint16_t crc_

        Packet()
        Packet(uchar *payload_buffer, uint16_t payload_length)
        string data() except +

    cdef cppclass PacketParser:
        int payload_bytes_received_
        bint packet_completed_
        uint16_t crc_

        PacketParser()
        bint parse(char *buffer, uint16_t buffer_length, Packet *packet)


cdef extern from "crc-16.h":
    crc_t c_crc_init "crc_init" ()
    crc_t c_crc_finalize "crc_finalize" (crc_t crc)
    crc_t c_crc_update "crc_update" (crc_t crc, uchar *data, size_t data_len)
    crc_t c_crc_update_byte "crc_update_byte" (crc_t crc, uchar data)


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

    property has_crc:
        def __get__(self):
            return self.thisptr.has_crc_

    property crc:
        def __get__(self):
            if not self.thisptr.has_crc_:
                raise RuntimeError, 'Packet has no CRC'
            return self.thisptr.crc_

    property command:
        def __get__(self):
            return self.thisptr.command_


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

    property crc:
        def __get__(self):
            return self.thisptr.crc_


def crc_init():
    return c_crc_init()


def crc_finalize(crc):
    return c_crc_finalize(crc)


def crc_update(uint16_t crc, uchar [:] data):
    return c_crc_update(crc, &data[0], len(data))


def crc_update_byte(uint16_t crc, uchar octet):
    return c_crc_update_byte(crc, octet)


def compute_crc16(data):
    cdef string data_ = data
    cdef uint16_t crc = crc_init()
    crc = c_crc_update(crc, <uchar *>data_.c_str(), len(data))
    return crc_finalize(crc)
