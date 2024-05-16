#!/bin/sh

RED='\033[1;31m'
GREEN='\033[1;32m'
BLUE='\033[1;34m'
NOCOLOR='\033[0m'

okay="${GREEN}ok${NOCOLOR}"
nope="${RED}nope${NOCOLOR}"
already="${BLUE}already built${NOCOLOR}"

buildFolder=`pwd`

if [ -x "$( command -v ccache )" ]; then
    export CC="ccache $( command -v gcc )"
    export CXX="ccache $( command -v g++ )"
fi

echo

pathToExternal="${buildFolder}"/external
echo "path to external packages is" "${pathToExternal}"

whereItGoes="_rootdir"
installPath="${buildFolder}"/"${whereItGoes}"
echo "install path is" "${installPath}"

echo
echo "    zlib"
echo "    Massively Spiffy Yet Delicately Unobtrusive Compression Library"
echo

zlib_version="1.3.1"
zlibname=zlib-"$zlib_version"
zlibinstallpath="${installPath}"/"$zlibname"

cd "${pathToExternal}"/zlib
##[ -d "$zlibname" ] && rm -r "$zlibname"
[ -d "$zlibname" ] || tar xzf "$zlibname".tar.gz
cd "$zlibname"

if [ ! -f ./okay ]; then

    ./configure --prefix="${zlibinstallpath}" --libdir="${zlibinstallpath}"/lib && make
    make install

    ./configure --prefix="${zlibinstallpath}" --libdir="${zlibinstallpath}"/lib --static && make
    make install

    zlibInstalledLibs=`find "${zlibinstallpath}"/lib -name "libz*" -type f -print 2>/dev/null`
    num_zlibs=`printf "${zlibInstalledLibs}" | wc -l`
    if [ $num_zlibs -gt 0 ]; then
        echo
        for thezlib in $zlibInstalledLibs; do
            file -h "${thezlib}"
        done

        echo "okay" > ./okay
    fi

    [ -f ./okay ] && echo && find "${zlibinstallpath}"/include -type f -print

    echo
    printf "zlib "
    if [ -f ./okay ]; then
        echo "$okay"
    else
        echo "$nope"
    fi

else

    echo "$already"

fi

echo
echo "    libpng"
echo "    Portable Network Graphics reference library which supports almost all PNG features"
echo

libpng_version="1.6.43"
libpngname=libpng-"$libpng_version"
libpnginstallpath="${installPath}"/"$libpngname"

cd "${pathToExternal}"/libpng
if [ ! -d "$libpngname" ]; then
    tar xzf "$libpngname".tar.gz
    cd "$libpngname"
    patch -p1 < ../no-known-incorrect-warnings.patch
    cd ..
fi

cd "$libpngname"

if [ ! -f ./okay ]; then

    if [ ! -f ./Makefile ]; then
        LDFLAGS="-L${zlibinstallpath}/lib" \
        CPPFLAGS="-I${zlibinstallpath}/include" \
        ./configure --prefix="${libpnginstallpath}" --enable-static --enable-shared --with-zlib-prefix="${zlibinstallpath}"
    fi
    make
    make install

    libpngInstalledLibs=`find "${libpnginstallpath}"/lib -name "libpng*" -type f -print 2>/dev/null`
    num_libpngs=`printf "${libpngInstalledLibs}" | wc -l`
    if [ $num_libpngs -gt 0 ]; then
        echo
        for thelibpng in $libpngInstalledLibs; do
            file -h "${thelibpng}"
        done

        echo "okay" > ./okay
    fi

    [ -f ./okay ] && echo && find "${libpnginstallpath}"/include -type f -print

    echo
    printf "libpng "
    if [ -f ./okay ]; then
        echo "$okay"
    else
        echo "$nope"
    fi

else

    echo "$already"

fi

echo
echo "    ogg-vorbis ( libogg, libvorbis, vorbis-tools )"
echo "    completely open, patent-free audio encoding and streaming technology"
echo

echo "    libogg"
echo

libogg_version="1.3.3"
liboggname=libogg-"$libogg_version"
libogginstallpath="${installPath}"/"$liboggname"

cd "${pathToExternal}"/ogg-vorbis
[ -d "$liboggname" ] || tar xzf "$liboggname".tar.gz
cd "$liboggname"

