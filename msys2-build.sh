#!/bin/sh

# get all the stuff for building

pacman -S --noconfirm --needed base-devel msys2-devel
pacman -S --noconfirm --needed binutils make autoconf automake libtool
pacman -S --noconfirm --needed mingw-w64-ucrt-x86_64-pkg-config

echo
which gcc && gcc --version

# get the dependencies

pacman -S --noconfirm --needed mingw-w64-ucrt-x86_64-allegro
pacman -S --noconfirm --needed mingw-w64-ucrt-x86_64-tinyxml2

echo

# change the Windows-specific direct.h to the POSIX-specific unistd.h inside the allegro headers

sed -i 's;direct\.h;unistd.h;' /ucrt64/include/allegro5/platform/almngw32.h

cat /ucrt64/include/allegro5/platform/almngw32.h | grep unistd\.h

[ -d ./m4 ] || mkdir m4

# autoreconf creates the configure script from configure.ac and Makefile.in from Makefile.am

[ -f ./configure -a -f source/Makefile.in ] || autoreconf -f -i

# the following fixes " implicit declaration of function '_spawnv'; did you mean 'spawnv'? "
# which comes from libtool

sed -i 's/_spawnv/spawnv/' ./ltmain.sh

export CPPFLAGS="-D_XOPEN_SOURCE=600" ; echo $CPPFLAGS

echo

# and then the configure script produces Makefile from Makefile.in

INSTALL_PREFIX=/hoh

DEPS_PREFIX=/ucrt64

./configure --prefix=${INSTALL_PREFIX} \
     --with-allegro5 --with-allegro-prefix=${DEPS_PREFIX} \
	 --with-libpng-prefix=${DEPS_PREFIX} --with-ogg-prefix=${DEPS_PREFIX} --with-tinyxml2-prefix=${DEPS_PREFIX}

[ -f ./libtool ] && sed -i 's/_spawnv/spawnv/' ./libtool ; grep spawnv ./libtool

echo

# now make it

make && make install

# after installing copy all shared libraries used by the game binary

echo

cp -v /bin/msys-2.0.dll ${INSTALL_PREFIX}/bin/

cp -v /bin/msys-gcc_s-seh-1.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/libgcc_s_seh-1.dll ${INSTALL_PREFIX}/bin/
cp -v /bin/msys-stdc++-6.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/libstdc++-6.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/libwinpthread-1.dll ${INSTALL_PREFIX}/bin/

cp -v ${DEPS_PREFIX}/bin/libogg-*.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/libvorbis-*.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/libvorbisfile-*.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/libopus-*.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/libopusfile-*.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/libFLAC.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/libdumb.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/libopenal-*.dll ${INSTALL_PREFIX}/bin/

cp -v ${DEPS_PREFIX}/bin/libwebp-*.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/libsharpyuv-*.dll ${INSTALL_PREFIX}/bin/

cp -v ${DEPS_PREFIX}/bin/allegro-*.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/allegro_acodec-*.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/allegro_audio-*.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/allegro_font-*.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/allegro_image-*.dll ${INSTALL_PREFIX}/bin/
cp -v ${DEPS_PREFIX}/bin/allegro_primitives-*.dll ${INSTALL_PREFIX}/bin/

cp -v ${DEPS_PREFIX}/bin/libtinyxml2.dll ${INSTALL_PREFIX}/bin/
