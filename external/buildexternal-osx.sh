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
echo "    Massively Spiffy Yet Delicately Unobtrusive Compression Library"
echo

zlib_version="1.3.1"

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
echo "    Portable Network Graphics reference library which supports almost all PNG features"
echo

libpng_version="1.6.43"

cd "${pathToExternal}"/libpng
if [ ! -d libpng-"$libpng_version" ]; then
    tar xzf libpng-"$libpng_version".tar.gz
    cd libpng-"$libpng_version"
    patch -p1 < ../no-known-incorrect-warnings.patch
    cd ..
fi

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
echo "    completely open, patent-free audio encoding and streaming technology"
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

libvorbis_version="1.3.6"

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
echo "    tinyxml2"
echo "    simple, small, C++ XML parser that can be easily integrated into other programs"
echo

tinyxml2_version="10.0.0"

if [ -x "$( command -v cmake )" ]; then

    cd "${pathToExternal}"/tinyxml2
    [ -d tinyxml2-"$tinyxml2_version" ] || tar xzf tinyxml2-"$tinyxml2_version".tar.gz
    cd tinyxml2-"$tinyxml2_version"

    if [ ! -f ./okay ]; then

    if [ ! -f xmltest.cpp.orig ]; then
        mv xmltest.cpp xmltest.cpp.orig
        cat xmltest.cpp.orig | sed s/123456789012345678/123456789012345678LL/ > xmltest.cpp
    fi

    [ -d Boo ] || mkdir Boo

    cd Boo

    if [ ! -f ./Makefile ]; then
        export CC=`command -v gcc`
        export CXX=`command -v g++`
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=/opt/tinyxml2-"$tinyxml2_version" \
              ..
    fi

    make

    make install
    rm -r /opt/tinyxml2-"$tinyxml2_version"/lib/cmake

    cd ..

    install_name_tool -id /opt/tinyxml2-"$tinyxml2_version"/lib/libtinyxml2.6.dylib /opt/tinyxml2-"$tinyxml2_version"/lib/libtinyxml2.6.dylib

    echo "okay" > ./okay

    else

    echo "already built"

    fi

else

    echo "can’t build tinyxml2, it needs cmake to build"

fi

echo
echo "    allegro 4"
echo "    cross-platform library for video game and multimedia programming"
echo

allegro4_version="4.4.2"

