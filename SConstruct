import os

# arm env

env = Environment()

EXE_PATH = ''
PREFIX = ''
env["CC"] = PREFIX + 'gcc'    
env["AS"] = PREFIX + 'gcc'    
env["AR"] = PREFIX + 'ar'    
env["LINK"] = PREFIX + 'gcc' 
env["CCFLAGS"] = "-Wall -O2"
env["LDFLAGS"] = "-static"
env.PrependENVPath ('PATH', EXE_PATH)

Export("env")

zigbee_script = '#SConscript'
zigbee_files = SConscript(zigbee_script, variant_dir="build")
