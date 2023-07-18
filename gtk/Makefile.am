BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST = CMakeLists.txt
SUBDIRS = icons

if TR_UNSTABLE
#FIXME: migrate from GtkUIManager to GtkBuilder in 2.90
#GTK_EXTRA_CPPFLAGS=-DG_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DPANGO_DISABLE_DEPRECATED
GTK_EXTRA_CPPFLAGS=-DG_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DPANGO_DISABLE_DEPRECATED
endif

# these should go in GTK_EXTRA_CPPFLAGS at some point, but not yet because it breaks libnotify's headers
# -DG_DISABLE_SINGLE_INCLUDES -DGTK_DISABLE_SINGLE_INCLUDES

AM_CPPFLAGS = \
    -I$(top_srcdir) \
    -DTRANSMISSIONLOCALEDIR=\""$(transmissionlocaledir)"\" \
    $(GTK_EXTRA_CPPFLAGS)

AM_CFLAGS = \
    @LIBAPPINDICATOR_CFLAGS@ \
    @LIBEVENT_CFLAGS@ \
    @GTK_CFLAGS@ \
    @LIBCURL_CFLAGS@ \
    @ZLIB_CFLAGS@ \
    @PTHREAD_CFLAGS@

bin_PROGRAMS = transmission-og-gtk

transmission_og_gtk_SOURCES = \
    actions.c \
    conf.c \
    details.c \
    dialogs.c \
    favicon.c \
    file-list.c \
    filter.c \
    hig.c \
    icons.c \
    main.c \
    makemeta-ui.c \
    msgwin.c \
    notify.c \
    open-dialog.c \
    relocate.c \
    stats.c \
    torrent-cell-renderer.c \
    transmission-resources.c \
    tr-core.c \
    tr-icon.c \
    tr-prefs.c \
    tr-window.c \
    util.c

transmission_og_gtk_SOURCES += \
    actions.h \
    conf.h \
    details.h \
    dialogs.h \
    favicon.h \
    file-list.h \
    filter.h \
    hig.h \
    icons.h \
    makemeta-ui.h \
    msgwin.h \
    notify.h \
    open-dialog.h \
    relocate.h \
    stats.h \
    torrent-cell-renderer.h \
    transmission-resources.h \
    tr-core.h \
    tr-icon.h \
    tr-prefs.h \
    tr-window.h \
    util.h \
    ../libtransmission/version.h

nodist_EXTRA_transmission_og_gtk_SOURCES = dummy.cxx # force C++ linking

# Manual page.
dist_man_MANS = transmission-og-gtk.1

transmission_og_gtk_LDADD = \
    $(top_builddir)/libtransmission/libtransmission-og.la \
    @LIBUPNP_LIBS@ \
    @LIBNATPMP_LIBS@ \
    @DHT_LIBS@ \
    @LIBB64_LIBS@ \
    @LIBUTP_LIBS@ \
    @GTK_LIBS@ \
    @LIBAPPINDICATOR_LIBS@ \
    @LIBEVENT_LIBS@ \
    @LIBCURL_LIBS@ \
    @CRYPTO_LIBS@ \
    @ZLIB_LIBS@ \
    @PTHREAD_LIBS@ \
    $(LIBM)

# Desktop integration.
DESKTOP_IN_FILES=transmission-og-gtk.desktop.in
DESKTOP_FILES=$(DESKTOP_IN_FILES:.desktop.in=.desktop)
MAINTAINERCLEANFILES = $(DESKTOP_FILES)
# "mingw32-make" cannot deal with an absolute UNIX path in a rule dependency
# (INTLTOOL_MERGE is "/usr/bin/intltool-merge"), so we copy the relevant
# rules from intltool.m4 and remove that useless dep.
#INTLTOOL_DESKTOP_RULE
%.desktop:   %.desktop.in   $(wildcard $(top_srcdir)/po/*.po) ; $(INTLTOOL_V_MERGE)LC_ALL=C $(INTLTOOL_MERGE) $(INTLTOOL_V_MERGE_OPTIONS) -d -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< $@

Productivitydir = $(datadir)/applications
Productivity_DATA = $(DESKTOP_FILES)

APPDATA_IN_FILES=transmission-og-gtk.appdata.xml.in
APPDATA_FILES=$(APPDATA_IN_FILES:.xml.in=.xml)
#INTLTOOL_XML_RULE
%.xml:       %.xml.in       $(wildcard $(top_srcdir)/po/*.po) ; $(INTLTOOL_V_MERGE)LC_ALL=C $(INTLTOOL_MERGE) $(INTLTOOL_V_MERGE_OPTIONS) -x -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< $@

appdatadir = $(datadir)/metainfo
appdata_DATA = $(APPDATA_FILES)

icon_DATA = transmission-og.png
icondir = $(datadir)/pixmaps

UI_FILES = transmission-ui.xml

transmission-resources.c: transmission.gresource.xml $(UI_FILES)
	$(AM_V_GEN)glib-compile-resources --target=$@ --sourcedir=$(srcdir) \
	  --generate-source --c-name transmission $<

transmission-resources.h: transmission.gresource.xml $(UI_FILES)
	$(AM_V_GEN)glib-compile-resources --target=$@ --sourcedir=$(srcdir) \
	  --generate-header --c-name transmission $<

CLEANFILES += \
	transmission-resources.c \
	transmission-resources.h
BUILT_SOURCES += \
	transmission-resources.c \
	transmission-resources.h

EXTRA_DIST += \
	$(DESKTOP_IN_FILES) \
	$(APPDATA_IN_FILES) \
	$(UI_FILES) \
	transmission.gresource.xml \
	transmission-og.ico \
	transmission-og.png

DISTCLEANFILES = \
	$(DESKTOP_FILES) \
	$(APPDATA_FILES)

# Windows resource file.
if WIN32
transmission-og-gtk.rc: ../transmission-og.rc.in
	TR_FILE_DESCRIPTION="Transmission OG GTK+ Client"; \
	TR_INTERNAL_NAME="$(PACKAGE_NAME)-gtk"; \
	TR_ORIGINAL_FILENAME="$(PACKAGE_NAME)-gtk.exe"; \
	HAVE_ICON=1; \
	TR_MAIN_ICON="$(PACKAGE_NAME).ico"; \
	sed \
	-e "s/%TR_FILE_DESCRIPTION%/$${TR_FILE_DESCRIPTION}/" \
	-e "s/%TR_INTERNAL_NAME%/$${TR_INTERNAL_NAME}/" \
	-e "s/%TR_ORIGINAL_FILENAME%/$${TR_ORIGINAL_FILENAME}/" \
	-e "s/%HAVE_ICON%/$${HAVE_ICON}/" \
	-e "s/%TR_MAIN_ICON%/$${TR_MAIN_ICON}/" \
	$< > $@

transmission-og-gtk.res: transmission-og-gtk.rc ../libtransmission/version.h
	$(WINDRES) -J rc -I $(top_srcdir) -I $(srcdir) -i $< -O coff -o $@

BUILT_SOURCES += transmission-og-gtk.res
CLEANFILES += transmission-og-gtk.rc transmission-og-gtk.res
transmission_og_gtk_LDADD += transmission-og-gtk.res
endif #WIN32
