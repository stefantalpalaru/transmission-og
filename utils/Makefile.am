AM_CPPFLAGS = -I@top_srcdir@

AM_CFLAGS = \
    @LIBEVENT_CFLAGS@ \
    @LIBCURL_CFLAGS@ \
    @ZLIB_CFLAGS@ \
    @PTHREAD_CFLAGS@

bin_PROGRAMS = \
    transmission-og-create \
    transmission-og-edit \
    transmission-og-remote \
    transmission-og-show

transmission_og_create_SOURCES = create.c
nodist_EXTRA_transmission_og_create_SOURCES = dummy.cxx # force C++ linking
transmission_og_edit_SOURCES = edit.c
nodist_EXTRA_transmission_og_edit_SOURCES = dummy.cxx # force C++ linking
transmission_og_remote_SOURCES = remote.c
nodist_EXTRA_transmission_og_remote_SOURCES = dummy.cxx # force C++ linking
transmission_og_show_SOURCES = show.c
nodist_EXTRA_transmission_og_show_SOURCES = dummy.cxx # force C++ linking

noinst_HEADERS = \
    units.h

dist_man_MANS = \
    transmission-og-create.1 \
    transmission-og-edit.1 \
    transmission-og-remote.1 \
    transmission-og-show.1

transmission_og_create_LDADD = \
    $(top_builddir)/libtransmission/libtransmission.a \
    @LIBUPNP_LIBS@ \
    @LIBNATPMP_LIBS@ \
    @INTLLIBS@ \
    @DHT_LIBS@ \
    @LIBB64_LIBS@ \
    @LIBUTP_LIBS@ \
    @LIBEVENT_LIBS@ \
    @LIBCURL_LIBS@ \
    @CRYPTO_LIBS@ \
    @ZLIB_LIBS@ \
    @PTHREAD_LIBS@ \
    ${LIBM}
transmission_og_edit_LDADD = $(transmission_og_create_LDADD)
transmission_og_remote_LDADD = $(transmission_og_create_LDADD)
transmission_og_show_LDADD = $(transmission_og_create_LDADD)
