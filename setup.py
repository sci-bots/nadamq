#!/usr/bin/env python
import sys
from distutils.core import setup, Extension
from Cython.Build import cythonize

import packet_parser


include_dirs = packet_parser.get_includes()
sys.path += include_dirs

cy_config = dict(include_dirs=include_dirs, language='c++',
                 extra_compile_args=['-O3', '-Wfatal-errors'])

cy_exts = [Extension('packet_parser.%s' % v, packet_parser.get_sources()
                     + ['packet_parser/%s.pyx' % v], **cy_config)
           for v in ('PacketParser', )]

setup(name="packet_parser",
      version="0.0.1",
      description="Embedded command packet parser",
      keywords="cython embedded packet parse",
      author="Christian Fobel",
      url="https://github.com/cfobel/packet_parser",
      license="GPL",
      packages=['packet_parser', ],
      ext_modules=cythonize(cy_exts))
