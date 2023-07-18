# Binary Transmission OG distribution

This binary distribution of the Transmission OG package is compiled
in a [reproducible way](https://reproducible-builds.org/) from source files
hosted at https://github.com/stefantalpalaru/transmission-og.

## Reproducing the build

Besides the generic build requirements, you also need [Docker](https://www.docker.com/).

```bash
git clone https://github.com/stefantalpalaru/transmission-og.git
cd transmission-og
git checkout GIT_COMMIT
git submodule update --init --recursive
./configure
make bindist
```

You can now verify the checksums:

```bash
cd tmp_install/bin
sha256sum transmission-og-*
```

You should see this output:

