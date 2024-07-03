#!/bin/sh

gccVersion="7.3p"

games="/Applications/Games"
headoverheelsbin="${games}"/"Head over Heels".app/Contents/MacOS/headoverheels
headoverheelslibs="${games}"/"Head over Heels".app/Contents/Libraries
mkdir -p "${headoverheelslibs}"

cp /opt/ogg-vorbis/lib/libvorbis.0.dylib "${headoverheelslibs}"/
cp /opt/ogg-vorbis/lib/libvorbisfile.3.dylib "${headoverheelslibs}"/
cp /opt/ogg-vorbis/lib/libvorbisenc.2.dylib "${headoverheelslibs}"/
cp /opt/ogg-vorbis/lib/libogg.0.dylib "${headoverheelslibs}"/
cp /opt/tinyxml2-10.0.0/lib/libtinyxml2.10.0.0.dylib "${headoverheelslibs}"/
cp /opt/libpng-1.6.43/lib/libpng16.16.dylib "${headoverheelslibs}"/
cp /opt/zlib-1.3.1/lib/libz.1.3.1.dylib "${headoverheelslibs}"/

install_name_tool -change /opt/zlib-1.3.1/lib/libz.1.dylib "@loader_path/libz.1.3.1.dylib" "${headoverheelslibs}"/libpng16.16.dylib

install_name_tool -change /opt/ogg-vorbis/lib/libogg.0.dylib "@loader_path/libogg.0.dylib" "${headoverheelslibs}"/libvorbis.0.dylib

install_name_tool -change /opt/ogg-vorbis/lib/libogg.0.dylib "@loader_path/libogg.0.dylib" "${headoverheelslibs}"/libvorbisenc.2.dylib
install_name_tool -change /opt/ogg-vorbis/lib/libvorbis.0.dylib "@loader_path/libvorbis.0.dylib" "${headoverheelslibs}"/libvorbisenc.2.dylib

install_name_tool -change /opt/ogg-vorbis/lib/libogg.0.dylib "@loader_path/libogg.0.dylib" "${headoverheelslibs}"/libvorbisfile.3.dylib
install_name_tool -change /opt/ogg-vorbis/lib/libvorbis.0.dylib "@loader_path/libvorbis.0.dylib" "${headoverheelslibs}"/libvorbisfile.3.dylib

otool -L "${headoverheelslibs}"/*.dylib

cp /Developer/GCC/${gccVersion}/PowerPC/32bit/lib/libstdc++.6.dylib "${headoverheelslibs}"/
cp /Developer/GCC/${gccVersion}/PowerPC/32bit/lib/libgcc_s.1.dylib "${headoverheelslibs}"/

install_name_tool -change /Developer/GCC/${gccVersion}/PowerPC/32bit/lib/libstdc++.6.dylib "@executable_path/../Libraries/libstdc++.6.dylib" "${headoverheelsbin}"
install_name_tool -change /Developer/GCC/${gccVersion}/PowerPC/32bit/lib/libgcc_s.1.dylib "@executable_path/../Libraries/libgcc_s.1.dylib" "${headoverheelsbin}"
## install_name_tool -change /usr/lib/libgcc_s.1.dylib "@executable_path/../Libraries/libgcc_s.1.dylib" "${headoverheelsbin}"

cp /opt/allegro-4.4.2/lib/liballeg.4.4.dylib "${headoverheelslibs}"/
install_name_tool -change /opt/allegro-4.4.2/lib/liballeg.4.4.dylib "@executable_path/../Libraries/liballeg.4.4.dylib" "${headoverheelsbin}"

install_name_tool -change /opt/libpng-1.6.43/lib/libpng16.16.dylib "@executable_path/../Libraries/libpng16.16.dylib" "${headoverheelsbin}"
install_name_tool -change /opt/ogg-vorbis/lib/libvorbisfile.3.dylib "@executable_path/../Libraries/libvorbisfile.3.dylib" "${headoverheelsbin}"
install_name_tool -change /opt/ogg-vorbis/lib/libvorbisenc.2.dylib "@executable_path/../Libraries/libvorbisenc.2.dylib" "${headoverheelsbin}"
install_name_tool -change /opt/ogg-vorbis/lib/libvorbis.0.dylib "@executable_path/../Libraries/libvorbis.0.dylib" "${headoverheelsbin}"
install_name_tool -change /opt/ogg-vorbis/lib/libogg.0.dylib "@executable_path/../Libraries/libogg.0.dylib" "${headoverheelsbin}"
install_name_tool -change /opt/tinyxml2-10.0.0/lib/libtinyxml2.6.dylib "@executable_path/../Libraries/libtinyxml2.10.0.0.dylib" "${headoverheelsbin}"
install_name_tool -change /opt/zlib-1.3.1/lib/libz.1.dylib "@executable_path/../Libraries/libz.1.3.1.dylib" "${headoverheelsbin}"

otool -L "${headoverheelsbin}"
