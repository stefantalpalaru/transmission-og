#!/bin/bash

set -e

cd /home/user/transmission-og

if [[ -z "${1}" ]]; then
	echo "Usage: $(basename ${0}) PLATFORM"
	exit 1
fi
PLATFORM="${1}"

echo -e "\nPLATFORM=${PLATFORM}"

CONFIGURE_ARGS=(
	--disable-shared
	--enable-cli
	--with-crypto=openssl
	--with-gtk
	--without-qt
)
NCPU="$(nproc)"
export CFLAGS="-O3 -pipe"
export CXXFLAGS="${CFLAGS}"
export LDFLAGS="-Wl,-O1 -Wl,--as-needed -Wl,--sort-common"

if [[ "${PLATFORM}" == "Windows_amd64" ]]; then
	# Cross-compilation using the MXE distribution of Mingw-w64
	export PATH="/opt/mxe/usr/bin:${PATH}"
	export CC=x86_64-w64-mingw32.static-gcc
	export CXX=x86_64-w64-mingw32.static-g++
	${CC} --version
	echo

	# For reproducible builds:
	export LDFLAGS="${LDFLAGS} -Wl,--no-insert-timestamp"

	./configure --host=x86_64-w64-mingw32.static --prefix="$(pwd)/tmp_install" "${CONFIGURE_ARGS[@]}"
	make clean
	make -j ${NCPU} V=1
	rm -rf tmp_install
	for DIR in cli daemon gtk utils web; do
		make -C "${DIR}" install
	done

	# Some GTK+ resources we cannot statically link.
	mkdir -p tmp_install/share/glib-2.0
	cp -a /opt/mxe/usr/x86_64-w64-mingw32.static/share/glib-2.0/schemas tmp_install/share/glib-2.0/
	#cp -a /opt/mxe/usr/x86_64-w64-mingw32.static/share/icons tmp_install/share/
	cp -a /opt/mxe/usr/x86_64-w64-mingw32.static/bin/gdbus.exe tmp_install/bin/
fi

if [[ -d .git ]]; then
	GIT_COMMIT="$(git rev-parse --short=8 HEAD)"
elif [[ -f REVISION ]]; then
	GIT_COMMIT="$(cat REVISION)"
else
	GIT_COMMIT="0"
fi

sed -e "s/GIT_COMMIT/${GIT_COMMIT}/" docker/build_image/README.md.tpl > "tmp_install/README.md"

if [[ "${PLATFORM}" == "Windows_amd64" ]]; then
  sed -i -e 's/^make bindist$/make bindist-win64/' "tmp_install/README.md"
fi

cd tmp_install/bin
echo '```text' >> "../README.md"
sha256sum transmission-og-* >> "../README.md"
echo '```' >> "../README.md"
cd - &>/dev/null
