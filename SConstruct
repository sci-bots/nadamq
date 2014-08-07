env = Environment(CPPPATH=Dir('#/nadamq/src').abspath, CCFLAGS=['-std=c++0x'])

SConscript('nadamq/src/SConscript', variant_dir='build', exports={'env': env})
