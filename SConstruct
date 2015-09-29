import c_array_defs

env = Environment(CPPPATH=[Dir('#/nadamq/src').abspath] +
                  c_array_defs.get_includes(), CCFLAGS=['-std=c++0x'])

SConscript('nadamq/src/SConscript', variant_dir='build', exports={'env': env})
