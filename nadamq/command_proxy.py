from pprint import pprint
import time

from nadamq.NadaMq import (cPacket, PACKET_TYPES, parse_from_string)
from .requests import (request_camelcase, get_request_from_camelcase,
                       CommandResponse, camelcase_to_underscore)
import serial


class NodeProxy(object):
    def __init__(self):
        self._serial = None
        self._reconnect()
        # Add method for each type of request discovered from `requests`
        # module.
        for camel in request_camelcase:
            def _do_request(self, name):
                def f(**kwargs):
                    response = self._do_request_from_camelcase(name, **kwargs)
                    lower_name = camelcase_to_underscore(name)
                    sub_response = getattr(response, lower_name)
                    return getattr(sub_response, 'result', sub_response)
                return f
            method_name = camelcase_to_underscore(camel)
            setattr(self, method_name, _do_request(self, camel))

    def _reconnect(self):
        if self._serial is not None:
            self._serial.close()
        self._serial = serial.Serial('/dev/ttyUSB0', baudrate=115200)
        time.sleep(.05)
        print self._serial.read(self._serial.inWaiting())

    def _do_request_from_camelcase(self, camelcase_request_name, iuid=0,
                                   **kwargs):
        request = get_request_from_camelcase(camelcase_request_name, **kwargs)
        return self._do_request(request, iuid=iuid)

    def _do_request(self, request, iuid=0):
        packet = cPacket(iuid=iuid, type_=PACKET_TYPES.DATA,
                         data=request.SerializeToString())
        self._serial.write(packet.tostring())
        time.sleep(.1)
        response_packet = parse_from_string(self._serial.read(self._serial
                                                              .inWaiting()))
        if response_packet.type_ == PACKET_TYPES.DATA:
            return CommandResponse.FromString(response_packet.data())
        else:
            raise ValueError('Invalid response.')
