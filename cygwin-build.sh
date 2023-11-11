#!/bin/sh

buildFolder=`pwd`

if [ -x "$( command -v ccache )" ]; then
    export CC="ccache $( command -v gcc )"
    export CXX="ccache $( command -v g++ )"
fi

echo

pathToExternal="${buildFolder}"/external
echo "path to external packages is" "${pathToExternal}"

echo
echo "   allegro 4"
echo "   cross-platform library for video game and multimedia programming"
echo

allegro4version="4.4.3.1"
allegro4name=allegro-"$allegro4version"
allegro4installpath=/usr

if [ -x "$( command -v cmake )" ]; then

    cd "${pathToExternal}"/allegro
    [ -d "$allegro4name" ] || tar xzf "$allegro4name".tar.gz

    cd "$allegro4name"

    if [ ! -f ./okay ]; then

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

        cd ..

        allegro4InstalledLibs=`find "${allegro4installpath}"/lib -name "lib*alleg*" -type f -print 2>/dev/null`
        num_allegro4s=`printf "${allegro4InstalledLibs}" | wc -l`
        if [ $num_allegro4s -gt 0 ]; then
            echo
            for theliballeg in $allegro4InstalledLibs; do
                file -h "${theliballeg}"
            done

            echo "okay" > ./okay
        fi

        [ -f ./okay ] && echo && find "${allegro4installpath}"/include -type f -print

        echo
        printf "allegro 4 "
        if [ -f ./okay ]; then
            echo "okay"
        else
            echo "nope"
        fi

    else

        echo "already built"

    fi

else
    echo "can’t build allegro 4"
    echo "it needs cmake to be built"
fi

echo
echo "   Head over Heels"
echo

cd "${buildFolder}"

[ -d ./m4 ] || mkdir m4
[ -f ./configure ] || autoreconf -f -i

gameInstallPath="${installPath}"

[ -f src/Makefile ] && rm -v src/Makefile
echo

./configure \
        --prefix="${gameInstallPath}" \
        --with-allegro4 \
        ##--with-allegro-prefix="${allegro4installpath}"
        ##--enable-debug=yes

make && make install ##DESTDIR="${gameInstallPath}"
