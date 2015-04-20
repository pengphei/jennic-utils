#
# Copyright (C) 2015 Focalcrest, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import os

env = Environment()
#EXE_PATH = '../loftq-build/toolschain/gcc-linaro/bin'
#PREFIX = '../loftq-build/toolschain/gcc-linaro/bin/arm-linux-gnueabi-'
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