if [ -x "$( command -v cmake )" ]; then

    cd "${pathToExternal}"/allegro
    if [ ! -d allegro-"$allegro4_version" ]; then
        tar xzf allegro-"$allegro4_version".tar.gz
        cd allegro-"$allegro4_version"
        patch -p1 < ../false_three_finger.patch
        cd ..
    fi

    cd allegro-"$allegro4_version"

    if [ ! -f ./okay ]; then

    [ -d TheBuild ] || mkdir TheBuild

    cd TheBuild

    if [ ! -f ./Makefile ]; then
        export CC=`command -v gcc`
        export CXX=`command -v g++`
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=/opt/allegro-"$allegro4_version" \
              -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=$(printf 10.%d `sw_vers -productVersion | cut -d . -f 2`) \
              -DCMAKE_LIBRARY_PATH="/opt/zlib-$zlib_version/lib;/opt/libpng-$libpng_version/lib" \
              -DPNG_PNG_INCLUDE_DIR="/opt/libpng-$libpng_version/include" \
              -DOGG_INCLUDE_DIR="/opt/ogg-vorbis/include" -DVORBIS_INCLUDE_DIR="/opt/ogg-vorbis/include" \
              -DCMAKE_C_FLAGS:STRING="-fno-common" \
              ..
    fi

    make

    mkdir -p docs/txt ; cp ./docs/build/* ./docs/txt/ && make install

    cd ..

    install_name_tool -id /opt/allegro-"$allegro4_version"/lib/liballeg.4.4.dylib /opt/allegro-"$allegro4_version"/lib/liballeg.4.4.dylib

    echo "okay" > ./okay

    else

    echo "already built"

    fi

else

    echo "can’t build allegro 4, it needs cmake to build"

fi

echo
echo "    allegro 5"
echo "    cross-platform library for video game and multimedia programming"
echo

allegro5_version="5.0.11"

if [ -x "$( command -v cmake )" ]; then

    cd "${pathToExternal}"/allegro
    if [ ! -d allegro-"$allegro5_version" ]; then
        tar xzf allegro-"$allegro5_version".tar.gz
        cd allegro-"$allegro5_version"
        patch -p1 < ../fix_wav_swapping.patch
        patch -p1 < ../aqueue_big_endian_format.patch
        patch -p1 < ../osxgl_10.4_compatibility.patch
        cd ..
    fi

    cd allegro-"$allegro5_version"

    if [ ! -f ./okay ]; then

    [ -d TheBuild ] || mkdir TheBuild

    cd TheBuild

    if [ ! -f ./Makefile ]; then
        export CC=`command -v gcc`
        export CXX=`command -v g++`
        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=/opt/allegro-"$allegro5_version" \
              -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=$(printf 10.%d `sw_vers -productVersion | cut -d . -f 2`) \
              -DCMAKE_LIBRARY_PATH="/opt/zlib-$zlib_version/lib;/opt/libpng-$libpng_version/lib;/opt/ogg-vorbis/lib" \
              -DWANT_NATIVE_IMAGE_LOADER:BOOL=OFF -DALLEGRO_CFG_WANT_NATIVE_IMAGE_LOADER=0 \
              -DWANT_IMAGE_JPG:BOOL=OFF \
              -DWANT_TTF:BOOL=OFF \
              -DPNG_PNG_INCLUDE_DIR="/opt/libpng-$libpng_version/include" \
              -DOGG_INCLUDE_DIR="/opt/ogg-vorbis/include" -DVORBIS_INCLUDE_DIR="/opt/ogg-vorbis/include" \
              ..
    fi

    make

    make install

    cd ..

    a5libs="/opt/allegro-$allegro5_version/lib"

    install_name_tool -id "$a5libs"/liballegro.5.0.dylib "$a5libs"/liballegro.5.0.dylib
    install_name_tool -id "$a5libs"/liballegro_audio.5.0.dylib "$a5libs"/liballegro_audio.5.0.dylib
    install_name_tool -id "$a5libs"/liballegro_acodec.5.0.dylib "$a5libs"/liballegro_acodec.5.0.dylib
    install_name_tool -id "$a5libs"/liballegro_color.5.0.dylib "$a5libs"/liballegro_color.5.0.dylib
    install_name_tool -id "$a5libs"/liballegro_dialog.5.0.dylib "$a5libs"/liballegro_dialog.5.0.dylib
    install_name_tool -id "$a5libs"/liballegro_font.5.0.dylib "$a5libs"/liballegro_font.5.0.dylib
    #install_name_tool -id "$a5libs"/liballegro_ttf.5.0.dylib "$a5libs"/liballegro_ttf.5.0.dylib
    install_name_tool -id "$a5libs"/liballegro_image.5.0.dylib "$a5libs"/liballegro_image.5.0.dylib
    install_name_tool -id "$a5libs"/liballegro_main.5.0.dylib "$a5libs"/liballegro_main.5.0.dylib
    install_name_tool -id "$a5libs"/liballegro_memfile.5.0.dylib "$a5libs"/liballegro_memfile.5.0.dylib
    install_name_tool -id "$a5libs"/liballegro_primitives.5.0.dylib "$a5libs"/liballegro_primitives.5.0.dylib

    install_name_tool -change liballegro.5.0.dylib "$a5libs"/liballegro.5.0.dylib "$a5libs"/liballegro_audio.5.0.dylib

    install_name_tool -change liballegro.5.0.dylib "$a5libs"/liballegro.5.0.dylib "$a5libs"/liballegro_acodec.5.0.dylib
    install_name_tool -change liballegro_audio.5.0.dylib "$a5libs"/liballegro_audio.5.0.dylib "$a5libs"/liballegro_acodec.5.0.dylib

    install_name_tool -change liballegro.5.0.dylib "$a5libs"/liballegro.5.0.dylib "$a5libs"/liballegro_color.5.0.dylib

    install_name_tool -change liballegro.5.0.dylib "$a5libs"/liballegro.5.0.dylib "$a5libs"/liballegro_dialog.5.0.dylib

    install_name_tool -change liballegro.5.0.dylib "$a5libs"/liballegro.5.0.dylib "$a5libs"/liballegro_font.5.0.dylib

    #install_name_tool -change liballegro.5.0.dylib "$a5libs"/liballegro.5.0.dylib "$a5libs"/liballegro_ttf.5.0.dylib
    #install_name_tool -change liballegro_font.5.0.dylib "$a5libs"/liballegro_font.5.0.dylib "$a5libs"/liballegro_ttf.5.0.dylib

    install_name_tool -change liballegro.5.0.dylib "$a5libs"/liballegro.5.0.dylib "$a5libs"/liballegro_image.5.0.dylib

    install_name_tool -change liballegro.5.0.dylib "$a5libs"/liballegro.5.0.dylib "$a5libs"/liballegro_main.5.0.dylib

    install_name_tool -change liballegro.5.0.dylib "$a5libs"/liballegro.5.0.dylib "$a5libs"/liballegro_memfile.5.0.dylib

    install_name_tool -change liballegro.5.0.dylib "$a5libs"/liballegro.5.0.dylib "$a5libs"/liballegro_primitives.5.0.dylib

    echo "okay" > ./okay

    else

    echo "already built"

    fi

else

    echo "can’t build allegro 5, it needs cmake to build"

fi

echo
echo "    the end"
