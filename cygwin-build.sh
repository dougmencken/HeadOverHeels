#!/bin/sh

[ -d ./m4 ] || mkdir m4
[ -f ./configure ] || autoreconf -f -i

installPrefix="/game"

pathToCompiler="/usr"

if [ ! -f src/Makefile ]
then
        CC="${pathToCompiler}/bin/gcc" CXX="${pathToCompiler}/bin/g++ -std=c++03" \
        ./configure --prefix=${installPrefix}
fi

installPath=`pwd`/_rootdir
rm -rf "${installPath}"

make -j2 CFLAGS=-Werror CXXFLAGS=-Werror && make install DESTDIR="${installPath}"
