BUILT_SOURCES =
CLEANFILES =

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
    units.h \
    ../libtransmission/version.h

dist_man_MANS = \
    transmission-og-create.1 \
    transmission-og-edit.1 \
    transmission-og-remote.1 \
    transmission-og-show.1

common_ldadd := \
    $(top_builddir)/libtransmission/libtransmission-og.la \
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
    $(LIBM)
transmission_og_create_LDADD := $(common_ldadd)
transmission_og_edit_LDADD := $(common_ldadd)
transmission_og_remote_LDADD := $(common_ldadd)
transmission_og_show_LDADD := $(common_ldadd)

if WIN32
transmission-og-create.rc: ../transmission-og.rc.in
	TR_FILE_DESCRIPTION="Transmission OG Utility ('create')"; \
	TR_INTERNAL_NAME="$(PACKAGE_NAME)-create"; \
	TR_ORIGINAL_FILENAME="$(PACKAGE_NAME)-create.exe"; \
	HAVE_ICON=0; \
	sed \
	-e "s/%TR_FILE_DESCRIPTION%/$${TR_FILE_DESCRIPTION}/" \
	-e "s/%TR_INTERNAL_NAME%/$${TR_INTERNAL_NAME}/" \
	-e "s/%TR_ORIGINAL_FILENAME%/$${TR_ORIGINAL_FILENAME}/" \
	-e "s/%HAVE_ICON%/$${HAVE_ICON}/" \
	$< > $@

transmission-og-create.res: transmission-og-create.rc ../libtransmission/version.h
	$(WINDRES) -J rc -I $(top_srcdir) -i $< -O coff -o $@

BUILT_SOURCES += transmission-og-create.res
CLEANFILES += transmission-og-create.rc transmission-og-create.res
transmission_og_create_LDADD += transmission-og-create.res

transmission-og-edit.rc: ../transmission-og.rc.in
	TR_FILE_DESCRIPTION="Transmission OG Utility ('edit')"; \
	TR_INTERNAL_NAME="$(PACKAGE_NAME)-edit"; \
	TR_ORIGINAL_FILENAME="$(PACKAGE_NAME)-edit.exe"; \
	HAVE_ICON=0; \
	sed \
	-e "s/%TR_FILE_DESCRIPTION%/$${TR_FILE_DESCRIPTION}/" \
	-e "s/%TR_INTERNAL_NAME%/$${TR_INTERNAL_NAME}/" \
	-e "s/%TR_ORIGINAL_FILENAME%/$${TR_ORIGINAL_FILENAME}/" \
	-e "s/%HAVE_ICON%/$${HAVE_ICON}/" \
	$< > $@

transmission-og-edit.res: transmission-og-edit.rc ../libtransmission/version.h
	$(WINDRES) -J rc -I $(top_srcdir) -i $< -O coff -o $@

BUILT_SOURCES += transmission-og-edit.res
CLEANFILES += transmission-og-edit.rc transmission-og-edit.res
transmission_og_edit_LDADD += transmission-og-edit.res

transmission-og-remote.rc: ../transmission-og.rc.in
	TR_FILE_DESCRIPTION="Transmission OG Utility ('remote')"; \
	TR_INTERNAL_NAME="$(PACKAGE_NAME)-remote"; \
	TR_ORIGINAL_FILENAME="$(PACKAGE_NAME)-remote.exe"; \
	HAVE_ICON=0; \
	sed \
	-e "s/%TR_FILE_DESCRIPTION%/$${TR_FILE_DESCRIPTION}/" \
	-e "s/%TR_INTERNAL_NAME%/$${TR_INTERNAL_NAME}/" \
	-e "s/%TR_ORIGINAL_FILENAME%/$${TR_ORIGINAL_FILENAME}/" \
	-e "s/%HAVE_ICON%/$${HAVE_ICON}/" \
	$< > $@

transmission-og-remote.res: transmission-og-remote.rc ../libtransmission/version.h
	$(WINDRES) -J rc -I $(top_srcdir) -i $< -O coff -o $@

BUILT_SOURCES += transmission-og-remote.res
CLEANFILES += transmission-og-remote.rc transmission-og-remote.res
transmission_og_remote_LDADD += transmission-og-remote.res

transmission-og-show.rc: ../transmission-og.rc.in
	TR_FILE_DESCRIPTION="Transmission OG Utility ('show')"; \
	TR_INTERNAL_NAME="$(PACKAGE_NAME)-show"; \
	TR_ORIGINAL_FILENAME="$(PACKAGE_NAME)-show.exe"; \
	HAVE_ICON=0; \
	sed \
	-e "s/%TR_FILE_DESCRIPTION%/$${TR_FILE_DESCRIPTION}/" \
	-e "s/%TR_INTERNAL_NAME%/$${TR_INTERNAL_NAME}/" \
	-e "s/%TR_ORIGINAL_FILENAME%/$${TR_ORIGINAL_FILENAME}/" \
	-e "s/%HAVE_ICON%/$${HAVE_ICON}/" \
	$< > $@

transmission-og-show.res: transmission-og-show.rc ../libtransmission/version.h
	$(WINDRES) -J rc -I $(top_srcdir) -i $< -O coff -o $@

BUILT_SOURCES += transmission-og-show.res
CLEANFILES += transmission-og-show.rc transmission-og-show.res
transmission_og_show_LDADD += transmission-og-show.res
endif #WIN32
