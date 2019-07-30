#!/usr/bin/env python3

#
# Benefit of this script:
#   remove unused source files.
#   make cscope reference essential.
#
# How is this script work:
#   After building the kernel, neccessary source files are *accessed*.
#   So their *access* time will be newer than *change* time.
#   This script delete the source files whose *access* time and *change* time
#   are same, after building.
#
# How to use this:
#   tar xf linux-5.0.tar.xz
#   mkdir build-5.0
#   cd linux-5.0
#   make O=../build-5.0 x86_64_defconfig
#   make O=../build-5.0 -j 6
#   python3 <this-script>
#

import os

whitelist = [ "scripts/tags.sh" ]
PWD = os.getcwd()

realwhitelist = [ PWD + "/" + f for f in whitelist ]

for root, dirs, files in os.walk(PWD):
    for f in files:
        file = root + "/" + f
        statinfo = os.stat(file, follow_symlinks=False)
        if (statinfo.st_atime > statinfo.st_ctime):
            print("file accessed during build, keep it : " + file)
        else:
            if (file in realwhitelist):
                print("file is in whitelist, keep it : " + file)
                continue
            print("==== removing: " + file)
            os.unlink(file)

