from pprint import pprint
from datetime import datetime
import re
from pprint import pformat
import time

import numpy as np
from nadamq.NadaMq import (cPacket, PACKET_TYPES, cPacketParser)
import serial


# Function to convert from camel-case to underscore separated.  Taken from
# [here][1].
#
# [1]: https://djangosnippets.org/snippets/585/
def camelcase_to_underscore(value):
    return re.sub('(((?<=[a-z])[A-Z])|([A-Z](?![A-Z]|$)))', '_\\1',
                  value).lower().strip('_')


class CommandRequestManagerBase(object):
    '''
    This class acts as a factory for creating Protocol buffer command request
    objects.

    # Requirements #

    The definition of an instance of this class requires a set of
    Protocol buffer definitions similar to the following:

        enum CommandType {
            MY_COMMAND_A      = 1;
            MY_COMMAND_B      = 2;
        }

        message MyCommandARequest {}
        message MYCommandBRequest {}

        message MyCommandAResponse { required uint32 result = 1; }
        message MYCommandBResponse { required int8 result = 1; }

        message CommandRequest {
            optional MyCommandARequest my_command_a = 1;
            optional MyCommandBRequest my_command_b = 2;
        }

        message CommandResponse {
            optional MyCommandAResponse my_command_a = 1;
            optional MyCommandBResponse my_command_b = 2;
        }

    Specifically, the following conditions must be met:

     - An enumerated type must be defined, containing one entry for each type
       of command.
     - A request message type _(e.g., `MyCommandARequest`)_ must be defined for
       each type of command.
     - A response message type _(e.g., `MyCommandAResponse`)_ must be defined
       for each type of command.
     - A union request message type _(e.g., `CommandRequest`) must be defined,
       containing:
      - A `type` field, which indicates which type of request is filled in.
      - An optional field for each type of possible request, only one of which
        will be populated in each union request message instance.
     - A union response message type _(e.g., `CommandResponse`) must be
       defined, containing:
      - A `type` field, which indicates which type of response is filled in.
      - An optional field for each type of possible response, only one of which
        will be populated in each union response message instance.

    ## `request_types` ##

    The `request_types` argument must be a `dict`-like object, containing a
    mapping from each request name, with the `Request` post-fix stripped
    _(e.g., `MyCommandA`, `MyCommandB`), to the corresponding Python Protocol
    buffer message class.

    ## `request_class` ##

    The `request_class` argument must be the Python Protocol buffer union
    request message class _(e.g., `CommandRequest`)_.

    ## `response_class` ##

    The `response_class` argument must be the Python Protocol buffer union
    response message class _(e.g., `CommandResponse`)_.

    ## `command_type_class` ##

    The `command_type_class` argument must be the Python Protocol buffer
    enumerated command type class _(e.g., `CommandType`)_.
    '''
    def __init__(self, request_types, request_class, response_class,
                 command_type_class):
        self.request_types = request_types
        self.request_class = request_class
        self.response_class = response_class
        self.command_type_class = command_type_class

    @property
    def command_names(self):
        '''
        Return a list containing the camel-case name of each available command.
        '''
        return self.request_types.keys()

    def request(self, request_type_name, **kwargs):
        '''
        Return a serialized Protocol buffer message instance corresponding to
        the provided request type name _(e.g., `'MyCommandA'`)_.

        __NB__ Any provided keyword arguments are passed on to the Protocol
        buffer message class constructor, allowing initial values to be set on
        the message.
        '''
        lowercase = camelcase_to_underscore(request_type_name)
        command_kwargs = {lowercase:
                          self.request_types[request_type_name](**kwargs)}
        return self.request_class(**command_kwargs).SerializeToString()

    def response(self, byte_data):
        '''
        Return a Protocol Buffer response object deserialized from the provided
        string of encoded message bytes.
        '''
        response = self.response_class.FromString(byte_data)
        sub_response = response.ListFields()[0][1]
        return sub_response


class CommandRequestManagerDebug(CommandRequestManagerBase):
    def request(self, request_type_name, **kwargs):
        encoded_request = super(CommandRequestManagerDebug,
                                self).request(request_type_name, **kwargs)
        print '# `%sRequest` #' % request_type_name
        print ''
        print ' - Arguments:'
        for k, v in kwargs.iteritems():
            print '  - `%s`: `%s`' % (k, v)
        data = np.fromstring(encoded_request, dtype=np.uint8)
        print ' - Encoded: `%s`' % repr(data.tostring())
        print '            `%s`' % data
        print ''
        return encoded_request

    def response(self, byte_data):
        '''
        Return a Protocol Buffer response object deserialized from the provided
        string of encoded message bytes, along with the raw byte data.  This is
        useful for determining the length of the encoded protocol buffer
        message, and/or debugging.
        '''
        sub_response = super(CommandRequestManagerDebug,
                             self).response(byte_data)
        return sub_response, byte_data


