dnl "user_agent_prefix" format: "[MAJOR].[MINOR][PATCH]" with a trailing "+" if it's a development version
dnl "peer_id_prefix" format: "-TR[MAJOR][MINOR][PATCH][STATUS]-" (each a single char)
dnl STATUS: "X" for prerelease beta builds,
dnl         "Z" for unsupported trunk builds,
dnl         "0" for stable, supported releases
dnl these should be the only two lines you need to change
m4_define([user_agent_prefix],[3.06+])
m4_define([peer_id_prefix],[-TR306Z-])

AC_INIT([transmission-og],[user_agent_prefix],[https://github.com/stefantalpalaru/transmission-og])
AC_SUBST(USERAGENT_PREFIX,[user_agent_prefix])
AC_SUBST(PEERID_PREFIX,[peer_id_prefix])

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_CONFIG_MACRO_DIR([m4])

dnl Prevent reconfiguring by default, due to Git not preserving timestamps.
AM_MAINTAINER_MODE

AC_CONFIG_SRCDIR(libtransmission/transmission.h)
AM_INIT_AUTOMAKE([tar-pax no-dist-gzip dist-xz foreign subdir-objects])

LT_LIB_M

CFLAGS="${CFLAGS} -std=gnu11 -fpermissive"
CXXFLAGS="${CXXFLAGS} -std=gnu++17 -fpermissive"
dnl Needed by XFS quota support:
CPPFLAGS="${CPPFLAGS} -D_GNU_SOURCE"

AC_PROG_CC
AC_PROG_CXX
AC_C_INLINE

dnl When we create a shared libtransmission-og, we are linking it to static
dnl libraries in "third-party". Let's make sure those contain PIC objects.
LT_INIT([pic-only])

if test m4_substr(peer_id_prefix,6,1) = "0"; then
  supported_build=yes
  CPPFLAGS="$CPPFLAGS -DNDEBUG"
else
  supported_build=no
  if test "x${GCC}" = "xyes" ; then
    CFLAGS="${CFLAGS} -ggdb3"
    CXXFLAGS="${CXXFLAGS} -ggdb3"
  fi
fi
AM_CONDITIONAL(TR_UNSTABLE, test "x$supported_build" = "xno")

##
##
##   MANDATORY for everything
##
##

CURL_MINIMUM=7.15.4
AC_SUBST(CURL_MINIMUM)
LIBEVENT_MINIMUM=2.0.10
AC_SUBST(LIBEVENT_MINIMUM)
ZLIB_MINIMUM=1.2.3
AC_SUBST(ZLIB_MINIMUM)

# crypto backends
OPENSSL_MINIMUM=0.9.7
AC_SUBST(OPENSSL_MINIMUM)
CYASSL_MINIMUM=3.0
AC_SUBST(CYASSL_MINIMUM)
POLARSSL_MINIMUM=0x01020000 # 1.2
AC_SUBST(POLARSSL_MINIMUM)

##
##
##   MANDATORY for the GTK+ client
##
##

GTK_MINIMUM=3.4.0
GLIB_MINIMUM=2.32.0
GIO_MINIMUM=2.26.0

dnl ----------------------------------------------------------------------------
dnl
dnl  platform-specific stuff.

AC_CANONICAL_HOST
on_macos="no"
on_windows="no"
case $host_os in
   *mingw32*)
     on_windows="yes"
     CXXFLAGS="${CXXFLAGS} -mms-bitfields -mwin32 -mwindows"
     dnl Target version (Vista and up)
     CPPFLAGS="${CPPFLAGS} -DWINVER=0x0600 -D_WIN32_WINNT=0x0600 -DWIN32_LEAN_AND_MEAN"
     dnl Use Unicode API (although we always use W or A names explicitly)
     CPPFLAGS="${CPPFLAGS} -DUNICODE -D_UNICODE"
     dnl Ignore various deprecation and security warnings (at least for now)
     CPPFLAGS="${CPPFLAGS} -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS -D_WINSOCK_DEPRECATED_NO_WARNINGS"
     dnl Increase maximum FD_SET size
     CPPFLAGS="${CPPFLAGS} -DFD_SETSIZE=1024"
     dnl Use MinGW versions of all the printf-family functions
     CPPFLAGS="${CPPFLAGS} -D__USE_MINGW_ANSI_STDIO=1"
     dnl Windows specific libraries
     LIBS="${LIBS} -liphlpapi -lshell32 -lws2_32 -lcrypt32 -lshlwapi -luuid -lole32"
     transmissionlocaledir="locale"
     if test -z "$host_alias"; then
       hostaliaswindres=
     else
       hostaliaswindres="$host_alias-windres";
     fi
     AC_CHECK_TOOL(WINDRES, windres)

     dnl For Windows tests on VPATH builds.
     AC_CONFIG_LINKS([libtransmission/subprocess-test.cmd:libtransmission/subprocess-test.cmd])
     ;;

  *darwin*)
    on_macos="yes"
    LIBS="${LIBS} -framework Cocoa"

    AS_IF([test -z "${MACOSX_DEPLOYMENT_TARGET}"], [
        MACOSX_DEPLOYMENT_TARGET="12.0"
    ])
    AC_SUBST(MACOSX_DEPLOYMENT_TARGET)

    CFLAGS="${CFLAGS} -mmacosx-version-min=${MACOSX_DEPLOYMENT_TARGET}"
    CXXFLAGS="${CXXFLAGS} -mmacosx-version-min=${MACOSX_DEPLOYMENT_TARGET}"
    LDFLAGS="${LDFLAGS} -mmacosx-version-min=${MACOSX_DEPLOYMENT_TARGET}"
    ;;
