BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST = CMakeLists.txt

AM_CPPFLAGS = -I@top_srcdir@ -DEMBEDDED

AM_CFLAGS = \
    @LIBEVENT_CFLAGS@ \
    @LIBCURL_CFLAGS@ \
    @SYSTEMD_CFLAGS@ \
    @ZLIB_CFLAGS@ \
    @PTHREAD_CFLAGS@

EXTRA_DIST += \
    transmission-og-daemon.service

dist_man_MANS = \
    transmission-og-daemon.1

bin_PROGRAMS = \
    transmission-og-daemon

transmission_og_daemon_LDADD = \
    $(top_builddir)/libtransmission/libtransmission-og.la \
    @LIBUPNP_LIBS@ \
    @LIBNATPMP_LIBS@ \
    @DHT_LIBS@ \
    @LIBB64_LIBS@ \
    @LIBUTP_LIBS@ \
    @LIBEVENT_LIBS@ \
    @LIBCURL_LIBS@ \
    @CRYPTO_LIBS@ \
    @INTLLIBS@ \
    @SYSTEMD_LIBS@ \
    @ZLIB_LIBS@ \
    @PTHREAD_LIBS@ \
    $(LIBM)

noinst_HEADERS = \
		 daemon.h \
		 ../libtransmission/version.h

transmission_og_daemon_SOURCES = daemon.c
nodist_EXTRA_transmission_og_daemon_SOURCES = dummy.cxx # force C++ linking

if WIN32
transmission-og-daemon.rc: ../transmission-og.rc.in
	TR_FILE_DESCRIPTION="Transmission OG Daemon"; \
	TR_INTERNAL_NAME="$(PACKAGE_NAME)-daemon"; \
	TR_ORIGINAL_FILENAME="$(PACKAGE_NAME)-daemon.exe"; \
	HAVE_ICON=0; \
	sed \
	-e "s/%TR_FILE_DESCRIPTION%/$${TR_FILE_DESCRIPTION}/" \
	-e "s/%TR_INTERNAL_NAME%/$${TR_INTERNAL_NAME}/" \
	-e "s/%TR_ORIGINAL_FILENAME%/$${TR_ORIGINAL_FILENAME}/" \
	-e "s/%HAVE_ICON%/$${HAVE_ICON}/" \
	$< > $@

transmission_og_daemon_SOURCES += daemon-win32.c

transmission-og-daemon.res: transmission-og-daemon.rc ../libtransmission/version.h
	$(WINDRES) -J rc -I $(top_srcdir) -i $< -O coff -o $@

BUILT_SOURCES += transmission-og-daemon.res
CLEANFILES += transmission-og-daemon.rc transmission-og-daemon.res
transmission_og_daemon_LDADD += transmission-og-daemon.res
else #WIN32
transmission_og_daemon_SOURCES += daemon-posix.c
endif #WIN32
