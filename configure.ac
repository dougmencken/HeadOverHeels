AC_PREREQ([2.65])
AC_LANG(C++)
AC_CONFIG_MACRO_DIRS([m4])

AC_INIT(headoverheels, 1.4dev, dougmencken@gmail.com)

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
AC_ARG_ENABLE([debug],
        AS_HELP_STRING([--enable-debug], [make debug build]),
        [case "${enableval}" in
                     yes|true)  debug=true ; AC_MSG_NOTICE([debug build is on]) ;;
                     no|false)  debug=false ;;
                     *)  debug=false ; AC_MSG_WARN([unknown value ${enableval} for --enable-debug, assume no]) ;;
        esac],
        [debug=false])

AM_CONDITIONAL([DEBUG], [test x"$debug" = x"true"])
AC_SUBST([DEBUG])

# look for header files
AC_CHECK_HEADERS([pthread.h],, [AC_MSG_ERROR(pthread.h is absent)])
AC_CHECK_HEADERS([execinfo.h])
AC_CHECK_HEADERS([zlib.h],, [AC_MSG_ERROR(zlib headers are absent)])
AC_CHECK_HEADERS([png.h],, [AC_MSG_ERROR(libpng headers are absent)])
AC_CHECK_HEADERS([vorbis/codec.h vorbis/vorbisenc.h vorbis/vorbisfile.h],, [AC_MSG_ERROR(vorbis headers are absent)])
AC_CHECK_HEADERS([ogg/ogg.h],, [AC_MSG_ERROR(ogg headers are absent)])
AC_LANG_PUSH([C++]) # tinyxml2.h is C++ header
AC_CHECK_HEADERS([tinyxml2.h],, [AC_MSG_ERROR(tinyxml2 header is absent)])
AC_LANG_POP([C++])

AC_ARG_WITH([allegro5],
        [AS_HELP_STRING([--with-allegro5], [build the game with the allegro5 library (default: check)])], [], [with_allegro5=check])
AC_ARG_WITH([allegro4],
        [AS_HELP_STRING([--with-allegro4], [build the game with the allegro4 library (default: check)])], [], [with_allegro4=check])
AC_ARG_WITH([allegro],
        [AS_HELP_STRING([--without-allegro], [build the game without allegro libraries])], [], [with_allegro=yes])

if test x"$with_allegro4" = x"no" -a x"$with_allegro5" = x"no" -a x"$with_allegro" = x"yes" ; then
        AC_MSG_ERROR(use --with-allegro5 or --with-allegro4 or --without-allegro)
elif test x"$with_allegro4" = x"no" -a x"$with_allegro5" = x"yes" -a x"$with_allegro" = x"yes" ; then
        AC_MSG_NOTICE(building with allegro5)
elif test x"$with_allegro4" = x"yes" -a x"$with_allegro5" = x"no" -a x"$with_allegro" = x"yes" ; then
        AC_MSG_NOTICE(building with allegro4)
elif test x"$with_allegro4" = x"yes" -a x"$with_allegro5" = x"yes" -a x"$with_allegro" = x"yes" ; then
        AC_MSG_ERROR(--with-allegro5 and --with-allegro4 are mutually exclusive)
elif test x"$with_allegro" = x"no" ; then
        AC_MSG_NOTICE(building without allegro libraries)
fi

AC_CHECK_HEADERS([allegro5/allegro.h], [HAVE_ALLEGRO5=1], [HAVE_ALLEGRO5=0], [])
AM_CONDITIONAL([USE_ALLEGRO5], [test "$with_allegro5" != no -a "$with_allegro4" != yes -a "$HAVE_ALLEGRO5" -eq 1])

AC_CHECK_HEADERS([allegro.h], [HAVE_ALLEGRO4=1], [HAVE_ALLEGRO4=0], [])
AM_CONDITIONAL([USE_ALLEGRO4], [test "$with_allegro4" != no -a "$with_allegro5" != yes -a "$HAVE_ALLEGRO4" -eq 1])

# check for Darwin
AC_MSG_CHECKING([if running on Darwin])
UNAME=`uname -s`
if test x"$UNAME" = x"Darwin"; then
        AC_MSG_RESULT([yes])
        on_darwin=true
else
        AC_MSG_RESULT([no])
        on_darwin=false
fi
AM_CONDITIONAL([DARWIN], [test x"$on_darwin" = x"true"])

AC_CONFIG_FILES([
        Makefile
        src/Makefile
        src/path_prefix.h
        gamedata/Makefile
        gamedata/gfx/Makefile
        gamedata/gfx.2009/Makefile
        gamedata/gfx.riderx/Makefile
        gamedata/gfx.2003/Makefile
        gamedata/gfx.simple/Makefile
        gamedata/map/Makefile
        gamedata/music/Makefile
        gamedata/savegame/Makefile
        gamedata/sound/Makefile
        gamedata/text/Makefile])
AC_OUTPUT
