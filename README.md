# Head over Heels

The free and open source remake of game

Originated from version 1.0.1 by Jorge Rodríguez Santos http://www.headoverheels2.com/drupal/en/

Build:

    autoreconf -f -i

    LDFLAGS="-L/opt/libpng-1.6.25/lib -L/opt/zlib-1.2.8/lib" \
      CC="/usr/bin/gcc-4.2" CXX="/usr/bin/g++-4.2" \
        ./configure --prefix=/game

    make && make install DESTDIR=`pwd`/headoverheelsroot

For external dependencies look at external/README.external

Collect application bundle for Mac OS X:

    export games="/Applications/Games"
    cp -r extras/Head\ over\ Heels.app "${games}"/
    mkdir -p "${games}"/Head\ over\ Heels.app/Contents/MacOS/
    mkdir -p "${games}"/Head\ over\ Heels.app/Contents/Resources/
    cp headoverheelsroot/game/bin/headoverheels "${games}"/Head\ over\ Heels.app/Contents/MacOS/
    cp -r headoverheelsroot/game/share/headoverheels/* "${games}"/Head\ over\ Heels.app/Contents/Resources/