esac

AM_CONDITIONAL(WIN32, test "x${on_windows}" = "xyes")
AM_CONDITIONAL(MACOS, test "x${on_macos}" = "xyes")


dnl ----------------------------------------------------------------------------
dnl
dnl  compiler

if test "x${GCC}" = "xyes"; then
    CFLAGS="${CFLAGS} -Wall -W -Wpointer-arith -Wformat-security -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-declarations -Wmissing-format-attribute -Wredundant-decls -Wnested-externs -Wunused-parameter -Wwrite-strings -Winline -Wfloat-equal"

    dnl figure out gcc version
    AC_MSG_CHECKING([gcc version])
    GCC_VERSION=`${CC} -dumpversion`
    GCC_MAJOR=`echo ${GCC_VERSION} | cut -d . -f1`
    GCC_MINOR=`echo ${GCC_VERSION} | cut -d . -f2`
    GCC_VERSION_NUM=`(expr ${GCC_MAJOR} "*" 100 + ${GCC_MINOR}) 2>/dev/null`

    AC_MSG_RESULT(${GCC_VERSION})
    if test ${GCC_VERSION_NUM} -ge 304; then
        dnl these were added in 3.4
        CFLAGS="${CFLAGS} -Wextra -Winit-self"
    fi
    if test ${GCC_VERSION_NUM} -ge 403; then
        dnl these were added in 4.3
        CFLAGS="${CFLAGS} -Wvariadic-macros"
    fi
fi

dnl ----------------------------------------------------------------------------
dnl
dnl  various checks

AC_PROG_LN_S
AC_PROG_MKDIR_P
AC_PROG_FGREP
AC_PROG_SED


AC_CHECK_HEADERS_ONCE([sys/time.h])

AC_CHECK_HEADERS([xlocale.h])
AC_CHECK_FUNCS([iconv pread pwrite lrintf strlcpy daemon dirname basename canonicalize_file_name strcasecmp localtime_r fallocate64 posix_fallocate memmem strsep strtold syslog valloc getpagesize posix_memalign statvfs htonll ntohll mkdtemp uselocale _configthreadlocale strcasestr flock])
AC_PROG_INSTALL
AC_PROG_MAKE_SET
ACX_PTHREAD

if test "x$ac_cv_func_strtold" != "xyes" ; then
    CPPFLAGS="$CPPFLAGS -Dstrtold=strtod"
fi

AS_IF([test "x${ac_cv_func_flock}" = "xyes"], [
       AC_DEFINE([HAVE_FLOCK], 1)
       ])

AC_SEARCH_LIBS(cos, [m])
AC_SEARCH_LIBS([socket], [socket net])
AC_SEARCH_LIBS([gethostbyname], [nsl bind])
AC_SEARCH_LIBS([quotacursor_skipidtype], [quota])
PKG_CHECK_MODULES(LIBCURL, [libcurl >= $CURL_MINIMUM])
PKG_CHECK_MODULES(ZLIB, [zlib >= $ZLIB_MINIMUM])

AC_SYS_LARGEFILE