if [ ! -f ./okay ]; then

    if [ ! -f ./Makefile ]; then
        ./configure --prefix="${libogginstallpath}"
    fi
    make
    make install

    liboggInstalledLibs=`find "${libogginstallpath}"/lib -name "libogg*" -type f -print 2>/dev/null`
    num_liboggs=`printf "${liboggInstalledLibs}" | wc -l`
    if [ $num_liboggs -gt 0 ]; then
        echo
        for thelibogg in $liboggInstalledLibs; do
            file -h "${thelibogg}"
        done

        echo "okay" > ./okay
    fi

    [ -f ./okay ] && echo && find "${libogginstallpath}"/include -type f -print

    echo
    printf "libogg "
    if [ -f ./okay ]; then
        echo "$okay"
    else
        echo "$nope"
    fi

else

    echo "$already"

fi

echo
echo "    libvorbis"
echo

libvorbis_version="1.3.6"
libvorbisname=libvorbis-"$libvorbis_version"
libvorbisinstallpath="${installPath}"/"$libvorbisname"

cd "${pathToExternal}"/ogg-vorbis
[ -d "$libvorbisname" ] || tar xzf "$libvorbisname".tar.gz
cd "$libvorbisname"

if [ ! -f ./okay ]; then

    if [ ! -f ./Makefile ]; then
        ./configure --prefix="${libvorbisinstallpath}" --with-ogg="${libogginstallpath}" && make
    fi
    make install

    libvorbisInstalledLibs=`find "${libvorbisinstallpath}"/lib -name "libvorbis*" -type f -print 2>/dev/null`
    num_libvorbises=`printf "${libvorbisInstalledLibs}" | wc -l`
    if [ $num_libvorbises -gt 0 ]; then
        echo
        for thelibvorbis in $libvorbisInstalledLibs; do
            file -h "${thelibvorbis}"
        done

        echo "okay" > ./okay
    fi

    [ -f ./okay ] && echo && find "${libvorbisinstallpath}"/include -type f -print

    echo
    printf "libvorbis "
    if [ -f ./okay ]; then
        echo "$okay"
    else
        echo "$nope"
    fi

else

    echo "$already"

fi

#echo
#echo "    vorbis-tools"
#echo
#
#vorbistools_version="1.4.0"
#vorbistools_name=vorbis-tools-"$vorbistools_version"
#vorbistoolsinstallpath="${installPath}"/"$vorbistools_name"
#
#cd "${pathToExternal}"/ogg-vorbis
#[ -d "$vorbistools_name" ] || tar xzf "$vorbistools_name".tar.gz
#cd "$vorbistools_name"
#
#if [ ! -f ./okay ]; then
#
#    if [ ! -f ./Makefile ]; then
#        ./configure \
#            --prefix="${vorbistoolsinstallpath}" \
#            --with-ogg-libraries="${libogginstallpath}"/lib \
#            --with-ogg-includes="${libogginstallpath}"/include \
#            --with-vorbis-libraries="${libvorbisinstallpath}"/lib \
#            --with-vorbis-includes="${libvorbisinstallpath}"/include \
#             && make
#    fi
#    make install
#
#    vorbistoolsInstalledBins=`find "${vorbistoolsinstallpath}"/bin -type f -print 2>/dev/null`
#    num_vorbistools=`printf "${vorbistoolsInstalledBins}" | wc -l`
#    if [ $num_vorbistools -gt 0 ]; then
#        echo
#        for thevorbistool in $vorbistoolsInstalledBins; do
#            file -h "${thevorbistool}"
#        done
#
#        echo "okay" > ./okay
#    fi
#
#    echo
#    printf "vorbis-tools "
#    if [ -f ./okay ]; then
#        echo "$okay"
#    else
#        echo "$nope"
#    fi
#
#else
#
#    echo "$already"
#
#fi

echo
echo "    tinyxml2"
echo "    simple, small, C++ XML parser that can be easily integrated into other programs"
echo

tinyxml2_version="6.2.0"
tinyxml2name=tinyxml2-"$tinyxml2_version"
tinyxml2installpath="${installPath}"/"$tinyxml2name"

if [ -x "$( command -v cmake )" ]; then

    cd "${pathToExternal}"/tinyxml2
    [ -d "$tinyxml2name" ] || tar xzf "$tinyxml2name".tar.gz
    cd "$tinyxml2name"

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
            cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH="${tinyxml2installpath}" \
                  -DBUILD_TESTS:BOOL=ON ..
        fi

        make

        make install
        rm -r "${tinyxml2installpath}"/lib/cmake

        cd "${tinyxml2installpath}"

        tinyxmlInstalledLibs=`find "${tinyxml2installpath}"/lib -name "*tinyxml*" -type f -print 2>/dev/null`
        num_tinyxmls=`printf "${tinyxmlInstalledLibs}" | wc -l`
        if [ $num_tinyxmls -gt 0 ]; then
            echo
            for thetinyxml in $tinyxmlInstalledLibs; do
                file -h "${thetinyxml}"
            done
        fi

        if [ -f ./include/tinyxml2.h ]; then
            mv ./include/tinyxml2.h ./ && rmdir ./include
            [ -f ./tinyxml2.h ] && echo "okay" > ./okay
        fi

        [ -f ./okay ] && echo && find "${tinyxml2installpath}" -name "*.h" -type f -print

        echo
        printf "tinyxml2 "
        if [ -f ./okay ]; then
            echo "$okay"
        else
            echo "$nope"
        fi

    else

        echo "$already"

    fi

