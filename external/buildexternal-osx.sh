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

zlib_version="1.2.8"

cd "${pathToExternal}"/zlib
[ -d "zlib-$zlib_version" ] || tar xzf zlib-"$zlib_version".tar.gz
cd zlib-"$zlib_version"

if [ ! -f ./okay ]; then

./configure --prefix=/opt/zlib-"$zlib_version" --libdir=/opt/zlib-"$zlib_version"/lib --archs="" && make
make install

./configure --prefix=/opt/zlib-"$zlib_version" --libdir=/opt/zlib-"$zlib_version"/lib --archs="" --static && make
make install

file -h /opt/zlib-"$zlib_version"/lib/libz*
echo "okay" > ./okay

else

echo "already built"

fi

echo
echo "    libpng"
echo "    a Portable Network Graphics reference library which supports almost all PNG features"
echo

libpng_version="1.6.34"

cd "${pathToExternal}"/libpng
[ -d libpng-"$libpng_version" ] || tar xzf libpng-"$libpng_version".tar.gz
cd libpng-"$libpng_version"

if [ ! -f ./okay ]; then

if [ ! -f ./Makefile ]; then
    LDFLAGS="-L/opt/zlib-$zlib_version/lib" ./configure --prefix=/opt/libpng-"$libpng_version" --enable-static --enable-shared --with-zlib-prefix=/opt/zlib-"$zlib_version"
fi
make
make install

echo "okay" > ./okay

else

echo "already built"

fi

echo
echo "    ogg-vorbis ( libogg, libvorbis, vorbis-tools )"
echo "    a completely open, patent-free audio encoding and streaming technology"
echo

echo "    libogg"
echo

libogg_version="1.3.3"

cd "${pathToExternal}"/ogg-vorbis
[ -d libogg-"$libogg_version" ] || tar xzf libogg-"$libogg_version".tar.gz
cd libogg-"$libogg_version"

if [ ! -f ./okay ]; then

if [ ! -f ./Makefile ]; then
    ./configure --prefix=/opt/ogg-vorbis && make
fi
make install

rm -r -f /usr/include/ogg
ln -s /opt/ogg-vorbis/include/ogg /usr/include/ogg

echo "okay" > ./okay

else

echo "already built"

fi

echo
echo "    libvorbis"
echo

libvorbis_version="1.3.5"

cd "${pathToExternal}"/ogg-vorbis
[ -d libvorbis-"$libvorbis_version" ] || tar xzf libvorbis-"$libvorbis_version".tar.gz
cd libvorbis-"$libvorbis_version"

if [ ! -f ./okay ]; then

if [ ! -f ./Makefile ]; then
    ./configure --prefix=/opt/ogg-vorbis --with-ogg=/opt/ogg-vorbis && make
fi
make install

rm -f -r /usr/include/vorbis
ln -s /opt/ogg-vorbis/include/vorbis /usr/include/vorbis

echo "okay" > ./okay

else

echo "already built"

fi

echo
echo "    vorbis-tools"
echo

vorbistools_version="1.4.0"

cd "${pathToExternal}"/ogg-vorbis
[ -d vorbis-tools-"$vorbistools_version" ] || tar xzf vorbis-tools-"$vorbistools_version".tar.gz
cd vorbis-tools-"$vorbistools_version"

if [ ! -f ./okay ]; then

if [ ! -f ./Makefile ]; then
    ./configure --prefix=/opt/ogg-vorbis --with-ogg=/opt/ogg-vorbis --with-vorbis=/opt/ogg-vorbis && make
fi
make install

echo "okay" > ./okay

else

echo "already built"

fi

echo
echo "    xerces-c"
echo "    a validating XML parser written in a portable subset of C++"
echo

xercesc_version="3.2.1"

cd "${pathToExternal}"/xerces-c
if [ ! -d xerces-c-"$xercesc_version" ]; then
    tar xjf xerces-c-"$xercesc_version".tar.bz2
    cd xerces-c-"$xercesc_version"
    patch -p1 < ../gcc_diagnostic.patch
    patch -p1 < ../explicitly-export-ArrayJanitor-templates.patch
    cd ..
fi
cd xerces-c-"$xercesc_version"

if [ ! -f ./okay ]; then

if [ ! -f ./Makefile ]; then
    ./configure --prefix=/opt/xerces-c-"$xercesc_version"
fi
make
make install

echo "okay" > ./okay

else

echo "already built"

fi

echo
echo "    allegro 4"
echo "    a cross-platform library for video game and multimedia programming"
echo

allegro4_version="4.4.2"

if [ -x "$( command -v cmake )" ]; then

    cd "${pathToExternal}"/allegro
    [ -d allegro-"$allegro4_version" ] || tar xzf allegro-"$allegro4_version".tar.gz
    cd allegro-"$allegro4_version"

    if [ ! -f ./okay ]; then

    [ -d TheBuild ] || mkdir TheBuild

    cd TheBuild

    if [ ! -f ./Makefile ]; then
        export CC=`command -v gcc`
        export CXX=`command -v g++`
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=/opt/allegro-"$allegro4_version" -DCMAKE_C_FLAGS:STRING="-fno-common" ..
    fi

    make

    mkdir -p docs/txt ; cp ./docs/build/* ./docs/txt/ && make install

    cd ..

    rm -f /usr/bin/allegro-config
    ln -s /opt/allegro-"$allegro4_version"/bin/allegro-config /usr/bin/allegro-config

    echo "okay" > ./okay

    else

    echo "already built"

    fi

else

    echo "can’t build allegro 4, it needs cmake to build"

fi

echo
echo "    the end"
