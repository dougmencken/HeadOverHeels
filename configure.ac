# Process this file with autoconf to get a configure script

AC_PREREQ([2.63])
AC_INIT(headoverheels, 1.25, dougmencken@gmail.com)
AM_INIT_AUTOMAKE(headoverheels, 1.25)

# Checks for programs
AC_PROG_CC
AC_PROG_CXX

# Checks for libraries

# Checks for header files
AC_CHECK_HEADERS(allegro.h,, [AC_MSG_ERROR(allegro headers not found)])
AC_CHECK_HEADERS(zlib.h,, [AC_MSG_ERROR(zlib headers not found)])
AC_CHECK_HEADERS(png.h,, [AC_MSG_ERROR(libpng headers not found)])
AC_CHECK_HEADERS(vorbis/codec.h vorbis/vorbisenc.h vorbis/vorbisfile.h,, [AC_MSG_ERROR(vorbis headers not found)])
AC_CHECK_HEADERS(ogg/ogg.h,, [AC_MSG_ERROR(ogg headers not found)])
AC_CHECK_HEADERS(xercesc/util/XercesVersion.hpp,, [AC_MSG_ERROR(xerces-c headers not found)])
AC_CHECK_HEADERS(pthread.h,, [AC_MSG_ERROR(pthread.h not found)])

# Checks for typedefs, structures, and compiler characteristics

# Checks for library functions

# Makefiles
AC_OUTPUT( \
            Makefile \
            src/Makefile \
            gamedata/Makefile \
            gamedata/gfx/Makefile \
            gamedata/gfx/blacktooth.png/Makefile \
            gamedata/gfx/byblos.png/Makefile \
            gamedata/gfx/egyptus.png/Makefile \
            gamedata/gfx/gui.png/Makefile \
            gamedata/gfx/item.png/Makefile \
            gamedata/gfx/jail.png/Makefile \
            gamedata/gfx/market.png/Makefile \
            gamedata/gfx/themoon.png/Makefile \
            gamedata/gfx/penitentiary.png/Makefile \
            gamedata/gfx/safari.png/Makefile \
            gamedata/gfx.2009/Makefile \
            gamedata/gfx.2009/blacktooth.png/Makefile \
            gamedata/gfx.2009/byblos.png/Makefile \
            gamedata/gfx.2009/egyptus.png/Makefile \
            gamedata/gfx.2009/gui.png/Makefile \
            gamedata/gfx.2009/item.png/Makefile \
            gamedata/gfx.2009/jail.png/Makefile \
            gamedata/gfx.2009/market.png/Makefile \
            gamedata/gfx.2009/themoon.png/Makefile \
            gamedata/gfx.2009/penitentiary.png/Makefile \
            gamedata/gfx.2009/safari.png/Makefile \
            gamedata/gfx.riderx/Makefile \
            gamedata/gfx.riderx/blacktooth.png/Makefile \
            gamedata/gfx.riderx/byblos.png/Makefile \
            gamedata/gfx.riderx/egyptus.png/Makefile \
            gamedata/gfx.riderx/gui.png/Makefile \
            gamedata/gfx.riderx/item.png/Makefile \
            gamedata/gfx.riderx/jail.png/Makefile \
            gamedata/gfx.riderx/market.png/Makefile \
            gamedata/gfx.riderx/themoon.png/Makefile \
            gamedata/gfx.riderx/penitentiary.png/Makefile \
            gamedata/gfx.riderx/safari.png/Makefile \
            gamedata/gfx.2003/Makefile \
            gamedata/gfx.2003/blacktooth.png/Makefile \
            gamedata/gfx.2003/byblos.png/Makefile \
            gamedata/gfx.2003/egyptus.png/Makefile \
            gamedata/gfx.2003/gui.png/Makefile \
            gamedata/gfx.2003/item.png/Makefile \
            gamedata/gfx.2003/jail.png/Makefile \
            gamedata/gfx.2003/market.png/Makefile \
            gamedata/gfx.2003/themoon.png/Makefile \
            gamedata/gfx.2003/penitentiary.png/Makefile \
            gamedata/gfx.2003/safari.png/Makefile \
            gamedata/map/Makefile \
            gamedata/map/blacktooth/Makefile \
            gamedata/map/byblos/Makefile \
            gamedata/map/egyptus/Makefile \
            gamedata/map/themoon/Makefile \
            gamedata/map/penitentiary/Makefile \
            gamedata/map/safari/Makefile \
            gamedata/music/Makefile \
            gamedata/savegame/Makefile \
            gamedata/sound/Makefile \
            gamedata/text/Makefile \
         )
