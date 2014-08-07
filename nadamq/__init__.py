import os


def get_includes():
    import nadamq
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
    return [os.path.join(os.path.dirname(nadamq.__file__), 'src')]


def get_sources():
    import nadamq
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
    return [os.path.join(source_dir, f) for f in ('crc-16.c', 'crc_common.cpp',
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
