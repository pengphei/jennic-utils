import os

Import('env')

inc_path = ['#']
dst = "jennic-ftdi"
srcs = env.Glob('*.c')
libs = ['ftdi', 'rt']

jennic_ftdi = env.Program(target=dst, source=srcs, LIBS=libs, CPPPATH=inc_path)
Return("jennic_ftdi")
