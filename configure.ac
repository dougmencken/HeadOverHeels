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

# configure paths for libpng

AC_ARG_WITH(libpng-prefix,AS_HELP_STRING([--with-libpng-prefix=],[prefix where libpng is installed (optional)]), libpng_prefix="$withval", libpng_prefix="")
AC_ARG_WITH(libpng-libs,AS_HELP_STRING([--with-libpng-libs=],[where libpng library is installed (optional)]), libpng_libraries="$withval", libpng_libraries="")
AC_ARG_WITH(libpng-includes,AS_HELP_STRING([--with-libpng-includes=],[where libpng headers are installed (optional)]), libpng_includes="$withval", libpng_includes="")

if test "x$libpng_libraries" != "x" ; then
    libpng_LIBS="-L$libpng_libraries"
elif test "x$libpng_prefix" != "x" ; then
    libpng_LIBS="-L$libpng_prefix/lib"
fi

libpng_LIBS="$libpng_LIBS -lpng"

if test "x$libpng_includes" != "x" ; then
    libpng_HEADERS="-I $libpng_includes"
elif test "x$libpng_prefix" != "x" ; then
    libpng_HEADERS="-I $libpng_prefix/include"
fi

AC_SUBST([libpng_LIBS])
AC_SUBST([libpng_HEADERS])

# configure paths for libogg

AC_ARG_WITH(ogg-prefix,AS_HELP_STRING([--with-ogg-prefix=],[prefix where libogg is installed (optional)]), ogg_prefix="$withval", ogg_prefix="")
AC_ARG_WITH(ogg-libs,AS_HELP_STRING([--with-ogg-libs=],[where libogg library is installed (optional)]), ogg_libraries="$withval", ogg_libraries="")
AC_ARG_WITH(ogg-includes,AS_HELP_STRING([--with-ogg-includes=],[where libogg headers are installed (optional)]), ogg_includes="$withval", ogg_includes="")

if test "x$ogg_libraries" != "x" ; then
    ogg_LIBS="-L$ogg_libraries"
elif test "x$ogg_prefix" != "x" ; then
    ogg_LIBS="-L$ogg_prefix/lib"
fi

ogg_LIBS="$ogg_LIBS -logg"

if test "x$ogg_includes" != "x" ; then
    ogg_HEADERS="-I $ogg_includes"
elif test "x$ogg_prefix" != "x" ; then
    ogg_HEADERS="-I $ogg_prefix/include"
fi

AC_SUBST([ogg_LIBS])
AC_SUBST([ogg_HEADERS])

# configure paths for libvorbis

AC_ARG_WITH(vorbis-prefix,AS_HELP_STRING([--with-vorbis-prefix=],[prefix where libvorbis is installed (optional)]), vorbis_prefix="$withval", vorbis_prefix="")
AC_ARG_WITH(vorbis-libs,AS_HELP_STRING([--with-vorbis-libs=],[where libvorbis library is installed (optional)]), vorbis_libraries="$withval", vorbis_libraries="")
AC_ARG_WITH(vorbis-includes,AS_HELP_STRING([--with-vorbis-includes=],[where libvorbis headers are installed (optional)]), vorbis_includes="$withval", vorbis_includes="")

if test "x$vorbis_libraries" != "x" ; then
    vorbis_LIBS="-L$vorbis_libraries"
elif test "x$vorbis_prefix" != "x" ; then
    vorbis_LIBS="-L$vorbis_prefix/lib"
fi

vorbis_LIBS="$vorbis_LIBS -lvorbis -lvorbisenc -lvorbisfile"

if test "x$vorbis_includes" != "x" ; then
    vorbis_HEADERS="-I $vorbis_includes"
elif test "x$vorbis_prefix" != "x" ; then
    vorbis_HEADERS="-I $vorbis_prefix/include"
fi

AC_SUBST([vorbis_LIBS])
AC_SUBST([vorbis_HEADERS])

# configure paths for tinyxml2

AC_ARG_WITH(tinyxml2-prefix,AS_HELP_STRING([--with-tinyxml2-prefix=],[prefix where tinyxml2 is installed (optional)]), tinyxml2_prefix="$withval", tinyxml2_prefix="")
AC_ARG_WITH(tinyxml2-libs,AS_HELP_STRING([--with-tinyxml2-libs=],[where tinyxml2 library is installed (optional)]), tinyxml2_libraries="$withval", tinyxml2_libraries="")
AC_ARG_WITH(tinyxml2-includes,AS_HELP_STRING([--with-tinyxml2-includes=],[where tinyxml2 headers are installed (optional)]), tinyxml2_includes="$withval", tinyxml2_includes="")

if test "x$tinyxml2_libraries" != "x" ; then
    tinyxml2_LIBS="-L$tinyxml2_libraries"
elif test "x$tinyxml2_prefix" != "x" ; then
    tinyxml2_LIBS="-L$tinyxml2_prefix/lib"
fi

tinyxml2_LIBS="$tinyxml2_LIBS -ltinyxml2"

if test "x$tinyxml2_includes" != "x" ; then
    tinyxml2_HEADERS="-I $tinyxml2_includes"
elif test "x$tinyxml2_prefix" != "x" ; then
    tinyxml2_HEADERS="-I $tinyxml2_prefix/include"
