#/bin/sh

pathToExternal=`pwd`/`dirname $0`
echo "path to external packages is ${pathToExternal}"

if [ ! `whoami` = root ];
then
        echo "I need root permissions for make install"
        exit
fi

echo
echo "    zlib"
echo "    a Massively Spiffy Yet Delicately Unobtrusive Compression Library"
echo

cd "${pathToExternal}"/zlib
tar xzf zlib-1.2.8.tar.gz
cd zlib-1.2.8

./configure --prefix=/opt/zlib-1.2.8 --libdir=/opt/zlib-1.2.8/lib --archs="" && make
make install DESTDIR=`pwd`/root32dir
CC="/usr/bin/gcc-4.2 -arch ppc64" CXX="/usr/bin/g++-4.2 -arch ppc64" ./configure --prefix=/opt/zlib-1.2.8 --libdir=/opt/zlib-1.2.8/lib --archs="" && make
make install DESTDIR=`pwd`/root64dir
lipo -create root32dir/opt/zlib-1.2.8/lib/libz.1.2.8.dylib root64dir/opt/zlib-1.2.8/lib/libz.1.2.8.dylib -output ./libz.1.2.8.dylib
make install

./configure --prefix=/opt/zlib-1.2.8 --libdir=/opt/zlib-1.2.8/lib --archs="" --static && make
make install DESTDIR=`pwd`/root32dir
CC="/usr/bin/gcc-4.2 -arch ppc64" CXX="/usr/bin/g++-4.2 -arch ppc64" ./configure --prefix=/opt/zlib-1.2.8 --libdir=/opt/zlib-1.2.8/lib --archs="" --static && make
make install DESTDIR=`pwd`/root64dir
lipo -create root32dir/opt/zlib-1.2.8/lib/libz.a root64dir/opt/zlib-1.2.8/lib/libz.a -output ./libz.a
make install

file -h /opt/zlib-1.2.8/lib/libz*

echo
echo "    libpng"
echo "    a Portable Network Graphics reference library which supports almost all PNG features"
echo

cd "${pathToExternal}"/libpng
tar xzf libpng-1.6.34.tar.gz
cd libpng-1.6.34

LDFLAGS="-L/opt/zlib-1.2.8/lib" ./configure --prefix=/opt/libpng-1.6.34 --enable-static --enable-shared --with-zlib-prefix=/opt/zlib-1.2.8
make
make install

cd "${pathToExternal}"

echo
echo "    ogg-vorbis ( libogg, libvorbis, vorbis-tools )"
echo "    a completely open, patent-free audio encoding and streaming technology"
echo

echo "    libogg"
echo

cd "${pathToExternal}"/ogg-vorbis
tar xzf libogg-1.3.3.tar.gz
cd libogg-1.3.3

./configure --prefix=/opt/ogg-vorbis && make
make install

rm -r -f /usr/include/ogg
ln -s /opt/ogg-vorbis/include/ogg /usr/include/ogg

echo
echo "    libvorbis"
echo

cd "${pathToExternal}"/ogg-vorbis
tar xzf libvorbis-1.3.5.tar.gz
cd libvorbis-1.3.5

./configure --prefix=/opt/ogg-vorbis --with-ogg=/opt/ogg-vorbis && make
make install

rm -f -r /usr/include/vorbis
ln -s /opt/ogg-vorbis/include/vorbis /usr/include/vorbis

echo
echo "    vorbis-tools"
echo

cd "${pathToExternal}"/ogg-vorbis
tar xzf vorbis-tools-1.4.0.tar.gz
cd vorbis-tools-1.4.0

./configure --prefix=/opt/ogg-vorbis --with-ogg=/opt/ogg-vorbis --with-vorbis=/opt/ogg-vorbis && make
make install

echo
echo "    xerces-c"
echo "    a validating XML parser written in a portable subset of C++"
echo

cd "${pathToExternal}"/xerces-c
tar xjf xerces-c-3.2.0.tar.bz2
cd xerces-c-3.2.0

CC="/usr/bin/gcc-4.2" CXX="/usr/bin/g++-4.2" ./configure --prefix=/opt/xerces-c-3.2.0
make
make install

echo
echo "    allegro 4"
echo "    a cross-platform library for video game and multimedia programming"
echo

cd "${pathToExternal}"/allegro
tar xzf allegro-4.4.2.tar.gz
cd allegro-4.4.2

mkdir TheBuild && cd TheBuild
export CC="/usr/bin/gcc-4.2" CXX="/usr/bin/g++-4.2"
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=/opt/allegro-4.4.2 ..
make
mkdir -p docs/txt ; cp ./docs/build/* ./docs/txt/ && make install

rm -f /usr/bin/allegro-config
ln -s /opt/allegro-4.4.2/bin/allegro-config /usr/bin/allegro-config

echo
echo "    the end"
