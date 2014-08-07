import sys
from Cython.Build import cythonize
from pprint import pprint

from paver.easy import task, needs, path, sh, cmdopts, options
from paver.setuputils import setup, find_package_data, install_distutils_tasks
from distutils.extension import Extension
from distutils.dep_util import newer

import version
# Add package directory to Python path. This enables the use of `nadamq`
# functions for discovering, e.g., the path to the Arduino firmware sketch
# source files.
sys.path.append(path('.').abspath())
import nadamq
import version

include_dirs = nadamq.get_includes()
sys.path += include_dirs

# Add the following to `extra_compile_args` to debug packet parser:
#
#     '-DVERBOSE_STATES'
cy_config = dict(include_dirs=include_dirs, language='c++',
                 extra_compile_args=['-O3', '-Wfatal-errors'])

print '[nadamq] sources: %s' % nadamq.get_sources()
cy_exts = [Extension('nadamq.%s' % v, nadamq.get_sources()
                     + ['nadamq/%s.pyx' % v], **cy_config)
           for v in ('NadaMq', )]
extensions = cythonize(cy_exts)
pprint(extensions)

setup(name='nadamq',
      version=version.getVersion(),
      description='Embedded-friendly transport layer, inspired by ZeroMQ',
      keywords='cython embedded zeromq transport packet parse',
      author='Christian Fobel',
      url='https://github.com/cfobel/nadamq',
      license='GPL',
      packages=['nadamq', ],
      package_data={'nadamq': ['*.p??', 'src/*.h', 'src/*.c*']},
      ext_modules=extensions)


@task
def build_arduino_library():
    import os
    import tarfile
    import nadamq

    tf = tarfile.TarFile.bz2open('build/NadaMQ-Arduino.tar.gz', 'w')
    for s in nadamq.get_arduino_library_sources():
        tf.add(s, os.path.join('NadaMQ', os.path.basename(s)))
    tf.close()


@task
def copy_packet_actions():
    path('build/packet_actions.cpp').copy('nadamq/src')


@task
@needs(['copy_packet_actions', 'generate_setup', 'minilib',
        'setuptools.command.build'])
def build():
    pass


@task
@needs(['copy_packet_actions', 'generate_setup', 'minilib',
        'setuptools.command.build_ext'])
def build_ext():
    pass


@task
@needs('copy_packet_actions', 'generate_setup', 'minilib',
       'setuptools.command.bdist')
def bdist():
    """Overrides bdist to make sure that our setup.py is generated."""
    pass


@task
@needs('copy_packet_actions', 'generate_setup', 'minilib', 'build_ext',
       # 'build_firmware',
       'build_arduino_library', 'setuptools.command.sdist')
def sdist():
    """Overrides sdist to make sure that our setup.py is generated."""
    pass
