[![CI](https://github.com/stefantalpalaru/transmission-og/actions/workflows/ci.yml/badge.svg)](https://github.com/stefantalpalaru/transmission-og/actions/workflows/ci.yml)

## About

Transmission OG (Old Generation) is a fork of [Transmission](https://github.com/transmission/transmission/) 3.00 (fast, easy, and free BitTorrent client). It comes in several flavors:
  * GTK+ and Qt GUI applications for Linux, BSD, macOS, Windows, etc.
  * A headless daemon for servers and routers
  * A native macOS GUI application (a bit lagging on features, help welcome)
  * A web UI for remote controlling any of the above

This fork is focused on stability, by rewinding the project's history back before the big C++ rewrite.

## Command line interface notes

Transmission OG is fully supported in transmission-og-remote, the preferred CLI client.

Three standalone tools to examine, create, and edit .torrent files exist: transmission-og-show, transmission-og-create, and transmission-og-edit, respectively.

Prior to development of transmission-og-remote, the standalone client transmission-og-cli was created. Limited to a single torrent at a time, transmission-og-cli is deprecated and exists primarily to support older hardware dependent upon it. In almost all instances, transmission-og-remote should be used instead.

Different distributions may choose to package any or all of these tools in one or more separate packages.

## Building

### Building a Transmission OG release from the command line

    $ tar -xf transmission-og-3.01.tar.xz
    $ cd transmission-og-3.01
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make -j4 # if you have 4 CPU cores
    $ sudo make install

### Building Transmission OG from Git (first time)

    $ git clone https://github.com/stefantalpalaru/transmission-og
    $ cd transmission-og
    $ git submodule update --init
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make -j4 # if you have 4 CPU cores
    $ sudo make install

### Building Transmission OG from Git (updating)

    $ cd transmission-og/build
    $ make clean
    $ git pull
    $ git submodule update --init
    $ cmake ..
    $ make -j4 # if you have 4 CPU cores
    $ sudo make install

### macOS native

Transmission OG has an Xcode project file (Transmission.xcodeproj) for building in Xcode.

For a more detailed description, and dependencies, visit the original wiki: https://github.com/transmission/transmission/wiki

## Contributing

### Updating translations

```text
cd po
make update-po

cd ../qt
/usr/lib64/qt5/bin/lupdate qtr.pro
```

