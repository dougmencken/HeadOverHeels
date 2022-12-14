#!/bin/sh

buildFolder=`pwd`

[ -d ./m4 ] || mkdir m4
[ -f ./configure ] || autoreconf -f -i

if [ -x "$( command -v ccache )" ]; then
    export CC="ccache $( command -v gcc )"
    export CXX="ccache $( command -v g++ )"
fi

allegro4version="4.4.3.1"
allegro4name=allegro-"$allegro4version"

pathToExternal="${buildFolder}"/external
echo
echo "path to external packages is ${pathToExternal}"

echo
echo "   allegro 4"
echo "   cross-platform library for video game and multimedia programming"
echo

allegro4installpath="${buildFolder}"/_rootdir/"${allegro4name}"

if [ -x "$( command -v cmake )" ]; then

    cd "${pathToExternal}"/allegro
    [ -d "${allegro4name}" ] && rm -r "${allegro4name}"
    tar xzf "${allegro4name}".tar.gz
    cd "${allegro4name}"
    patch -p1 < ../false_three_finger.patch

    [ -d TheBuild ] || mkdir TheBuild
    cd TheBuild

    if [ ! -f ./Makefile ]; then
        sed -i 's/add_subdirectory(docs)/##add_subdirectory(docs)/' ../CMakeLists.txt

        cmake -DCMAKE_BUILD_TYPE=Release \
              -DCMAKE_INSTALL_PREFIX:PATH="${allegro4installpath}" \
              -DCMAKE_C_FLAGS:STRING="-fno-common" \
              ..
    fi

    make && make install

else
    echo "can’t build allegro 4, it needs cmake to build"
fi

echo
echo "   Head over Heels"
echo

if [ ! -d "${allegro4installpath}/lib" ] ; then
        echo "allegro4 libraries are absent"
        echo "can't go ahead"
        exit 1
fi

if [ ! -d "${allegro4installpath}/include" ] ; then
        echo "allegro4 headers are absent"
        echo "can't go ahead"
        exit 2
fi

if [ -d "${allegro4installpath}/bin" ] ; then
        export PATH="${allegro4installpath}/bin":$PATH
else
        echo "allegro4 binaries are absent"
        echo "can't go ahead"
        exit 3
fi

cd "${buildFolder}"

gameInstallPath="${buildFolder}"/_rootdir

if [ ! -f src/Makefile ]
then
        LDFLAGS="-L${allegro4installpath}/lib" \
        CFLAGS="-I${allegro4installpath}/include" \
        CXXFLAGS="${CFLAGS}" \
        ./configure \
                --prefix="${gameInstallPath}" \
                --without-allegro
                ##--enable-debug=yes
fi

make && make install ##DESTDIR="${gameInstallPath}"