fi

AC_SUBST([tinyxml2_LIBS])
AC_SUBST([tinyxml2_HEADERS])

# configure paths for allegro

AC_ARG_WITH(allegro-prefix,AS_HELP_STRING([--with-allegro-prefix=],[prefix where allegro is installed (optional)]), allegro_prefix="$withval", allegro_prefix="")
AC_ARG_WITH(allegro-libs,AS_HELP_STRING([--with-allegro-libs=],[where allegro library is installed (optional)]), allegro_libraries="$withval", allegro_libraries="")
AC_ARG_WITH(allegro-includes,AS_HELP_STRING([--with-allegro-includes=],[where allegro headers are installed (optional)]), allegro_includes="$withval", allegro_includes="")

if test "x$allegro_libraries" != "x" ; then
    allegro_LIBS="-L$allegro_libraries"
elif test "x$allegro_prefix" != "x" ; then
    allegro_LIBS="-L$allegro_prefix/lib"
fi

if test "x$allegro_includes" != "x" ; then
    allegro_HEADERS="-I $allegro_includes"
elif test "x$allegro_prefix" != "x" ; then
    allegro_HEADERS="-I $allegro_prefix/include"
fi

AC_SUBST([allegro_HEADERS])

# look for headers

preprocessor_options_before="$CPPFLAGS"

AC_CHECK_HEADER([unistd.h],, [AC_MSG_ERROR([unistd.h is absent])])
AC_CHECK_HEADER([pthread.h],, [AC_MSG_ERROR([pthread.h is absent])])

AC_CHECK_HEADER([execinfo.h], [EXECINFO_H=1], [EXECINFO_H=0])
AM_CONDITIONAL([HAVE_EXECINFO_H], [test "$EXECINFO_H" -eq 1])

AC_SUBST([CPPFLAGS], ["$libpng_HEADERS $preprocessor_options_before"])
AC_CHECK_HEADER([png.h],, [AC_MSG_ERROR([libpng headers are absent, with CPPFLAGS="$CPPFLAGS"])])

AC_SUBST([CPPFLAGS], ["$ogg_HEADERS $preprocessor_options_before"])
AC_CHECK_HEADER([ogg/ogg.h],, [AC_MSG_ERROR([libogg headers are absent, with CPPFLAGS="$CPPFLAGS"])])

AC_SUBST([CPPFLAGS], ["$ogg_HEADERS $vorbis_HEADERS $preprocessor_options_before"])
AC_CHECK_HEADERS([vorbis/codec.h vorbis/vorbisenc.h vorbis/vorbisfile.h],, [AC_MSG_ERROR([libvorbis headers are absent, with CPPFLAGS="$CPPFLAGS"])])

AC_SUBST([CPPFLAGS], ["$tinyxml2_HEADERS $preprocessor_options_before"])
AC_LANG_PUSH([C++]) # tinyxml2.h is C++ header
AC_CHECK_HEADER([tinyxml2.h],, [AC_MSG_ERROR([tinyxml2 header is absent, with CPPFLAGS="$CPPFLAGS"])])
AC_LANG_POP([C++])

AC_SUBST([CPPFLAGS], ["$preprocessor_options_before"])

# which allegro version to use

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

AC_SUBST([CPPFLAGS], ["$allegro_HEADERS $preprocessor_options_before"])
AC_MSG_NOTICE([looking for allegro headers with CPPFLAGS="$CPPFLAGS"])
AC_CHECK_HEADER([allegro5/allegro.h], [HAVE_ALLEGRO5=1], [HAVE_ALLEGRO5=0])
AC_CHECK_HEADER([allegro.h], [HAVE_ALLEGRO4=1], [HAVE_ALLEGRO4=0])
AC_SUBST([CPPFLAGS], ["$preprocessor_options_before"])

AM_CONDITIONAL([USE_ALLEGRO5], [test "$with_allegro5" != no -a "$with_allegro4" != yes -a "$HAVE_ALLEGRO5" -eq 1])
AM_CONDITIONAL([USE_ALLEGRO4], [test "$with_allegro4" != no -a "$with_allegro5" != yes -a "$HAVE_ALLEGRO4" -eq 1])

if test "$with_allegro5" != no -a "$with_allegro4" != yes -a "$HAVE_ALLEGRO5" -eq 1 ; then
# use allegro 5
        allegro_lFiles="-lallegro -lallegro_audio -lallegro_acodec -lallegro_image -lallegro_font -lallegro_primitives"
        if test x"$on_darwin" = x"true" ; then
                allegro_lFiles="$allegro_lFiles -lallegro_main"
        fi
elif test "$with_allegro4" != no -a "$with_allegro5" != yes -a "$HAVE_ALLEGRO4" -eq 1 ; then
# use allegro 4
        allegro_lFiles="-lalleg"
        ##allegro_lFiles=`command -v allegro-config && allegro-config --libs release`
fi

allegro_LIBS="$allegro_LIBS $allegro_lFiles"

AC_SUBST([allegro_LIBS])

# make it make it

AC_CONFIG_FILES([
        Makefile
        source/Makefile
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
        gamedata/text/Makefile
    ])
AC_OUTPUT
