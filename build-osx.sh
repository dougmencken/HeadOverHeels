#!/bin/sh

autoreconf -f -i

LDFLAGS="-L/opt/libpng-1.6.25/lib -L/opt/zlib-1.2.8/lib" \
  CC="/Developer/GCC/6.2/PowerPC/32bit/bin/gcc" CXX="/Developer/GCC/6.2/PowerPC/32bit/bin/g++ -std=c++03" \
  ./configure --prefix=/game

make && make install DESTDIR=`pwd`/headoverheelsroot

# export games="/Applications/Games"
# cp -r extras/Head\ over\ Heels.app "${games}"/
# cp headoverheelsroot/game/bin/headoverheels "${games}"/Head\ over\ Heels.app/Contents/MacOS/
# cp -r headoverheelsroot/game/share/headoverheels/* "${games}"/Head\ over\ Heels.app/Contents/Resources/
