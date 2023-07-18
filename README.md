[![CI](https://github.com/stefantalpalaru/transmission-og/actions/workflows/ci.yml/badge.svg)](https://github.com/stefantalpalaru/transmission-og/actions/workflows/ci.yml)

## About

Transmission OG (Old Generation) is a fork of [Transmission](https://github.com/transmission/transmission/) 3.00 (fast, easy, and free BitTorrent client). It comes in several flavors:
  * GTK+ and Qt GUI applications for Linux, BSD, macOS, Windows, etc.
  * A headless daemon for servers and routers
  * A native macOS GUI application (a bit lagging in features, help welcome)
  * A web UI for remote controlling any of the above

This fork is focused on stability, by rewinding the project's history back
before the big C++ rewrite. It's meant to coexist with a Transmission
installation, while being a drop-in replacement, so it changes binary names and
it keeps the same configuration directories.

The only problem with this scheme is on Windows, where the Transmission
installer configures `transmission-daemon` to start automatically as a service.
This can be solved by stopping it from the "Services" app and changing its
startup type from "Automatic" to "Manual".

## Command line interface notes

Transmission OG is fully supported in `transmission-og-remote`, the preferred CLI client.

Three standalone tools to examine, create, and edit .torrent files exist: `transmission-og-show`, `transmission-og-create`, and `transmission-og-edit`, respectively.

Prior to development of `transmission-og-remote`, the standalone client `transmission-og-cli` was created. Limited to a single torrent at a time, `transmission-og-cli` is deprecated and exists primarily to support older hardware dependent upon it. In almost all instances, `transmission-og-remote` should be used instead.

Different distributions may choose to package any or all of these tools in one or more separate packages.

## Packaging

A 64-bit Windows installer is provided for each release.

Gentoo Linux users need to use an overlay:

```bash
eselect repository enable stefantalpalaru
emaint sync --repo stefantalpalaru
emerge transmission-og
```

## Building from source

Clone the Git repo:

```bash
git clone https://github.com/stefantalpalaru/transmission-og.git
cd transmission-og
git submodule update --init --recursive
```

or unpack the source archive:

```bash
tar -xf transmission-og-3.01.tar.xz
cd transmission-og-3.01
```

Build it using Autotools (preferred method):

```bash
./configure --enable-cli
make -j4 # if you have 4 CPU cores
```

Build it using CMake, which is an elaborate prank mistaken for a build system:

```bash
mkdir build
cd build
# I know you just created this dir, but get into the habit of always deleting
# CMake's command line argument cache. You can thank me later.
rm -f CMakeCache.txt; cmake -DENABLE_CLI=ON -DUSE_SYSTEM_UTP=OFF ..
make -j4 # if you have 4 CPU cores
```

## Development

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