AC_FUNC_GETMNTENT

AC_CHECK_HEADERS([sys/statvfs.h \
                  xfs/xfs.h])


dnl ----------------------------------------------------------------------------
dnl
dnl  Cryptography library.

AC_ARG_WITH([crypto], AS_HELP_STRING([--with-crypto=PKG],
            [use specified crypto library: auto (default), openssl, cyassl, polarssl]),
            [want_crypto=$withval], [want_crypto=auto])

AS_IF([test "x$want_crypto" = "xauto" -o "x$want_crypto" = "xopenssl"], [
    PKG_CHECK_MODULES(OPENSSL, [openssl >= $OPENSSL_MINIMUM],
        [want_crypto="openssl"; CRYPTO_PKG="openssl"; CRYPTO_CFLAGS="$OPENSSL_CFLAGS"; CRYPTO_LIBS="$OPENSSL_LIBS"],
        [AS_IF([test "x$want_crypto" = "xopenssl"],
            [AC_MSG_ERROR([OpenSSL support requested, but library not found.])]
        )]
    )
])
AS_IF([test "x$want_crypto" = "xauto" -o "x$want_crypto" = "xcyassl"], [
    PKG_CHECK_MODULES(WOLFSSL, [wolfssl >= $CYASSL_MINIMUM],
        [want_crypto="cyassl"; CRYPTO_PKG="cyassl"; CRYPTO_CFLAGS="$WOLFSSL_CFLAGS"; CRYPTO_LIBS="$WOLFSSL_LIBS"; CYASSL_IS_WOLFSSL=yes],
        [PKG_CHECK_MODULES(CYASSL, [cyassl >= $CYASSL_MINIMUM],
            [want_crypto="cyassl"; CRYPTO_PKG="cyassl"; CRYPTO_CFLAGS="$CYASSL_CFLAGS"; CRYPTO_LIBS="$CYASSL_LIBS"],
            [AS_IF([test "x$want_crypto" = "xcyassl"],
                [AC_MSG_ERROR([CyaSSL support requested, but library not found.])]
            )]
        )]
    )
])
AS_IF([test "x$want_crypto" = "xauto" -o "x$want_crypto" = "xpolarssl"], [
    PKG_CHECK_MODULES(MBEDTLS, [mbedtls >= $POLARSSL_MINIMUM],
        [want_crypto="polarssl"; CRYPTO_PKG="polarssl"; CRYPTO_CFLAGS="$MBEDTLS_CFLAGS"; CRYPTO_LIBS="$MBEDTLS_LIBS"; POLARSSL_IS_MBEDTLS=yes],
        [AC_CHECK_HEADER([polarssl/version.h],
            [AC_EGREP_CPP([version_ok], [#include <polarssl/version.h>
                                         #if defined (POLARSSL_VERSION_NUMBER) && POLARSSL_VERSION_NUMBER >= $POLARSSL_MINIMUM
                                         version_ok
                                         #endif],
                [AC_CHECK_LIB([polarssl], [dhm_calc_secret],
                    [want_crypto="polarssl"; CRYPTO_PKG="polarssl"; CRYPTO_CFLAGS=""; CRYPTO_LIBS="-lpolarssl"],
                    [AS_IF([test "x$want_crypto" = "xpolarssl"],
                        [AC_MSG_ERROR([PolarSSL support requested, but library not found.])]
                    )]
                )],
                [AS_IF([test "x$want_crypto" = "xpolarssl"],
                    [AC_MSG_ERROR([PolarSSL support requested, but version not suitable.])]
                )]
            )],
            [AS_IF([test "x$want_crypto" = "xpolarssl"],
                [AC_MSG_ERROR([PolarSSL support requested, but headers not found.])]
            )]
        )]
    )
])
# we should have found the library by now
AS_IF([test "x$CRYPTO_PKG" = "x"], [
    AS_IF([test "x$want_crypto" = "xauto"],
        [AC_MSG_ERROR([Unable to find any supported crypto library.])],
        [AC_MSG_ERROR([Requested crypto library "$want_crypto" is not supported.])],
    )
])

AM_CONDITIONAL([CRYPTO_USE_OPENSSL],[test "x$CRYPTO_PKG" = "xopenssl"])
AM_CONDITIONAL([CRYPTO_USE_CYASSL],[test "x$CRYPTO_PKG" = "xcyassl"])
AM_CONDITIONAL([CYASSL_IS_WOLFSSL],[test "x$CYASSL_IS_WOLFSSL" = "xyes"])
AM_CONDITIONAL([CRYPTO_USE_POLARSSL],[test "x$CRYPTO_PKG" = "xpolarssl"])
AM_CONDITIONAL([POLARSSL_IS_MBEDTLS],[test "x$POLARSSL_IS_MBEDTLS" = "xyes"])
AC_SUBST(CRYPTO_PKG)
AC_SUBST(CRYPTO_CFLAGS)
AC_SUBST(CRYPTO_LIBS)


dnl ----------------------------------------------------------------------------
dnl
dnl posix_fadvise

dnl can posix_fadvise be used
AC_CHECK_DECLS(posix_fadvise, [], [], [
#define _XOPEN_SOURCE 600
#include <fcntl.h>
])
AC_CHECK_FUNCS([posix_fadvise])


dnl ----------------------------------------------------------------------------
dnl
dnl file monitoring for the daemon

AC_ARG_WITH([inotify],
            [AS_HELP_STRING([--with-inotify], [enable inotify support (default=auto)])],
            [WANT_INOTIFY=${withval}],
            [WANT_INOTIFY=auto])
HAVE_INOTIFY=0
AS_IF([test "x$WANT_INOTIFY" != "xno"],
      [AC_CHECK_HEADER([sys/inotify.h],
                       [AC_CHECK_FUNC([inotify_init],
                                      [HAVE_INOTIFY=1])],
                       [AS_IF([test "x$WANT_INOTIFY" = "xyes"],
                              [AC_MSG_ERROR("inotify not found!")])])])
AM_CONDITIONAL([USE_INOTIFY], [test "x$WANT_INOTIFY" != "xno" -a $HAVE_INOTIFY -eq 1])

AC_ARG_WITH([kqueue],
            [AS_HELP_STRING([--with-kqueue],[enable kqueue support (default=auto)])],
            [WANT_KQUEUE=${withval}],
            [WANT_KQUEUE=auto])
HAVE_KQUEUE=0
AS_IF([test "x$WANT_KQUEUE" != "xno"],
      [AC_CHECK_HEADER([sys/event.h],
                       [AC_CHECK_FUNC([kqueue],
                                      [HAVE_KQUEUE=1])],
                       [AS_IF([test "x$WANT_KQUEUE" = "xyes"],
                              [AC_MSG_ERROR("kqueue not found!")])])])
AM_CONDITIONAL([USE_KQUEUE], [test "x$WANT_KQUEUE" != "xno" -a $HAVE_KQUEUE -eq 1])


dnl ----------------------------------------------------------------------------
dnl
dnl Check whether to enable systemd startup notification. This requires libsystemd.

AC_ARG_WITH([systemd], AS_HELP_STRING([--with-systemd],
            [Add support for systemd startup notification (default is autodetected)]),
            [USE_SYSTEMD=$withval], [USE_SYSTEMD=auto])
AS_IF([test "x$USE_SYSTEMD" != "xno"], [
    PKG_CHECK_MODULES([SYSTEMD], [libsystemd],
        [AC_DEFINE(USE_SYSTEMD,1,[use systemd startup notification])],
        [AS_IF([test "x$USE_SYSTEMD" = "xyes"],
            [AC_MSG_ERROR([systemd startup notification support requested, but libsystemd not found.])]
        )]
    )
])


dnl ----------------------------------------------------------------------------
dnl
dnl  dht

AC_ARG_ENABLE([external-dht],
              AS_HELP_STRING([--enable-external-dht], [use system libdht]))
AS_IF([test "x${enable_external_dht}" = "xyes"], [
       DHT_CFLAGS=""
       DHT_LIBS="-ldht"
       DHT_LIBS_QMAKE="-ldht"
       ], [
           DHT_CFLAGS="-I\$(top_srcdir)/third-party/dht/include"
           DHT_LIBS="\$(top_builddir)/third-party/dht/libdht.la"
           DHT_LIBS_QMAKE="\$\${TRANSMISSION_TOP}/third-party/dht/.libs/libdht.a"
           ])

AM_CONDITIONAL([BUILD_DHT], [test "x${enable_external_dht}" != "xyes"])
AC_SUBST(DHT_CFLAGS)
AC_SUBST(DHT_LIBS)
AC_SUBST(DHT_LIBS_QMAKE)


dnl ----------------------------------------------------------------------------
dnl
dnl  libb64

AC_ARG_ENABLE([external-b64],
              AS_HELP_STRING([--enable-external-b64], [use system libb64]))
AS_IF([test "x${enable_external_b64}" = "xyes"], [
       AC_SEARCH_LIBS([base64_init_encodestate], [b64], [], [
                       AC_MSG_ERROR([unable to find the "b64" library])
                       ])
       LIBB64_CFLAGS="-DUSE_SYSTEM_B64"
       LIBB64_LIBS="-lb64"
       LIBB64_LIBS_QMAKE="-lb64"
       ], [
           LIBB64_CFLAGS="-I\$(top_srcdir)/third-party/libb64/include"
           LIBB64_LIBS="\$(top_builddir)/third-party/libb64/libb64.la"
           LIBB64_LIBS_QMAKE="\$\${TRANSMISSION_TOP}/third-party/libb64/.libs/libb64.a"
           ])

AM_CONDITIONAL([BUILD_B64], [test "x${enable_external_b64}" != "xyes"])
AC_SUBST(LIBB64_CFLAGS)
AC_SUBST(LIBB64_LIBS)
AC_SUBST(LIBB64_LIBS_QMAKE)


dnl ----------------------------------------------------------------------------
dnl
dnl  utp

AC_CHECK_LIB([rt],
             [clock_gettime],
             [libutp_extra_libs="-lrt"],
             [libutp_extra_libs=""])

AC_ARG_ENABLE([utp],
              AS_HELP_STRING([--disable-utp], [disable µTP support]))
AC_ARG_ENABLE([external-utp],
              AS_HELP_STRING([--enable-external-utp], [use system libutp]))
AS_IF([test "x${enable_utp}" != "xno"], [
       AC_DEFINE([WITH_UTP], [1])
       AS_IF([test "x${enable_external_utp}" = "xyes"], [
              AC_SEARCH_LIBS([utp_close], [utp], [], [
                              AC_MSG_ERROR([unable to find the "utp" library])
                              ])
              LIBUTP_CFLAGS=""
              LIBUTP_LIBS="-lutp ${libutp_extra_libs}"
              LIBUTP_LIBS_QMAKE="-lutp ${libutp_extra_libs}"
              ], [
                  LIBUTP_CFLAGS="-I\$(top_srcdir)/third-party/libutp/include"
                  LIBUTP_LIBS="\$(top_builddir)/third-party/libutp/libutp.la ${libutp_extra_libs}"
                  LIBUTP_LIBS_QMAKE="\$\${TRANSMISSION_TOP}/third-party/libutp/.libs/libutp.a ${libutp_extra_libs}"
                  ])
       ])

AC_SUBST(LIBUTP_CFLAGS)
AC_SUBST(LIBUTP_LIBS)
AC_SUBST(LIBUTP_LIBS_QMAKE)
AM_CONDITIONAL([BUILD_UTP],[test "x${enable_external_utp}" != "xyes"])

dnl
dnl  miniupnpc
dnl

AC_ARG_ENABLE([external-miniupnpc],
              AS_HELP_STRING([--enable-external-miniupnpc], [use system miniupnpc]))
AS_IF([test "x${enable_external_miniupnpc}" = "xyes"], [
       AC_SEARCH_LIBS([upnpDiscover], [miniupnpc], [], [
                       AC_MSG_ERROR([unable to find the "miniupnpc" library])
                       ])
       LIBUPNP_CFLAGS=""
       LIBUPNP_LIBS="-lminiupnpc"
       LIBUPNP_LIBS_QMAKE="-lminiupnpc"
       ], [
           LIBUPNP_CFLAGS="-I\$(top_srcdir)/third-party/miniupnpc/include"
           LIBUPNP_LIBS="\$(top_builddir)/third-party/miniupnpc/libminiupnp.la"
           LIBUPNP_LIBS_QMAKE="\$\${TRANSMISSION_TOP}/third-party/miniupnpc/.libs/libminiupnp.a"
	   AC_DEFINE([MINIUPNP_STATICLIB])
           ])

AM_CONDITIONAL([BUILD_MINIUPNP], [test "x${enable_external_miniupnpc}" != "xyes"])
AC_SUBST(LIBUPNP_CFLAGS)
AC_SUBST(LIBUPNP_LIBS)
AC_SUBST(LIBUPNP_LIBS_QMAKE)

dnl ----------------------------------------------------------------------------
dnl
dnl  Allow usage of system natpmp library

AC_ARG_ENABLE([external-natpmp],
              AS_HELP_STRING([--enable-external-natpmp], [use system natpmp]))
AS_IF([test "x${enable_external_natpmp}" = "xyes"], [
       AC_SEARCH_LIBS([initnatpmp], [natpmp], [], [
                       AC_MSG_ERROR([unable to find the "natpmp" library])
                       ])
       LIBNATPMP_CFLAGS=""
       LIBNATPMP_LIBS="-lnatpmp"
       LIBNATPMP_LIBS_QMAKE="-lnatpmp"
       ], [
           LIBNATPMP_CFLAGS="-I\$(top_srcdir)/third-party/libnatpmp/"
           LIBNATPMP_LIBS="\$(top_builddir)/third-party/libnatpmp/libnatpmp.la"
           LIBNATPMP_LIBS_QMAKE="\$\${TRANSMISSION_TOP}/third-party/libnatpmp/.libs/libnatpmp.a"
	   AC_DEFINE([NATPMP_STATICLIB])
           ])

AM_CONDITIONAL([BUILD_NATPMP], [test "x${enable_external_natpmp}" != "xyes"])
AC_SUBST(LIBNATPMP_CFLAGS)
AC_SUBST(LIBNATPMP_LIBS)
AC_SUBST(LIBNATPMP_LIBS_QMAKE)

dnl ----------------------------------------------------------------------------
dnl
dnl  libevent


AC_ARG_ENABLE([external-libevent],
              AS_HELP_STRING([--enable-external-libevent], [use system libevent]))
AS_IF([test "x${enable_external_libevent}" = "xyes"], [
       PKG_CHECK_MODULES(LIBEVENT, [libevent >= ${LIBEVENT_MINIMUM}])
       LIBEVENT_INCLUDEPATH_QMAKE=""
       LIBEVENT_LIBS_QMAKE="${LIBEVENT_LIBS}"
       ], [
	   dnl "event2/event-config.h" is generated at configuration time and placed in the builddir
           LIBEVENT_CFLAGS="-I\$(top_srcdir)/third-party/libevent/include -I\$(top_builddir)/third-party/libevent/include"
	   dnl We handle that for qmake in "qt/Makefile.am", because that's where $(top_builddir) has a value - not in "qt/config.pri.in".
           LIBEVENT_INCLUDEPATH_QMAKE="\$\${TRANSMISSION_TOP}/third-party/libevent/include"
           LIBEVENT_LIBS="\$(top_builddir)/third-party/libevent/.libs/libevent.a"
           LIBEVENT_LIBS_QMAKE="\$\${TRANSMISSION_TOP}/third-party/libevent/.libs/libevent.a"
           AX_SUBDIRS_CONFIGURE([third-party/libevent], [
                                    [--disable-shared],
                                    [--with-pic],
                                    [--disable-samples],
                                    [--disable-libevent-regress],
                                    [CFLAGS=${CFLAGS}],
                                    [CPPFLAGS=${CPPFLAGS}],
                                    [LDFLAGS=${LDFLAGS}]
                                ])
           ])

AM_CONDITIONAL([BUILD_EVENT], [test "x${enable_external_libevent}" != "xyes"])
AC_SUBST(LIBEVENT_CFLAGS)
AC_SUBST(LIBEVENT_INCLUDEPATH_QMAKE)
AC_SUBST(LIBEVENT_LIBS)
AC_SUBST(LIBEVENT_LIBS_QMAKE)

dnl ----------------------------------------------------------------------------
dnl
dnl  GTK+ client

AC_ARG_ENABLE([nls],
              [AS_HELP_STRING([--disable-nls],[disable native language support])])
PKG_CHECK_EXISTS([gtk+-3.0 >= $GTK_MINIMUM
                  glib-2.0 >= $GLIB_MINIMUM
                  gio-2.0 >= $GIO_MINIMUM,
                  gmodule-2.0 >= $GLIB_MINIMUM
                  gthread-2.0 >= $GLIB_MINIMUM],
                 [have_gtk=yes],
                 [have_gtk=no])
AC_ARG_WITH([gtk], AS_HELP_STRING([--with-gtk],[with Gtk]),
            [with_gtk=$withval],
            [with_gtk=$have_gtk])
AM_CONDITIONAL([BUILD_GTK],[test "x$with_gtk" = "xyes"])
use_libappindicator=no
AS_IF([test "x${with_gtk}" = "xyes"], [
    AS_IF([test "x${enable_nls}" = "xno"], [
        AC_MSG_ERROR("The gtk client cannot be built without nls support.  Try adding either --enable-nls or --without-gtk" )
    ])

    PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= $GTK_MINIMUM
                              glib-2.0 >= $GLIB_MINIMUM
                              gio-2.0 >= $GIO_MINIMUM,
                              gmodule-2.0 >= $GLIB_MINIMUM
                              gthread-2.0 >= $GLIB_MINIMUM])
    PKG_CHECK_MODULES([LIBAPPINDICATOR],
                      [ayatana-appindicator3-0.1],
                      [have_ayatana_libappindicator=yes],
                      [have_ayatana_libappindicator=no])
    AS_IF([test "x${have_ayatana_libappindicator}" = "xyes"], [
        use_libappindicator=yes
        AC_DEFINE([HAVE_LIBAPPINDICATOR], 1)
        AC_DEFINE([APPINDICATOR_IS_AYATANA], 1)
    ], [
	PKG_CHECK_MODULES([LIBAPPINDICATOR],
			  [appindicator3-0.1],
			  [have_libappindicator=yes],
			  [have_libappindicator=no])
	AS_IF([test "x${have_libappindicator}" = "xyes"], [
	    use_libappindicator=yes
	    AC_DEFINE([HAVE_LIBAPPINDICATOR], 1)
	], [
	    LIBAPPINDICATOR_CFLAGS=
	    LIBAPPINDICATOR_LIBS=
	])
    ])
])