class CommandRequestManager(CommandRequestManagerBase):
    def response(self, byte_data):
        '''
        Return a response based on the provided string of encoded message
        bytes.

        By convention, we consider the return value of a response message to
        correspond to the value of the field named `result`.  If there is no
        `result` field, return the specific response object _(e.g.,
        `MyCommandAResponse`)_ to give the caller a chance to manually retrieve
        the return value.
        '''
        sub_response = super(CommandRequestManager, self).response(byte_data)
        return getattr(sub_response, 'result', sub_response)


class NodeProxy(object):
    '''
    This class uses a command request manager to automatically populate methods
    to expose the API of a remote node, where the interface to the remote node
    is provided through the specified `stream` instance.

    # Requirements #

    ## `command_request_mananger` ##

    The `command_request_manager` must provide the following API:

     - Attributes:
      - `command_names`: A list containing the camel-case name of each
        available command.
     - Methods:
      - `request`: Given a request type name and an optional set of keyword
        arguments, return an encoded request string, suitable for sending over
        a byte stream.
      - `response`: Given an encoded response string, return a result value, or
        `None` if there is no result value.

    ## `stream` ##

    The `stream` argument must provide the following API:

     - Methods:
      - `available()`: Return the number of bytes available for reading.
      - `read(count=None)`: Read the specified number of bytes from the stream.
        If `count` is `None`, read as many bytes as are currently available.
      - `write(data)`: Write the specified data to the stream.
    '''
    def __init__(self, command_request_manager, stream, timeout=None):
        timeout = 1.5 if timeout is None else timeout
        self._timeout = timeout
        self._stream = stream
        self._command_request_manager = command_request_manager
        self._initialize_methods()

    def _do_request(self, name):
        # Note that we need to use [partial function application][1]
        # here so we can encapsulate the current value of
        # `command_name` from this loop iteration when calling the
        # associated generated method outside of this loop.  If we
        # _don't_ create the `f` function below, the `name` variable
        # would refer to the last `command_name` value visited by the
        # loop in all methods, which is _not_ the behaviour we want.
        #
        # [1]: http://en.wikipedia.org/wiki/Partial_application
        def f(**kwargs):
            retry_count = kwargs.pop('retry_count', 10)
            remote_address = kwargs.pop('remote_address', None)
            if remote_address is not None:
                def _remote_func(**kwargs):
                    request = self._command_request_manager.request(name,
                                                                    **kwargs)
                    return (getattr(self, 'forward_i2c_request')
                            (address=remote_address, request=request))
                command_func = _remote_func
            else:
                command_func = (lambda **kwargs:
                                self._do_request_from_command_name(
                                    name, **kwargs))
            for i in xrange(retry_count):
                try:
                    return command_func(**kwargs)
                except ValueError, exception:
                    exception_str = str(exception)
                    if not exception_str.startswith('Timeout'):
                        raise
            raise exception
        return f

    def _initialize_methods(self):
        # Add method for each type of request discovered from `requests`
        # module.
        #
        # For instance, for the following list of command names:
        #
        #     ['MyCommandA', 'MyCommandB']
        #
        # the following methods will be created:
        #
        #  - `my_command_a(**kwargs)`
        #  - `my_command_b(**kwargs)`
        #
        # where the `kwargs` will be passed along to the corresponding command.
        for command_name in self._command_request_manager.command_names:
            method_name = camelcase_to_underscore(command_name)
            setattr(self, method_name, self._do_request(command_name))

    def _do_request_from_command_name(self, command_name, iuid=0, **kwargs):
        request = self._command_request_manager.request(command_name, **kwargs)
        packet = cPacket(iuid=iuid, type_=PACKET_TYPES.DATA,
                         data=request)
        # Flush any remaining bytes from stream.
        self._stream.read()
        # Write request packet to stream.
        self._stream.write(packet.tostring())
        parser = cPacketParser()
        data = np.array([ord(v) for v in self._stream.read()], dtype='uint8')
        start = datetime.now()
        wait_counts = 0
        try:
            result = parser.parse(data)
            while not result:
                data = np.array([ord(v) for v in self._stream.read()],
                                dtype='uint8')
                result = parser.parse(data)
                if (datetime.now() - start).total_seconds() > self._timeout:
                    raise ValueError('Timeout while waiting for packet.\n"%s"'
                                     % (pformat(data.tostring())))
                if not result:
                    time.sleep(0.0001)
                    wait_counts += 1
                else:
                    response_packet = result
                    break
        except RuntimeError:
            raise ValueError('Error parsing response packet.\n"%s"' %
                             (pformat(data.tostring())))
        if response_packet.type_ == PACKET_TYPES.DATA:
            if command_name == 'ForwardI2cRequest':
                # This was a forwarded request, so we must return the undecoded
                # response data, since decoding is the responsibility of the
                # calling code.
                return response_packet.data()
            else:
                return (self._command_request_manager
                        .response(response_packet.data()))
        else:
            raise ValueError('Invalid response. (%s).\n"%s"' %
                             (response_packet.type_, pformat(data)))


