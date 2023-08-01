ACLOCAL_AMFLAGS = -I m4
AM_DISTCHECK_CONFIGURE_FLAGS = --enable-cli

if BUILD_CLI
  CLI_DIR = cli
endif

if BUILD_DAEMON
  DAEMON_DIR = daemon
endif

if BUILD_GTK
  GTK_DIR = gtk po
endif

if BUILD_QT
  TR_QT_DIR = qt
endif

if BUILD_MAC
  MAC_DIR = macosx
endif

SUBDIRS = \
  third-party \
  libtransmission \
  utils \
  $(CLI_DIR) \
  $(DAEMON_DIR) \
  $(GTK_DIR) \
  $(TR_QT_DIR) \
  $(MAC_DIR) \
  web

# We need to distribute all these subdirs unconditionally, so the "$(.._DIR)" scheme above is not suitable.
DIST_SUBDIRS = \
  third-party \
  libtransmission \
  utils \
  cli \
  daemon \
  gtk po \
  qt \
  web

# Whatever the GitHub Actions Windows image is doing with Git symlinks, copying
# them during a "make distcheck" is broken beyond repair. We only care about
# "distcheck" on Windows in the context of CI testing. The source archive we
# actually distribute is only generated on Linux.
if !WIN32
DIST_SUBDIRS += \
  macosx
endif

EXTRA_DIST = \
  AUTHORS \
  CMakeLists.txt \
  COPYING \
  NEWS.md \
  README.md \
  Transmission.xcodeproj/project.pbxproj \
  autogen.sh \
  cmake \
  docker \
  extras \
  installers \
  m4 \
  po/CMakeLists.txt \
  transmission-og.rc.in \
  update-version-h.sh \
  utils/CMakeLists.txt

if HAVE_REVISION_FILE
  EXTRA_DIST += REVISION
endif

dist-hook:
	rm -rf "$(distdir)/installers/installer"

# Do a CMake build during "make distcheck".
# Notice how we get "ctest" to run the tests in parallel by setting "ARGS" so
# it contains any "-jN" argument passed to "make". Luckily, "ctest" ignores
# unknown arguments.
CMAKE_ARGS = \
	     -DENABLE_CLI=ON \
	     -DUSE_SYSTEM_B64=OFF \
	     -DUSE_SYSTEM_DHT=OFF \
	     -DUSE_SYSTEM_EVENT2=OFF \
	     -DUSE_SYSTEM_MINIUPNPC=OFF \
	     -DUSE_SYSTEM_NATPMP=OFF \
	     -DUSE_SYSTEM_UTP=OFF
if WIN32
CMAKE_ARGS += -G "MinGW Makefiles"
endif
if MACOS
CMAKE_ARGS += \
	      -DENABLE_MAC=ON \
	      -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl
endif
# A qmake build still errors out on macOS, with this ridiculous message:
# "ld: warning: ignoring file ../libtransmission/.libs/libtransmission-og.a, building for macOS-x86_64 but attempting to link with file built for macOS-x86_64"
distcheck-hook:
	@ echo "*** CMake build ***"
	+ TMPDIR="$(abs_top_builddir)/build" && \
		rm -rf "$${TMPDIR}" && \
		mkdir "$${TMPDIR}" && \
		cd "$${TMPDIR}" && \
		cmake $(CMAKE_ARGS) "../$(distdir)" && \
		$(MAKE) VERBOSE=$(V) && \
		$(MAKE) VERBOSE=$(V) ARGS="$(MAKEFLAGS)" test && \
		rm -rf "$${TMPDIR}"
if !MACOS
	@ echo "*** qmake build ***"
	+ TMPDIR="$(abs_top_builddir)/build" && \
		rm -rf "$${TMPDIR}" && \
		mkdir "$${TMPDIR}" && \
		cd "$${TMPDIR}" && \
		"../$(distdir)/configure" && \
		$(MAKE) -C qt qmake-build && \
		rm -rf "$${TMPDIR}"
endif

DISTCLEANFILES = \
  intltool-extract \
  intltool-merge \
  intltool-update 

distuninstallcheck:
	@:

.PHONY: tags
tags:
	ctags -R \
		--exclude="*.md" \
		--exclude="*/icons" \
		--exclude="installers" \
		--exclude="tmp_install" \
		--exclude="web" \
		.

# List all Git submodule URLs and commit hashes, including the nested ones.
.PHONY: list-submodules
list-submodules:
	@{ "$(srcdir)/extras/git_show_deps.sh" ;\
		git submodule foreach --quiet --recursive "$(CURDIR)/extras/git_show_deps.sh"; } \
		| sort -u

# If you want to use SSH keys with GitHub, for this project and all its submodules.
.PHONY: github-ssh
github-ssh:
	git config url."git@github.com:".insteadOf "https://github.com/"
	git submodule foreach --recursive 'git config url."git@github.com:".insteadOf "https://github.com/"'

.PHONY: bindist-win64
bindist-win64:
	+ MAKE="$(MAKE)" \
		docker/build_image/make_bindist.sh win64
	ls -lh tmp_install/bin

.PHONY: installer-win64
installer-win64: bindist-win64
	rm -rf installers/installer
	wine installers/InnoSetup/ISCC.exe installers/transmission-og.iss
	ls -lh installers/installer
