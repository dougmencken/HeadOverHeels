#!/bin/sh

[ -d ./m4 ] || mkdir m4
[ -f ./configure ] || autoreconf -f -i

installPrefix="/game"

if [ ! -f source/Makefile ]
then
        ## CXX="/usr/bin/g++ -fsanitize=address -fno-omit-frame-pointer" \
        ./configure --prefix=${installPrefix} --enable-debug=yes
fi

installPath=`pwd`/_rootdir
rm -rf "${installPath}"

make && make install DESTDIR="${installPath}"
