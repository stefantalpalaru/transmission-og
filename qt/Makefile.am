# Turns out it is possible to build Qt projects with Autotools.
# Heavily inspired by VLC: https://code.videolan.org/videolan/vlc/-/blob/master/modules/gui/qt/Makefile.am

BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST = CMakeLists.txt

# Some of these come from "qtr.pro", some from the output of a "qmake"-generated Makefile.
AM_CPPFLAGS = \
    -DENABLE_DBUS_INTEROP \
    -DQT_CORE_LIB \
    -DQT_DBUS_LIB \
    -DQT_GUI_LIB \
    -DQT_NETWORK_LIB \
    -DQT_NO_CAST_FROM_ASCII \
    -DQT_NO_DEBUG \
    -DQT_WIDGETS_LIB \
    -DUNICODE \
    -D_UNICODE \
    -I$(top_srcdir) \
    -I/usr/local/include

if WIN32
AM_CPPFLAGS += \
    -DMINGW_HAS_SECURE_API=1 \
    -DQT_DBUS \
    -DQT_NEEDS_QMAIN \
    -DQT_WINEXTRAS_LIB \
    -DWIN32
endif #WIN32

AM_CXXFLAGS = \
    @QT_CXXFLAGS@ \
    @LIBEVENT_CFLAGS@ \
    @LIBCURL_CFLAGS@ \
    @OPENSSL_CFLAGS@

bin_PROGRAMS = transmission-og-qt

# Libraries we link to.
transmission_og_qt_LDADD = \
    $(top_builddir)/libtransmission/libtransmission-og.la \
    @QT_LIBS@ \
    @LIBEVENT_LIBS@

if WIN32
transmission_og_qt_LDADD += \
    -lintl \
    -liconv \
    -lwldap32 \
    -lmingw32 \
    -lqtmain
endif #WIN32

# Regular C++ source files.
transmission_og_qt_SOURCES = \
    AboutDialog.cc \
    AddData.cc \
    Application.cc \
    ColumnResizer.cc \
    DBusInteropHelper.cc \
    DetailsDialog.cc \
    FaviconCache.cc \
    FileTreeDelegate.cc \
    FileTreeItem.cc \
    FileTreeModel.cc \
    FileTreeView.cc \
    FilterBar.cc \
    FilterBarComboBox.cc \
    FilterBarComboBoxDelegate.cc \
    Filters.cc \
    Formatter.cc \
    FreeSpaceLabel.cc \
    IconToolButton.cc \
    InteropHelper.cc \
    InteropObject.cc \
    LicenseDialog.cc \
    MainWindow.cc \
    MakeDialog.cc \
    OptionsDialog.cc \
    PathButton.cc \
    Prefs.cc \
    PrefsDialog.cc \
    RelocateDialog.cc \
    RpcClient.cc \
    RpcQueue.cc \
    Session.cc \
    SessionDialog.cc \
    SqueezeLabel.cc \
    StatsDialog.cc \
    StyleHelper.cc \
    Torrent.cc \
    TorrentDelegate.cc \
    TorrentDelegateMin.cc \
    TorrentFilter.cc \
    TorrentModel.cc \
    TorrentView.cc \
    TrackerDelegate.cc \
    TrackerModel.cc \
    TrackerModelFilter.cc \
    Utils.cc \
    WatchDir.cc

# Regular headers.
transmission_og_qt_SOURCES += \
    AboutDialog.h \
    AddData.h \
    Application.h \
    ColumnResizer.h \
    DBusInteropHelper.h \
    DetailsDialog.h \
    FaviconCache.h \
    FileTreeDelegate.h \
    FileTreeItem.h \
    FileTreeModel.h \
    FileTreeView.h \
    FilterBar.h \
    FilterBarComboBox.h \
    FilterBarComboBoxDelegate.h \
    Filters.h \
    Formatter.h \
    FreeSpaceLabel.h \
    IconToolButton.h \
    InteropHelper.h \
    InteropObject.h \
    LicenseDialog.h \
    MainWindow.h \
    MakeDialog.h \
    OptionsDialog.h \
    PathButton.h \
    Prefs.h \
    PrefsDialog.h \
    RelocateDialog.h \
    RpcClient.h \
    RpcQueue.h \
    Session.h \
    SessionDialog.h \
    SqueezeLabel.h \
    StatsDialog.h \
    StyleHelper.h \
    Torrent.h \
    TorrentDelegate.h \
    TorrentDelegateMin.h \
    TorrentFilter.h \
    TorrentModel.h \
    TorrentView.h \
    TrackerDelegate.h \
    TrackerModel.h \
    TrackerModelFilter.h \
    Utils.h \
    WatchDir.h

