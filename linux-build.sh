#!/bin/sh

[ -d ./m4 ] || mkdir m4
[ -f ./configure ] || autoreconf -f -i

installPrefix="/game"
##mkdir -p "${installPrefix}"

if [ ! -f src/Makefile ]
then
        ## CXX="/usr/bin/g++ -fsanitize=address -fno-omit-frame-pointer" \
        ./configure --prefix="${installPrefix}" --with-allegro4 #--enable-debug=yes
fi

installPath=`pwd`/_rootdir
rm -r -f "${installPath}"

make && make install DESTDIR="${installPath}"