dnl ----------------------------------------------------------------------------
dnl
dnl  Qt client

AX_HAVE_QT
AC_ARG_WITH([qt], AS_HELP_STRING([--with-qt],[with Qt]),
            [with_qt=${withval}],
            [with_qt=${have_qt}])
AM_CONDITIONAL([BUILD_QT],[test "x${with_qt}" = "xyes"])
AS_IF([test "x${with_qt}" = "xyes"], [
       AS_IF([test "x${have_qt}" = "xno"], [
	      AC_MSG_ERROR([Qt support requested, but no Qt5 installation found.])
	      ])
       ])


dnl This section is only used for internationalization.
dnl If you don't need translations and this section gives you trouble --
dnl such as if you're building for a headless system --
dnl it's okay to tear this section out and re-build the configure script.
dnl
dnl Note to packagers: the bump to intltool 0.40 was made to fix
dnl a "make check" failure on some systems.  if upgrading to 0.40 is
dnl a problem and the old version was working fine for you,
dnl it should be safe to re-edit 0.40 back down to 0.23

AS_IF([test "x${enable_nls}" != "xno"], [
    m4_ifdef([IT_PROG_INTLTOOL], [
dnl intltoolize needs this at the start of a line:
IT_PROG_INTLTOOL([0.35.0],[no-xml])
    ], [
        AC_MSG_ERROR("--enable-nls requires intltool to be installed.")
    ])

    AC_CHECK_HEADERS([libintl.h])
    GETTEXT_PACKAGE=transmission-og-gtk
    AC_SUBST(GETTEXT_PACKAGE)
    AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
    AM_GLIB_GNU_GETTEXT
    transmissionlocaledir='${prefix}/${DATADIRNAME}/locale'
    AC_SUBST(transmissionlocaledir)

    dnl Check for and fix missing delete of intltool cache lock file.  Only
    dnl needed with intltool >= 0.51.0-5.1, but just always fix as that is
    dnl simpler and safe.
    dnl From: https://gitlab.gnome.org/GNOME/gparted/-/merge_requests/103
    AC_MSG_CHECKING([whether po/Makefile.in.in deletes intltool cache lock file])
    file='po/Makefile.in.in'
    AS_IF([test ! -f "$srcdir/$file"], [
        AC_MSG_RESULT([not applicable])
        ], [
            AS_IF([${FGREP} -q '.intltool-merge-cache.lock' "$srcdir/$file" 2> /dev/null], [
                AC_MSG_RESULT([yes])
                ], [
                    AS_IF([test -w "${srcdir}/${file}"], [
                        ${SED} -i '/rm -f .intltool-merge-cache/s/$/ .intltool-merge-cache.lock/' "${srcdir}/${file}" 2> /dev/null
                    ])
                AS_IF([${FGREP} -q '.intltool-merge-cache.lock' "${srcdir}/${file}" 2> /dev/null], [
                    AC_MSG_RESULT([fixed])
                ], [
                    AC_MSG_RESULT([no])
                ])
        ])
    ])
])
AC_SUBST(INTLLIBS)


