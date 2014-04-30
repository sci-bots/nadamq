import re
from pprint import pprint, pformat
import time

from nadamq.NadaMq import (cPacket, PACKET_TYPES, parse_from_string)
import serial


# Function to convert from camel-case to underscore separated.  Taken from
# [here][1].
#
# [1]: https://djangosnippets.org/snippets/585/
def camelcase_to_underscore(value):
    return re.sub('(((?<=[a-z])[A-Z])|([A-Z](?![A-Z]|$)))', '_\\1',
                  value).lower().strip('_')


class CommandRequestManager(object):
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
            // Identifies which field is filled in.
            required CommandType type = 1;

            optional MyCommandARequest my_command_a = 2;
            optional MyCommandBRequest my_command_b = 3;
        }

        message CommandResponse {
            // Identifies which field is filled in.
            required CommandType type = 1;

            optional MyCommandAResponse my_command_a = 2;
            optional MyCommandBResponse my_command_b = 3;
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
        kwargs.update({lowercase: self.request_types[request_type_name],
                       'type': self.command_type_class.Value(lowercase
                                                             .upper())})
        return self.request_class(**kwargs).SerializeToString()

    def response(self, command_name, byte_data):
        '''
        Return a response based on the provided string of encoded message
        bytes.
        '''
        response = self.response_class.FromString(byte_data)
        lower_name = camelcase_to_underscore(command_name)
        sub_response = getattr(response, lower_name)
        # By convention, we consider the return value of a response message to
        # correspond to the value of the field named `result`.  If there is no
        # `result` field, return the specific response object _(e.g.,
        # `MyCommandAResponse`)_ to give the caller a chance to manually
        # retrieve the return value.
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
    def __init__(self, command_request_manager, stream):
        self._stream = stream
        self._command_request_manager = command_request_manager

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
                    return self._do_request_from_command_name(name, **kwargs)
                return f
            method_name = camelcase_to_underscore(command_name)
            setattr(self, method_name, _do_request(self, command_name))

    def _do_request_from_command_name(self, command_name, iuid=0, **kwargs):
        request = self._command_request_manager.request(command_name, **kwargs)
        packet = cPacket(iuid=iuid, type_=PACKET_TYPES.DATA,
                         data=request)
        self._stream.write(packet.tostring())
        time.sleep(.1)
        data = self._stream.read()
        response_packet = parse_from_string(data)
        if response_packet.type_ == PACKET_TYPES.DATA:
            return (self._command_request_manager
                    .response(command_name, response_packet.data()))
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