class Stream(object):
    '''
    This class defines an API that a stream class must implement to be used by
    a `NodeProxy` instance.

    # Methods #

     - `available()`: Return the number of bytes available for reading.
     - `read(count=None)`: Read the specified number of bytes from the stream.
       If `count` is `None`, read as many bytes as are currently available.
     - `write(data)`: Write the specified data to the stream.

    # TODO #

     - Add `timeout` option for `read` method, which, when set, raises an
       exception if the specified number of bytes is not read after the
       specified timeout.
    '''
    def available(self):
        raise NotImplementedError

    def read(self, count):
        raise NotImplementedError

    def write(self, data):
        raise NotImplementedError


class SerialStream(object):
    '''
    This class provides an adapter around the `serial.Serial` class to expose
    the following API:

     - Methods:
      - `available()`: Return the number of bytes available for reading.
      - `read(count=None)`: Read the specified number of bytes from the stream.
        If `count` is `None`, read as many bytes as are currently available.
      - `write(data)`: Write the specified data to the stream.
    '''
    def __init__(self, *args, **kwargs):
        self._serial = None
        self._args = args
        self._kwargs = kwargs
        self.reconnect()

    def reconnect(self):
        if self._serial is not None:
            self._serial.close()
        self._serial = serial.Serial(*self._args, **self._kwargs)
        time.sleep(.05)
        # Flush welcome message.
        print self.read()

    def available(self):
        return self._serial.inWaiting()

    def read(self, count=None):
        if count is None:
            count = self.available()
        else:
            count = min(self.available(), count)
        return self._serial.read(count)

    def write(self, data):
        self._serial.write(data)


class RemoteNodeProxy(object):
    '''
    By default, this class forwards all method calls through a connected device
    '''

    def __init__(self, forward_proxy, remote_address, command_request_manager):
        self._forward_proxy = forward_proxy
        self._remote_address = remote_address
        self._command_request_manager = command_request_manager
        self._initialize_methods()

    def _initialize_methods(self):
        # Add method for each type of request discovered from `requests`
        # module.
        #
        # For instance, for the following list of command names:
        #
        #     ['MyCommandA', 'MyCommandB']
        #
        # the following methods will be created:
        #
        #  - `my_command_a(**kwargs)`
        #  - `my_command_b(**kwargs)`
        #
        # where the `kwargs` will be passed along to the corresponding command.
        for command_name in self._command_request_manager.command_names:
            method_name = camelcase_to_underscore(command_name)
            setattr(self, method_name, self._do_request(command_name))

    def _do_request(self, name):
        # Note that we need to use [partial function application][1]
        # here so we can encapsulate the current value of
        # `command_name` from this loop iteration when calling the
        # associated generated method outside of this loop.  If we
        # _don't_ create the `f` function below, the `name` variable
        # would refer to the last `command_name` value visited by the
        # loop in all methods, which is _not_ the behaviour we want.
        #
        # [1]: http://en.wikipedia.org/wiki/Partial_application
        def f(**kwargs):
            retry_count = kwargs.pop('retry_count', 10)

            def _remote_func(**kwargs):
                request = self._command_request_manager.request(name,
                                                                **kwargs)
                # Responses from `forward_i2c_request` requests are raw byte
                # streams.  It is our responsibility, as the source of a
                # `forward_i2c_request` request, to decode the raw response
                # bytes, since the forwarder may not provide the same API.
                return self._command_request_manager.response(
                    self._forward_proxy
                    .forward_i2c_request(address=self._remote_address,
                                         request=request))
            command_func = _remote_func
            for i in xrange(retry_count):
                try:
                    return command_func(**kwargs)
                except ValueError, exception:
                    exception_str = str(exception)
                    if not exception_str.startswith('Timeout'):
                        raise
            raise exception
        return f
