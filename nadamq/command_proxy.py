import time

from nadamq.NadaMq import (cPacket, PACKET_TYPES, parse_from_string, COMMANDS)
import serial
import numpy as np


class DeviceProxy(object):
    def __init__(self):
        self.serial = None
        self.result_types = dict([
            (COMMANDS.ECHO, None),
            (COMMANDS.SYSTEM__RAM_SIZE, np.uint32),
            (COMMANDS.SYSTEM__RAM_DATA_SIZE, np.uint32),
            (COMMANDS.SYSTEM__RAM_BSS_SIZE, np.uint32),
            (COMMANDS.SYSTEM__RAM_HEAP_SIZE, np.uint32),
            (COMMANDS.SYSTEM__RAM_STACK_SIZE, np.uint32),
            (COMMANDS.SYSTEM__RAM_FREE, np.uint32),
            (COMMANDS.SYSTEM__SIMPLE_TYPE, np.dtype([('a', '<f4'),
                                                     ('b', '<u4'),
                                                     ('c', '<i2')])),
        ])
        self._reconnect()

    def _reconnect(self):
        if self.serial is not None:
            self.serial.close()
        self.serial = serial.Serial('/dev/ttyUSB0', baudrate=115200)
        time.sleep(.1)
        print self.serial.read(self.serial.inWaiting())

    def test_echo_packet(self):
        self.do_command(0x10, 'hello, world!')

    def do_command(self, command, data='', iuid=0):
        packet = cPacket(iuid=iuid, type_=PACKET_TYPES.DATA,
                         data=chr(command) + data)
        self.serial.write(packet.tostring())
        time.sleep(.1)
        data = self.serial.read(self.serial.inWaiting())
        in_packet = parse_from_string(data)
        command = ord(in_packet.data()[0])
        result_type = self.result_types.get(command, None)
        if result_type is not None:
            try:
                item_size = result_type(0).itemsize
            except TypeError:
                item_size = result_type.itemsize
            count = len(in_packet.data()[1:]) / item_size
            result = np.fromstring(in_packet.data()[1:], count=count,
                                   dtype=result_type)
            if count == 1:
                result = result[0]
        else:
            result = None
        if result is None:
            return packet
        return result
