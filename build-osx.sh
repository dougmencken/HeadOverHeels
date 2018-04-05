#!/bin/sh

[ -d ./m4 ] || mkdir m4
[ -f ./configure ] || autoreconf -f -i

gccVersion="7.3p" # ="6.4"

installPrefix="/game"

pathToCompiler="/Developer/GCC/${gccVersion}/PowerPC/32bit" # ="/usr"

if [ ! -f src/Makefile ]
then
        LDFLAGS="-L/opt/tinyxml2-6.0.0/lib -L/opt/xerces-c-3.2.1/lib -L/opt/ogg-vorbis/lib -L/opt/libpng-1.6.34/lib -L/opt/zlib-1.2.8/lib -L${pathToCompiler}/lib" \
        CPPFLAGS="-I/opt/libpng-1.6.34/include -I/opt/tinyxml2-6.0.0/include -I/opt/xerces-c-3.2.1/include -I/opt/allegro-4.4.2/include" \
        CC="${pathToCompiler}/bin/gcc" CXX="${pathToCompiler}/bin/g++" \
        ./configure --prefix=${installPrefix} --enable-debug
fi

installPath=`pwd`/headoverheelsroot
rm -rf "${installPath}"

makeCOptions="-Wextra -Werror" # ="-Wall -Werror"

time make -j2 CFLAGS="${makeCOptions}" CXXFLAGS="${makeCOptions}" && echo && make install DESTDIR="${installPath}"

headoverheelsbin="${installPath}${installPrefix}"/bin/headoverheels
if [ -f "${headoverheelsbin}" ]
then
        install_name_tool -change liballeg.4.4.dylib /opt/allegro-4.4.2/lib/liballeg.4.4.dylib "${headoverheelsbin}"
        install_name_tool -change libtinyxml2.6.dylib /opt/tinyxml2-6.0.0/lib/libtinyxml2.6.dylib "${headoverheelsbin}"
        install_name_tool -change /usr/lib/libgcc_s.1.dylib "${pathToCompiler}"/lib/libgcc_s.1.dylib "${headoverheelsbin}"
        otool -L "${headoverheelsbin}"

        games="/Applications/Games"
        mkdir -p "${games}"
        rm -r -f "${games}"/Head\ over\ Heels.app
        cp -r extras/Head\ over\ Heels.app "${games}"/
        mkdir -p "${games}"/Head\ over\ Heels.app/Contents/MacOS/
        mkdir -p "${games}"/Head\ over\ Heels.app/Contents/Resources/
        cp headoverheelsroot/game/bin/headoverheels "${games}"/Head\ over\ Heels.app/Contents/MacOS/
        cp -r headoverheelsroot/game/share/headoverheels/* "${games}"/Head\ over\ Heels.app/Contents/Resources/
fi
