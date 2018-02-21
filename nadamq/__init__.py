from __future__ import absolute_import
import datetime as dt
import os
import threading

from or_event import OrEvent
import numpy as np
import si_prefix as si

from ._version import get_versions

__version__ = get_versions()['version']
del get_versions


def get_includes():
    r"""
    Return the directory that contains the `nadamq` Cython *.hpp and
    *.pxd header files.

    Extension modules that need to compile against `nadamq` should use
    this function to locate the appropriate include directory.

    Notes
    -----
    When using ``distutils``, for example in ``setup.py``.
    ::

        import nadamq
        ...
        Extension('extension_name', ...
                  include_dirs=[...] + nadamq.get_includes())
        ...

    """
    return [os.path.join(os.path.dirname(__file__), 'src')]


def get_sources():
    r"""
    Return a list of the additional *.cpp files that must be compiled along
    with the `nadamq` Cython extension definitions.

    Extension modules that need to compile against `nadamq` should use
    this function to locate the appropriate source files.

    Notes
    -----
    When using ``distutils``, for example in ``setup.py``.
    ::

        import nadamq
        ...
        Extension('extension_name', ...
                  sources + nadamq.get_sources())
        ...

    """
    source_dir = get_includes()[0]
    return [os.path.join(source_dir, f) for f in ('crc-16.cpp',
                                                  'crc_common.cpp',
                                                  'packet_actions.cpp')]


def get_arduino_library_sources():
    r"""
    Return a list of files to include in the Arduino library to support NadaMq
    packets on Arduino devices.
    """
    source_dir = get_includes()[0]
    library_sources = [('Arduino', 'packet_handler', 'output_buffer.h'),
                       ('Arduino', 'packet_handler', 'packet_handler.h'),
                       ('Arduino', 'packet_handler', 'NadaMQ.h'),
                       ('BufferAllocator.h',), ('Packet.h',),
                       ('PacketAllocator.h',), ('PacketHandler.h',),
                       ('PacketParser.h',), ('PacketSocket.h',),
                       ('PacketSocketEvents.h',), ('PacketStream.h',),
                       ('PacketWriter.h',), ('SimpleCommand.h',),
                       ('StreamPacketParser.h',), ('crc-16.c',), ('crc-16.h',),
                       ('crc_common.cpp',), ('crc_common.h',),
                       ('packet_actions.cpp',)]
    return [os.path.join(source_dir, *f) for f in library_sources]


def read_packet(read_func, timeout_s=None, poll_s=0.001):
    '''
    Read packet from specified callback function.

    Blocks until full packet is read (or exception occurs).

    .. versionadded:: 0.14

    Parameters
    ----------
    read_func : function
        Callback function.  Must return ``bytes``.
    timeout_s : float, optional
        Number of seconds to wait for full packet.

        By default, block until packet is received.
    poll_s : float, optional
        Time to wait between calls to :func:`read_func`.

    Returns
    -------
    cPacket
        Parsed packet.

    Raises
    ------
    RuntimeError
        If specified time out is reached before a packet is received.
    Exception
        If an exception is encountered while reading or parsing, the exception
        is raised.
    '''
    from .NadaMq import cPacketParser

    # Record start time.
    start = dt.datetime.utcnow()

    stop_request = threading.Event()
    packet_ready = threading.Event()
    parse_error = threading.Event()

    result = {}

    def _do_read(read_func, output):
        parser = cPacketParser()

        try:
            while True:
                data = read_func()
                if data:
                    result_ = parser.parse(np.fromstring(data, dtype='uint8'))
                    if result_ is not False:
                        output['response'] = result_
                        packet_ready.set()
                        break
                if stop_request.wait(poll_s):
                    break
        except Exception as exception:
            # Exception occurred while reading/parsing.
            # Store exception and report to calling thread.
            parse_error._exception = exception
            parse_error.set()

    # Start background thread to read data.
    thread = threading.Thread(target=_do_read, args=(read_func, result))
    thread.daemon = True
    thread.start()

    # Create combined event to wait for either a completed packet or an error.
    complete = OrEvent(packet_ready, parse_error)

    if not complete.wait(timeout=timeout_s):
        stop_request.set()
        raise RuntimeError('Timed out waiting for packet (after %ss).' %
                           si.si_format((dt.datetime.utcnow() -
                                         start).total_seconds()))
    elif parse_error.is_set():
        # Exception occurred while reading/parsing.
        raise parse_error._exception
    return result['response']