AC_ARG_ENABLE([lightweight],
              AS_HELP_STRING([--enable-lightweight],[optimise libtransmission-og for low-resource systems: smaller cache size, prefer unencrypted peer connections, etc.]),
              [enable_lightweight=${enableval}],
              [enable_lightweight="no"])
if test "x${enable_lightweight}" = "xyes" ; then
  AC_DEFINE([TR_LIGHTWEIGHT],[1],[optimize libtransmission for low-resource systems])
fi

AC_ARG_ENABLE([cli],
              [AS_HELP_STRING([--enable-cli],[build old command-line client (deprecated, use transmission-og-remote instead)])],
              [build_cli=${enableval}],
              [build_cli="no"])
AM_CONDITIONAL([BUILD_CLI],[test "x${build_cli}" = "xyes"])

AC_ARG_ENABLE([mac],
              [AS_HELP_STRING([--enable-mac],[build Mac client (broken, use CMake instead)])],
              [build_mac=${enableval}],
              [build_mac="no"])
AM_CONDITIONAL([BUILD_MAC],[test "x${build_mac}" = "xyes"])
if test "x${build_mac}" = "xyes"; then
   AC_DEFINE([BUILD_MAC_CLIENT], 1)
fi

AC_ARG_ENABLE([daemon],
	      AS_HELP_STRING([--disable-daemon],[do not build the daemon]))
