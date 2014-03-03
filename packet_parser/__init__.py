import os


def get_includes():
    import packet_parser
    r"""
    Return the directory that contains the `packet_parser` Cython *.hpp and
    *.pxd header files.

    Extension modules that need to compile against `packet_parser` should use
    this function to locate the appropriate include directory.

    Notes
    -----
    When using ``distutils``, for example in ``setup.py``.
    ::

        import packet_parser
        ...
        Extension('extension_name', ...
                  include_dirs=[...] + packet_parser.get_includes())
        ...

    """
    return [os.path.join(os.path.dirname(packet_parser.__file__), 'src')]


def get_sources():
    import packet_parser
    r"""
    Return a list of the additional *.cpp files that must be compiled along
    with the `packet_parser` Cython extension definitions.

    Extension modules that need to compile against `packet_parser` should use
    this function to locate the appropriate source files.

    Notes
    -----
    When using ``distutils``, for example in ``setup.py``.
    ::

        import packet_parser
        ...
        Extension('extension_name', ...
                  sources + packet_parser.get_sources())
        ...

    """
    source_dir = get_includes()[0]
    return [os.path.join(source_dir, f) for f in ('crc-16.c',
                                                  'packet_actions.cpp')]
