#!/usr/bin/env python
import sys
from distutils.core import setup, Extension
from Cython.Build import cythonize

import nadamq
import version


include_dirs = nadamq.get_includes()
sys.path += include_dirs

cy_config = dict(include_dirs=include_dirs, language='c++',
                 extra_compile_args=['-O3', '-Wfatal-errors'])

cy_exts = [Extension('nadamq.%s' % v, nadamq.get_sources()
                     + ['nadamq/%s.pyx' % v], **cy_config)
           for v in ('NadaMq', )]

setup(name='nadamq',
      version=version.getVersion(),
      description='Embedded-friendly transport layer, inspired by ZeroMQ',
      keywords='cython embedded zeromq transport packet parse',
      author='Christian Fobel',
      url='https://github.com/cfobel/nadamq',
      license='GPL',
      packages=['nadamq', ],
      ext_modules=cythonize(cy_exts))
