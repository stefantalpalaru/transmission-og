[![CI](https://github.com/stefantalpalaru/transmission-og/actions/workflows/ci.yml/badge.svg)](https://github.com/stefantalpalaru/transmission-og/actions/workflows/ci.yml)
[![distcheck](https://github.com/stefantalpalaru/transmission-og/actions/workflows/distcheck.yml/badge.svg)](https://github.com/stefantalpalaru/transmission-og/actions/workflows/distcheck.yml)
[![stylecheck](https://github.com/stefantalpalaru/transmission-og/actions/workflows/stylecheck.yml/badge.svg)](https://github.com/stefantalpalaru/transmission-og/actions/workflows/stylecheck.yml)

## About

Transmission OG (Old Generation) is a fork of
[Transmission](https://github.com/transmission/transmission/) 3.00 (fast, easy
and free BitTorrent client). It comes in several flavours:
  * GTK+ and Qt GUI applications for Linux, BSD, macOS, Windows, etc.
  * a daemon for servers and routers
  * a native macOS GUI application (a bit lagging in features, help welcome)
  * a web UI for remote controlling any of the above

This fork is focused on stability, by rewinding the project's history back
before the big C++ rewrite. It's meant to coexist with a Transmission
installation, while being a drop-in replacement, so it changes binary names and
it keeps the same configuration directories.

The only problem with this scheme is on Windows, where the Transmission
installer configures `transmission-daemon` to start automatically as a service.
This can be solved by stopping it from the "Services" app and changing its
startup type from "Automatic" to "Manual".

## Command line interface notes

Transmission OG is fully supported by `transmission-og-remote`, the preferred CLI client.

Three standalone tools to examine, create, and edit .torrent files exist: `transmission-og-show`, `transmission-og-create`, and `transmission-og-edit`, respectively.

Prior to development of `transmission-og-remote`, the standalone client `transmission-og-cli` was created. Limited to a single torrent at a time, `transmission-og-cli` is deprecated and exists primarily to support older hardware dependent upon it. In almost all instances, `transmission-og-remote` should be used instead.

Different distributions may choose to package any or all of these tools in one or more separate packages.

## Packaging

A 64-bit Windows installer is provided for each release. It has no external
dependencies. All installed binaries are statically linked and
[reproducible](https://reproducible-builds.org/). The GTK+ client was chosen
over the Qt one, because it has more features.

Gentoo Linux users need to use an overlay:

```text
eselect repository enable stefantalpalaru
emaint sync --repo stefantalpalaru
emerge transmission-og
```

## Building from source

### Dependencies

Ubuntu:

```bash
sudo apt-get install --no-install-recommends -yq \
         build-essential \
         gettext \
         intltool \
         libayatana-appindicator3-dev \
         libcurl4-openssl-dev \
         libglib2.0-dev \
         libgtk-3-dev \
         libnotify-dev \
         libssl-dev \
         libsystemd-dev \
         qttools5-dev \
         zlib1g-dev
```

macOS with Homebrew:

```bash
export CPPFLAGS="${CPPFLAGS} -I/usr/local/opt/gettext/include"
export LDFLAGS="${LDFLAGS} -L/usr/local/opt/gettext/lib"
export PATH="${PATH}:/usr/local/opt/qt@5/bin:/usr/local/opt/gettext/bin"
export PKG_CONFIG_PATH=/usr/local/opt/qt@5/lib/pkgconfig:/usr/local/opt/openssl@1.1/lib/pkgconfig
export MACOSX_DEPLOYMENT_TARGET=12.0

brew install \
         autoconf \
         automake \
         cmake \
         gettext \
         gtk+3 \
         intltool \
         openssl@1.1 \
         pkg-config \
         qt@5
```

Windows with MSYS2's Mingw-w64 distribution:

```bash
pacman -Syu \
           base-devel \
           intltool \
           mingw-w64-x86_64-autotools \
           mingw-w64-x86_64-cmake \
           mingw-w64-x86_64-curl \
           mingw-w64-x86_64-gtk3 \
           mingw-w64-x86_64-libnotify \
           mingw-w64-x86_64-openssl \
           mingw-w64-x86_64-qt5-base \
           mingw-w64-x86_64-qt5-tools \
           mingw-w64-x86_64-qt5-winextras \
           mingw-w64-x86_64-toolchain \
           mingw-w64-x86_64-zlib
```

### Building

Clone the Git repo:

```bash
git clone https://github.com/stefantalpalaru/transmission-og.git
cd transmission-og
# Not needed the first time, only after building, pulling and getting errors
# from the submodule update.
git submodule foreach --quiet --recursive "git restore ."
git submodule update --init --recursive
```

Or unpack the source archive:

```bash
tar -xf transmission-og-3.01.tar.xz
cd transmission-og-3.01
```

Build it using Autotools (preferred method):

```bash
./configure --enable-cli
make -j4 # if you have 4 CPU cores
```

(You'll want to use `mingw32-make` on Windows)

Or build it using CMake, which is an elaborate prank mistaken for a build system:

```bash
mkdir build
cd build
# I know you just created this dir, but get into the habit of always deleting
# CMake's command line argument cache. You can thank me later.
rm -f CMakeCache.txt; cmake -DENABLE_CLI=ON -DUSE_SYSTEM_UTP=OFF ..
make -j4 # if you have 4 CPU cores
```

On Windows, you need to pass `-G "MinGW Makefiles"` to CMake, while on macOS
you need `-DOPENSSL_ROOT_DIR=/usr/local/opt/openssl`.

## Translating

Translations for Mac, Qt and GTK+ clients are managed on Transifex. To help
translate, or add a new language, you'll need to [register on Transifex
and join the translation
team](https://help.transifex.com/en/articles/6248698-getting-started-as-a-translator)
for the [Transmission OG
project](https://explore.transifex.com/transmission-og/transmission-og/).

Please open an issue here after updating a translation on Transifex.

## Development

### Code formatting

We are using `clang-format` through the wrapper script `code_style.sh` (`make format` also works).

Vim integration, using [vim-clang-format](https://github.com/rhysd/vim-clang-format):

```vimrc
" in your ~/.vimrc
autocmd defgroup BufNewFile,BufRead /path/to/transmission-og/*.{c,cc,h} setlocal shiftwidth=4 softtabstop=4 expandtab
autocmd defgroup BufNewFile,BufRead /path/to/transmission-og/*.{c,cc,h} ClangFormatAutoEnable
```

Git integration, using a ".git/hooks/pre-commit" with executable permissions:

```bash
#!/bin/sh

root="$(git rev-parse --show-toplevel)"
"${root}/code_style.sh" --check
```

### Updating translations

```text
cd po
make update-po

cd ../qt
/usr/lib64/qt5/bin/lupdate qtr.pro
```

### Version bumping

Needs to be done in: "configure.ac", "CMakeLists.txt" and
"libtransmission/Makefile.am" (for the shared library).

"NEWS.md" also needs a new section.
