#!/bin/bash

set -e

ver=""
if [ "x$1" = "x" ]; then
    ver="5.2"
else
    ver=$1
fi

if [ ! -e linux-$ver.tar.xz ]; then
    wget https://mirrors.edge.kernel.org/pub/linux/kernel/v5.x/linux-$ver.tar.xz
fi

if [ ! -d linux-$ver ]; then
    tar xf linux-$ver.tar.xz
fi

if [ ! -d build-$ver ]; then
    mkdir build-$ver
fi

cd linux-$ver

if (( $(echo "$ver > 4.15" | bc -l) )); then
    echo "# CONFIG_RETPOLINE is not set" >> arch/x86/configs/x86_64_defconfig
fi

if [ ! -e ../build-$ver/.config ]; then
    make O=../build-$ver x86_64_defconfig
    make O=../build-$ver -j 6
fi

if [ ! -e file-extracted.txt ]; then
    python3 ../scripts/file-extract.py
    touch file-extracted.txt
fi

if [ ! -d .git ]; then
    git init
    git add .
    git commit -m "linux-$ver striped"
fi

if [ ! -e cscope.out ]; then
    make ARCH=x86_64 cscope
fi