# Stand-alone headers.
transmission_og_qt_SOURCES += \
    BaseDialog.h \
    CustomVariantType.h \
    Speed.h \
    Typedefs.h \
    ../libtransmission/version.h

# For CMake.
EXTRA_DIST += \
	      ComInteropHelper.cc \
	      ComInteropHelper.h

# Meta-object code.
%.moc.cpp: %.h
	$(QT_MOC) $(AM_CPPFLAGS) -o $@ $<

nodist_transmission_og_qt_SOURCES = \
    AboutDialog.moc.cpp \
    AddData.moc.cpp \
    Application.moc.cpp \
    ColumnResizer.moc.cpp \
    DBusInteropHelper.moc.cpp \
    DetailsDialog.moc.cpp \
    FaviconCache.moc.cpp \
    FileTreeDelegate.moc.cpp \
    FileTreeItem.moc.cpp \
    FileTreeModel.moc.cpp \
    FileTreeView.moc.cpp \
    FilterBar.moc.cpp \
    FilterBarComboBox.moc.cpp \
    FilterBarComboBoxDelegate.moc.cpp \
    Filters.moc.cpp \
    Formatter.moc.cpp \
    FreeSpaceLabel.moc.cpp \
    IconToolButton.moc.cpp \
    InteropHelper.moc.cpp \
    InteropObject.moc.cpp \
    LicenseDialog.moc.cpp \
    MainWindow.moc.cpp \
    MakeDialog.moc.cpp \
    OptionsDialog.moc.cpp \
    PathButton.moc.cpp \
    Prefs.moc.cpp \
    PrefsDialog.moc.cpp \
    RelocateDialog.moc.cpp \
    RpcClient.moc.cpp \
    RpcQueue.moc.cpp \
    Session.moc.cpp \
    SessionDialog.moc.cpp \
    SqueezeLabel.moc.cpp \
    StatsDialog.moc.cpp \
    StyleHelper.moc.cpp \
    Torrent.moc.cpp \
    TorrentDelegate.moc.cpp \
    TorrentDelegateMin.moc.cpp \
    TorrentFilter.moc.cpp \
    TorrentModel.moc.cpp \
    TorrentView.moc.cpp \
    TrackerDelegate.moc.cpp \
    TrackerModel.moc.cpp \
    TrackerModelFilter.moc.cpp \
    Utils.moc.cpp \
    WatchDir.moc.cpp

# "MakeDialog.cc" includes "MakeDialog.moc" which is generated from itself.
nodist_transmission_og_qt_SOURCES += \
    MakeDialog.moc
MakeDialog.moc: MakeDialog.cc
	$(QT_MOC) $(AM_CPPFLAGS) -o $@ "$(srcdir)/$(patsubst %.moc,%.cc,$@)"

# UI files are used to generate headers.
transmission_og_qt_UI = \
    AboutDialog.ui \
    DetailsDialog.ui \
    LicenseDialog.ui \
    MainWindow.ui \
    MakeDialog.ui \
    MakeProgressDialog.ui \
    OptionsDialog.ui \
    PrefsDialog.ui \
    RelocateDialog.ui \
    SessionDialog.ui \
    StatsDialog.ui
EXTRA_DIST += $(transmission_og_qt_UI)

nodist_transmission_og_qt_SOURCES += \
    ui_AboutDialog.h \
    ui_DetailsDialog.h \
    ui_LicenseDialog.h \
    ui_MainWindow.h \
    ui_MakeDialog.h \
    ui_MakeProgressDialog.h \
    ui_OptionsDialog.h \
    ui_PrefsDialog.h \
    ui_RelocateDialog.h \
    ui_SessionDialog.h \
    ui_StatsDialog.h

ui_%.h: %.ui
	$(QT_UIC) $< > $@

# Qt resources we embed in the resulting binary.
transmission_og_qt_QRC = \
    application.qrc
if WIN32
transmission_og_qt_QRC += \
    icons/Faenza/Faenza.qrc
endif #WIN32
EXTRA_DIST += \
    $(transmission_og_qt_QRC) \
    icons

transmission_og_qt_RES = \
			 translations/transmission_af.ts \
			 translations/transmission_ca.ts \
			 translations/transmission_da.ts \
			 translations/transmission_de.ts \
			 translations/transmission_el.ts \
			 translations/transmission_en.ts \
			 translations/transmission_es.ts \
			 translations/transmission_eu.ts \
			 translations/transmission_fi.ts \
			 translations/transmission_fr.ts \
			 translations/transmission_hu.ts \
			 translations/transmission_id.ts \
			 translations/transmission_it_IT.ts \
			 translations/transmission_ja.ts \
			 translations/transmission_ka.ts \
			 translations/transmission_kk.ts \
			 translations/transmission_ko.ts \
			 translations/transmission_lt.ts \
			 translations/transmission_nb.ts \
			 translations/transmission_nl.ts \
			 translations/transmission_pl.ts \
			 translations/transmission_pt_BR.ts \
			 translations/transmission_pt_PT.ts \
			 translations/transmission_ru.ts \
			 translations/transmission_sl.ts \
			 translations/transmission_sv.ts \
			 translations/transmission_tr.ts \
			 translations/transmission_uk.ts \
			 translations/transmission_zh_CN.ts
