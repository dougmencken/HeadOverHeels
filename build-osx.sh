#!/bin/sh

[ -d ./m4 ] || mkdir m4
[ -f ./configure ] || autoreconf -f -i

gccVersion="8.2p"

installPrefix="/game"

pathToCompiler="/Developer/GCC/${gccVersion}/PowerPC/32bit" # ="/usr"

a5path="/opt/allegro-5.0.11"
a4path="/opt/allegro-4.4.2"

export PATH="$a4path"/bin:$PATH

if [ ! -f source/Makefile ]
then
        LDFLAGS="-headerpad_max_install_names -L${a4path}/lib -L${a5path}/lib -L/opt/tinyxml2-10.0.0/lib -L/opt/ogg-vorbis/lib -L/opt/libpng-1.6.43/lib -L/opt/zlib-1.3.1/lib -L${pathToCompiler}/lib" \
        CPPFLAGS="-I/opt/zlib-1.3.1/include -I/opt/libpng-1.6.43/include -I/opt/tinyxml2-10.0.0/include -I/opt/ogg-vorbis/include -I${a4path}/include -I${a5path}/include" \
        CC="${pathToCompiler}/bin/gcc" CXX="${pathToCompiler}/bin/g++" \
        ./configure --prefix=${installPrefix} --enable-debug --with-allegro5 # --with-allegro4
fi

installPath=`pwd`/headoverheelsroot
rm -rf "${installPath}"

makeCOptions="-Wextra -Werror" # ="-Wall -Werror"

time make -j2 CFLAGS="${makeCOptions}" CXXFLAGS="${makeCOptions}" && echo && make install DESTDIR="${installPath}"

rm -f source/headoverheels

headoverheelsbin="${installPath}${installPrefix}"/bin/headoverheels
if [ -f "${headoverheelsbin}" ]
then
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

        if [ -x "$( command -v haxelib )" ]; then
                cd source
                rm -r -f obj
                haxelib run hxcpp hxcpp-build.xml
                [ -f headoverheels ] && mv headoverheels "${installPath}${installPrefix}"/bin/headoverheels.allegro4
                cd ..
        fi
fi
