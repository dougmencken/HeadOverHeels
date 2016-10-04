# HeadOverHeels remake

http://www.headoverheels2.com/drupal/en/gallery/20

Originated from version 1.0.1 by Jorge Rodríguez Santos < jorge@helmantika.org >

Build:

    autoreconf -f -i
    CC="/usr/bin/gcc-4.2" CXX="/usr/bin/g++-4.2" ./configure --prefix=/usr
    make && make install DESTDIR=`pwd`/headoverheelsroot

For external dependencies look at external/README.external

Collect application bundle for Mac OS X:

    export games="/Applications/Games"
    cp -r extras/Head\ over\ Heels.app "${games}"/
    cp headoverheelsroot/usr/bin/headoverheels "${games}"/Head\ over\ Heels.app/Contents/MacOS/
    cp -r headoverheelsroot/usr/share/headoverheels/* "${games}"/Head\ over\ Heels.app/Contents/Resources/
