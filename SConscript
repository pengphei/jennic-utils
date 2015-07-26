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

Import('env')

inc = ['#']
dst = "jennic-ctrl"
src = env.Glob('*.c')
lib = []

if env["platform"] == "pc":
    env["CCFLAGS"] = "-O2 -DJENNIC_FTDI_ENABLE"
    env["LDFLAGS"] = "-static"
    lib = ['ftdi']

jennic_ctrl = env.Program(target=dst, source=src, LIBS=lib, CPPPATH=inc)
Return("jennic_ctrl")