EXTRA_DIST += $(transmission_og_qt_RES)

resources.cc: $(transmission_og_qt_QRC) $(transmission_og_qt_RES)
	$(QT_RCC) -o $@ $<
nodist_transmission_og_qt_SOURCES += \
    resources.cc

# Make sure that all these generated source files get built first and are
# cleaned properly.
BUILT_SOURCES += $(nodist_transmission_og_qt_SOURCES)
CLEANFILES += $(nodist_transmission_og_qt_SOURCES)

# Manual page.
dist_man_MANS = transmission-og-qt.1

# Desktop integration.
DESKTOP_FILES = transmission-og-qt.desktop
Productivitydir = $(datadir)/applications
Productivity_DATA = $(DESKTOP_FILES)
EXTRA_DIST += \
	      $(DESKTOP_FILES) \
	      qtr.ico \
	      qtr.pro \
	      qtr.rc

# We need "$(abs_top_builddir)" in here, because "$(top_builddir)" has the
# wrong value in a VPATH build, for some mysterious reason.
QMAKE_ARGS = \
	     -early \
	     QMAKE_CC="$(CC)" \
	     QMAKE_CXX="$(CXX)" \
	     QMAKE_LINK="$(CXX)" \
	     PKG_CONFIG="$(PKG_CONFIG)" \
	     QMAKE_LFLAGS="$(LDFLAGS)" \
	     QMAKE_CXXFLAGS="$(CPPFLAGS) $(CXXFLAGS)" \
	     QMAKE_INCDIR="$(abs_top_builddir)/third-party/libevent/include"
if MACOS
# A qmake build still errors out on macOS, with this ridiculous message:
# "ld: warning: ignoring file ../libtransmission/.libs/libtransmission-og.a, building for macOS-x86_64 but attempting to link with file built for macOS-x86_64"
QMAKE_ARGS += \
	      QMAKE_MACOSX_DEPLOYMENT_TARGET="$(MACOSX_DEPLOYMENT_TARGET)"
endif
.PHONY: qmake-build
qmake-build:
	+ $(QMAKE) $(QMAKE_ARGS) -o Makefile.qmake "$(srcdir)/qtr.pro" && \
		$(MAKE) -C ../third-party && \
		$(MAKE) -C ../libtransmission && \
		$(MAKE) -f Makefile.qmake
CLEANFILES += Makefile.qmake

# This is the stock rule with $(BUILT_SOURCES) dependency removed.
# It's supposed to be generated by the "no-dist-built-sources" Automake option
# (https://git.savannah.gnu.org/cgit/automake.git/commit/?id=13659a7385)
# but, for some reason, automake-1.16.5 fails to do that.
distdir:
	$(MAKE) $(AM_MAKEFLAGS) distdir-am

# Windows resource file.
if WIN32
transmission-og-qt.rc: ../transmission-og.rc.in
	TR_FILE_DESCRIPTION="Transmission OG Qt Client"; \
	TR_INTERNAL_NAME="$(PACKAGE_NAME)-qt"; \
	TR_ORIGINAL_FILENAME="$(PACKAGE_NAME)-qt.exe"; \
	HAVE_ICON=1; \
	TR_MAIN_ICON="qtr.ico"; \
	sed \
	-e "s/%TR_FILE_DESCRIPTION%/$${TR_FILE_DESCRIPTION}/" \
	-e "s/%TR_INTERNAL_NAME%/$${TR_INTERNAL_NAME}/" \
	-e "s/%TR_ORIGINAL_FILENAME%/$${TR_ORIGINAL_FILENAME}/" \
	-e "s/%HAVE_ICON%/$${HAVE_ICON}/" \
	-e "s/%TR_MAIN_ICON%/$${TR_MAIN_ICON}/" \
	$< > $@

transmission-og-qt.res: transmission-og-qt.rc ../libtransmission/version.h
	$(WINDRES) -J rc -I $(top_srcdir) -I $(srcdir) -i $< -O coff -o $@

BUILT_SOURCES += transmission-og-qt.res
CLEANFILES += transmission-og-qt.rc transmission-og-qt.res
transmission_og_qt_LDADD += transmission-og-qt.res
endif #WIN32