AM_CONDITIONAL([BUILD_DAEMON],[test "x${enable_daemon}" != "xno"])

dnl ----------------------------------------------------------------------------
dnl
dnl  Generate the output

AC_CONFIG_FILES([Makefile
                 transmission-og-gtk.spec
                 cli/Makefile
                 daemon/Makefile
                 libtransmission/Makefile
                 utils/Makefile
                 third-party/Makefile
                 third-party/dht/Makefile
                 third-party/libb64/Makefile
                 third-party/libnatpmp/Makefile
                 third-party/libutp/Makefile
                 third-party/miniupnpc/Makefile
                 gtk/Makefile
                 gtk/icons/Makefile
                 qt/config.pri
		 qt/Makefile
                 web/Makefile
                 web/images/Makefile
                 web/style/Makefile
                 web/style/jqueryui/Makefile
                 web/style/jqueryui/images/Makefile
                 web/style/transmission/Makefile
                 web/style/transmission/images/Makefile
                 web/style/transmission/images/buttons/Makefile
                 web/javascript/Makefile
                 web/javascript/jquery/Makefile
                 po/Makefile.in])

dnl Whatever the GitHub Actions Windows image is doing with Git symlinks, copying
dnl them during a "make distcheck" is broken beyond repair. We only care about
dnl "distcheck" on Windows in the context of CI testing. The source archive we
dnl actually distribute is only generated on Linux.
AS_IF([test "x${on_windows}" = "xno"], [
    AC_CONFIG_FILES([
        macosx/Makefile
    ])
])

AC_OUTPUT

dnl https://stackoverflow.com/a/67107413
AC_MSG_RESULT([configure: removing rpath from libtool...])
${SED} -i s/'^hardcode_libdir_flag_spec.*$'/'hardcode_libdir_flag_spec="-D__LIBTOOL_IS_A_FOOL__"'/g libtool

echo "

Configuration:
   Source code location:                              ${srcdir}
   Compilers:                                         ${CC} ${CXX}

   Build libtransmission-og:                          yes
      * optimised for low-resource systems:           ${enable_lightweight}
      * µTP enabled:                                  ${enable_utp:-yes}
      * crypto library:                               ${CRYPTO_PKG}

   Build daemon:                                      ${enable_daemon:-yes}
   Build old command-line client:                     ${build_cli}
   Build GTK+ client:                                 ${with_gtk}
      * libappindicator for an Ubuntu-style tray:     ${use_libappindicator}
   Build Qt client:                                   ${with_qt}
   Build native Mac client:                           ${build_mac}
"