else

    echo "can’t build tinyxml2"
    echo "${RED}it needs cmake to be built${NOCOLOR}"

fi

echo
echo "   allegro 5"
echo "   cross-platform library for video game and multimedia programming"
echo

allegro5version="5.2.9.1"
allegro5name=allegro-"$allegro5version"
allegro5installpath="${installPath}"/"$allegro5name"

if [ -x "$( command -v cmake )" ]; then

    cd "${pathToExternal}"/allegro
    if [ ! -d "$allegro5name" ]; then
        tar xzf "$allegro5name".tar.gz
    fi

    cd "$allegro5name"

    if [ ! -f ./okay ]; then

        [ -d TheBuild ] || mkdir TheBuild
        cd TheBuild

        if [ ! -f ./Makefile ]; then
            cmake -DCMAKE_BUILD_TYPE=Release \
                  -DCMAKE_INSTALL_PREFIX:PATH="${allegro5installpath}" \
                  -DCMAKE_LIBRARY_PATH="${zlibinstallpath}/lib;${libpnginstallpath}/lib;${libogginstallpath}/lib;${libvorbisinstallpath}/lib" \
                  -DPNG_PNG_INCLUDE_DIR:PATH="${libpnginstallpath}"/include \
                  -DOGG_INCLUDE_DIR="${libogginstallpath}"/include \
                  -DVORBIS_INCLUDE_DIR="${libvorbisinstallpath}"/include \
                  ..
        fi

        make && make install

        cd ..

        allegro5InstalledLibs=`find "${allegro5installpath}"/lib -name "liballegro*" -type f -print 2>/dev/null`
        num_allegro5s=`printf "${allegro5InstalledLibs}" | wc -l`
        if [ $num_allegro5s -gt 0 ]; then
            echo
            for theliballeg in $allegro5InstalledLibs; do
                file -h "${theliballeg}"
            done

            echo "okay" > ./okay
        fi

        [ -f ./okay ] && echo && find "${allegro5installpath}"/include -type f -print

        echo
        printf "allegro 5 "
        if [ -f ./okay ]; then
            echo "$okay"
        else
            echo "$nope"
        fi

    else

        echo "$already"

    fi

else
    echo "can’t build allegro 5"
    echo "${RED}it needs cmake to be built${NOCOLOR}"
fi

echo
echo "   Head over Heels"
echo

if [ ! -d "${allegro5installpath}/lib" ] ; then
        echo "${RED}allegro5 libraries are absent${NOCOLOR}"
        echo "can't go ahead"
        exit 1
fi

if [ ! -d "${allegro5installpath}/include" ] ; then
        echo "${RED}allegro5 headers are absent${NOCOLOR}"
        echo "can't go ahead"
        exit 2
fi

cd "${buildFolder}"

[ -d ./m4 ] || mkdir m4
[ -f ./configure ] || autoreconf -f -i

gameInstallPath="${installPath}"

## [ -f source/Makefile ] && rm -v source/Makefile

if [ ! -f source/Makefile ]; then

LDFLAGS="-Wl,-rpath ${zlibinstallpath}/lib -Wl,-rpath ${libpnginstallpath}/lib -Wl,-rpath ${libogginstallpath}/lib -Wl,-rpath ${libvorbisinstallpath}/lib -Wl,-rpath ${tinyxml2installpath}/lib -Wl,-rpath ${allegro5installpath}/lib" \
./configure \
        --prefix="${gameInstallPath}" \
        --with-libpng-prefix="${libpnginstallpath}" \
        --with-ogg-prefix="${libogginstallpath}" \
        --with-vorbis-prefix="${libvorbisinstallpath}" \
        --with-tinyxml2-libs="${tinyxml2installpath}"/lib \
        --with-tinyxml2-includes="${tinyxml2installpath}" \
        --with-allegro5 \
        --with-allegro-libs="${allegro5installpath}"/lib \
        --with-allegro-includes="${allegro5installpath}"/include
        ##--enable-debug=yes
fi

make && make install ##DESTDIR="${gameInstallPath}"
