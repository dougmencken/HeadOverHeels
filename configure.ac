AC_PREREQ([2.65])
AC_LANG(C++)
AC_CONFIG_MACRO_DIRS([m4])

AC_INIT(headoverheels, 1.30, dougmencken@gmail.com)

AM_INIT_AUTOMAKE([subdir-objects])

# default output looks too verbose
AM_SILENT_RULES([yes])

# use libtool
LT_INIT

# look for programs
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O

# support for debug build
AC_ARG_ENABLE(debug,
        AS_HELP_STRING([--enable-debug], [make debug build]),
        [case "${enableval}" in
                     yes|true)  debug=true ; AC_MSG_NOTICE([debug build is on]) ;;
                     no|false)  debug=false ;;
                     *)  debug=false ; AC_MSG_WARN([unknown value ${enableval} for --enable-debug, assume no]) ;;
        esac],
        [debug=false])

AM_CONDITIONAL([DEBUG], [test x"$debug" = x"true"])
AC_SUBST([DEBUG])

# look for libraries

# look for header files
AC_CHECK_HEADERS([execinfo.h])
AC_CHECK_HEADERS(allegro.h,, [AC_MSG_ERROR(allegro headers not found)])
AC_CHECK_HEADERS(zlib.h,, [AC_MSG_ERROR(zlib headers not found)])
AC_CHECK_HEADERS(png.h,, [AC_MSG_ERROR(libpng headers not found)])
AC_CHECK_HEADERS(vorbis/codec.h vorbis/vorbisenc.h vorbis/vorbisfile.h,, [AC_MSG_ERROR(vorbis headers not found)])
AC_CHECK_HEADERS(ogg/ogg.h,, [AC_MSG_ERROR(ogg headers not found)])
AC_CHECK_HEADERS(xercesc/util/XercesVersion.hpp,, [AC_MSG_ERROR(xerces-c headers not found)])
AC_LANG_PUSH([C++]) # tinyxml2.h is C++ header
AC_CHECK_HEADERS(tinyxml2.h,, [AC_MSG_ERROR(tinyxml2 header not found)])
AC_LANG_POP([C++])
AC_CHECK_HEADERS(pthread.h,, [AC_MSG_ERROR(pthread.h not found)])

# look for typedefs, structures, and compiler characteristics

# look for library functions

# makefiles
AC_OUTPUT( \
            Makefile \
            src/Makefile \
            gamedata/Makefile \
            gamedata/gfx/Makefile \
            gamedata/gfx.2009/Makefile \
            gamedata/gfx.riderx/Makefile \
            gamedata/gfx.2003/Makefile \
            gamedata/gfx.simple/Makefile \
            gamedata/map/Makefile \
            gamedata/music/Makefile \
            gamedata/savegame/Makefile \
            gamedata/sound/Makefile \
            gamedata/text/Makefile \
         )
