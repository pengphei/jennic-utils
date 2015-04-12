import os

Import('env')

inc_path = ['#']
dst = "jennic-ctrl"
srcs = env.Glob('*.c')
libs = ['ftdi', 'rt']

jennic_ctrl = env.Program(target=dst, source=srcs, LIBS=libs, CPPPATH=inc_path)
Return("jennic_ctrl")
