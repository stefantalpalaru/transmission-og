BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST = CMakeLists.txt

AM_CPPFLAGS = \
  -I$(top_srcdir) \
  -D__TRANSMISSION__ \
  -DPACKAGE_DATA_DIR=\""$(datadir)"\"

AM_CFLAGS = \
  @DHT_CFLAGS@ \
  @LIBB64_CFLAGS@ \
  @LIBUTP_CFLAGS@ \
  @LIBUPNP_CFLAGS@ \
  @LIBNATPMP_CFLAGS@ \
  @LIBEVENT_CFLAGS@ \
  @LIBCURL_CFLAGS@ \
  @CRYPTO_CFLAGS@ \
  @PTHREAD_CFLAGS@ \
  @ZLIB_CFLAGS@

lib_LTLIBRARIES = libtransmission-og.la

# https://autotools.info/libtool/version.html:
# Version format: "current:revision:age"
# - Increase the "current" value whenever an interface has been added, removed or changed.
# - Always increase the "revision" value.
# - Increase the "age" value only if the changes made to the ABI are backward compatible
libtransmission_og_la_LDFLAGS = -version-info 3:7:0

libtransmission_og_la_LIBADD = \
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

libtransmission_og_la_SOURCES = \
  announcer.c \
  announcer-http.c \
  announcer-udp.c \
  bandwidth.c \
  bitfield.c \
  blocklist.c \
  cache.c \
  clients.c \
  completion.c \
  ConvertUTF.c \
  crypto.c \
  crypto-utils.c \
  crypto-utils-fallback.c \
  error.c \
  fdlimit.c \
  file.c \
  handshake.c \
  history.c \
  inout.c \
  list.c \
  log.c \
  magnet.c \
  makemeta.c \
  metainfo.c \
  natpmp.c \
  net.c \
  peer-io.c \
  peer-mgr.c \
  peer-msgs.c \
  platform.c \
  platform-quota.c \
  port-forwarding.c \
  ptrarray.c \
  quark.c \
  resume.c \
  rpcimpl.c \
  rpc-server.c \
  session.c \
  session-id.c \
  stats.c \
  torrent.c \
  torrent-ctor.c \
  torrent-magnet.c \
  tr-assert.c \
  tr-dht.c \
  tr-lpd.c \
  tr-udp.c \
  tr-utp.c \
  tr-getopt.c \
  trevent.c \
  upnp.c \
  utils.c \
  variant.c \
  variant-benc.c \
  variant-json.c \
  verify.c \
  watchdir.c \
  watchdir-generic.c \
  web.c \
  webseed.c \
  whereami.c \
  wildmat.c

if USE_INOTIFY
libtransmission_og_la_SOURCES += watchdir-inotify.c
AM_CPPFLAGS += -DWITH_INOTIFY
endif

if USE_KQUEUE
libtransmission_og_la_SOURCES += watchdir-kqueue.c
AM_CPPFLAGS += -DWITH_KQUEUE
endif

if WIN32
libtransmission_og_la_SOURCES += file-win32.c subprocess-win32.c watchdir-win32.c
else
libtransmission_og_la_SOURCES += file-posix.c subprocess-posix.c
endif

if CRYPTO_USE_OPENSSL
libtransmission_og_la_SOURCES += crypto-utils-openssl.c
endif
if CRYPTO_USE_CYASSL
libtransmission_og_la_SOURCES += crypto-utils-cyassl.c
if CYASSL_IS_WOLFSSL
AM_CPPFLAGS += -DCYASSL_IS_WOLFSSL
endif
endif
if CRYPTO_USE_POLARSSL
libtransmission_og_la_SOURCES += crypto-utils-polarssl.c
if POLARSSL_IS_MBEDTLS
AM_CPPFLAGS += -DPOLARSSL_IS_MBEDTLS
endif
endif

pkginclude_HEADERS = \
  error.h \
  error-types.h \
  file.h \
  log.h \
  makemeta.h \
  quark.h \
  rpcimpl.h \
  session-id.h \
  tr-assert.h \
  tr-getopt.h \
  tr-macros.h \
  transmission.h \
  utils.h \
  variant.h \
  watchdir.h \
  web.h \
  version.h

libtransmission_og_la_SOURCES += \
  $(pkginclude_HEADERS) \
  announcer.h \
  announcer-common.h \
  bandwidth.h \
  bitfield.h \
  blocklist.h \
  cache.h \
  clients.h \
  ConvertUTF.h \
  crypto.h \
  crypto-utils.h \
  completion.h \
  fdlimit.h \
  handshake.h \
  history.h \
  inout.h \
  jsonsl.h \
  libtransmission-test.h \
  list.h \
  magnet.h \
  metainfo.h \
  natpmp_local.h \
  net.h \
  peer-common.h \
  peer-io.h \
  peer-mgr.h \
  peer-msgs.h \
  peer-socket.h \
  platform.h \
  platform-quota.h \
  port-forwarding.h \
  ptrarray.h \
  resume.h \
  rpc-server.h \
  session.h \
  stats.h \
  subprocess.h \
  torrent.h \
  torrent-magnet.h \
  tr-dht.h \
  tr-udp.h \
  tr-utp.h \
  tr-lpd.h \
  trevent.h \
  upnp.h \
  variant-common.h \
  verify.h \
  watchdir-common.h \
  webseed.h \
  whereami.h

# Weird inclusion of a C file in another one.
noinst_HEADERS = \
  jsonsl.c

BUILT_SOURCES += version.h
CLEANFILES += version.h
EXTRA_DIST += version.h.in
version.h:
	cd $(top_srcdir); ./update-version-h.sh

# For CMake.
EXTRA_DIST += \
	      subprocess-test.cmd

TESTS = \
  bitfield-test \
  blocklist-test \
  clients-test \
  crypto-test \
  error-test \
  file-test \
  history-test \
  json-test \
  magnet-test \
  makemeta-test \
  metainfo-test \
  move-test \
  peer-msgs-test \
  quark-test \
  rename-test \
  rpc-test \
  session-test \
  subprocess-test \
  tr-getopt-test \
  utils-test \
  variant-test \
  watchdir-test \
  watchdir-generic-test

check_PROGRAMS = $(TESTS)

apps_ldadd = \
  ./libtransmission-og.la \
  @LIBEVENT_LIBS@

TEST_SOURCES = libtransmission-test.c

bitfield_test_SOURCES = bitfield-test.c $(TEST_SOURCES)
nodist_EXTRA_bitfield_test_SOURCES = dummy.cxx # force C++ linking
bitfield_test_LDADD = $(apps_ldadd)
bitfield_test_LDFLAGS = $(apps_ldflags)

blocklist_test_SOURCES = blocklist-test.c $(TEST_SOURCES)
nodist_EXTRA_blocklist_test_SOURCES = dummy.cxx # force C++ linking
blocklist_test_LDADD = $(apps_ldadd)
blocklist_test_LDFLAGS = $(apps_ldflags)

clients_test_SOURCES = clients-test.c $(TEST_SOURCES)
nodist_EXTRA_clients_test_SOURCES = dummy.cxx # force C++ linking
clients_test_LDADD = $(apps_ldadd)
clients_test_LDFLAGS = $(apps_ldflags)

crypto_test_SOURCES = crypto-test.c crypto-test-ref.h $(TEST_SOURCES)
nodist_EXTRA_crypto_test_SOURCES = dummy.cxx # force C++ linking
crypto_test_LDADD = $(apps_ldadd)
crypto_test_LDFLAGS = $(apps_ldflags)

error_test_SOURCES = error-test.c $(TEST_SOURCES)
nodist_EXTRA_error_test_SOURCES = dummy.cxx # force C++ linking
error_test_LDADD = $(apps_ldadd)
error_test_LDFLAGS = $(apps_ldflags)

file_test_SOURCES = file-test.c $(TEST_SOURCES)
nodist_EXTRA_file_test_SOURCES = dummy.cxx # force C++ linking
file_test_LDADD = $(apps_ldadd)
file_test_LDFLAGS = $(apps_ldflags)

history_test_SOURCES = history-test.c $(TEST_SOURCES)
nodist_EXTRA_history_test_SOURCES = dummy.cxx # force C++ linking
history_test_LDADD = $(apps_ldadd)
history_test_LDFLAGS = $(apps_ldflags)

json_test_SOURCES = json-test.c $(TEST_SOURCES)
nodist_EXTRA_json_test_SOURCES = dummy.cxx # force C++ linking
json_test_LDADD = $(apps_ldadd)
json_test_LDFLAGS = $(apps_ldflags)

quark_test_SOURCES = quark-test.c $(TEST_SOURCES)
nodist_EXTRA_quark_test_SOURCES = dummy.cxx # force C++ linking
quark_test_LDADD = $(apps_ldadd)
quark_test_LDFLAGS = $(apps_ldflags)

magnet_test_SOURCES = magnet-test.c $(TEST_SOURCES)
nodist_EXTRA_magnet_test_SOURCES = dummy.cxx # force C++ linking
magnet_test_LDADD = $(apps_ldadd)
magnet_test_LDFLAGS = $(apps_ldflags)

metainfo_test_SOURCES = metainfo-test.c $(TEST_SOURCES)
nodist_EXTRA_metainfo_test_SOURCES = dummy.cxx # force C++ linking
metainfo_test_LDADD = $(apps_ldadd)
metainfo_test_LDFLAGS = $(apps_ldflags)

makemeta_test_SOURCES = makemeta-test.c $(TEST_SOURCES)
nodist_EXTRA_makemeta_test_SOURCES = dummy.cxx # force C++ linking
makemeta_test_LDADD = $(apps_ldadd)
makemeta_test_LDFLAGS = $(apps_ldflags)

move_test_SOURCES = move-test.c $(TEST_SOURCES)
nodist_EXTRA_move_test_SOURCES = dummy.cxx # force C++ linking
move_test_LDADD = $(apps_ldadd)
move_test_LDFLAGS = $(apps_ldflags)

peer_msgs_test_SOURCES = peer-msgs-test.c $(TEST_SOURCES)
nodist_EXTRA_peer_msgs_test_SOURCES = dummy.cxx # force C++ linking
peer_msgs_test_LDADD = $(apps_ldadd)
peer_msgs_test_LDFLAGS = $(apps_ldflags)

rpc_test_SOURCES = rpc-test.c $(TEST_SOURCES)
nodist_EXTRA_rpc_test_SOURCES = dummy.cxx # force C++ linking
rpc_test_LDADD = $(apps_ldadd)
rpc_test_LDFLAGS = $(apps_ldflags)

session_test_SOURCES = session-test.c $(TEST_SOURCES)
nodist_EXTRA_session_test_SOURCES = dummy.cxx # force C++ linking
session_test_LDADD = $(apps_ldadd)
session_test_LDFLAGS = $(apps_ldflags)

subprocess_test_SOURCES = subprocess-test.c $(TEST_SOURCES)
nodist_EXTRA_subprocess_test_SOURCES = dummy.cxx # force C++ linking
subprocess_test_LDADD = $(apps_ldadd)
subprocess_test_LDFLAGS = $(apps_ldflags)

tr_getopt_test_SOURCES = tr-getopt-test.c $(TEST_SOURCES)
nodist_EXTRA_tr_getopt_test_SOURCES = dummy.cxx # force C++ linking
tr_getopt_test_LDADD = $(apps_ldadd)
tr_getopt_test_LDFLAGS = $(apps_ldflags)

utils_test_SOURCES = utils-test.c $(TEST_SOURCES)
nodist_EXTRA_utils_test_SOURCES = dummy.cxx # force C++ linking
utils_test_LDADD = $(apps_ldadd)
utils_test_LDFLAGS = $(apps_ldflags)

variant_test_SOURCES = variant-test.c $(TEST_SOURCES)
nodist_EXTRA_variant_test_SOURCES = dummy.cxx # force C++ linking
variant_test_LDADD = $(apps_ldadd)
variant_test_LDFLAGS = $(apps_ldflags)

watchdir_test_SOURCES = watchdir-test.c $(TEST_SOURCES)
nodist_EXTRA_watchdir_test_SOURCES = dummy.cxx # force C++ linking
watchdir_test_LDADD = $(apps_ldadd)
watchdir_test_LDFLAGS = $(apps_ldflags)

watchdir_generic_test_SOURCES = watchdir-test.c $(TEST_SOURCES)
nodist_EXTRA_watchdir_generic_test_SOURCES = dummy.cxx # force C++ linking
watchdir_generic_test_LDADD = $(apps_ldadd)
watchdir_generic_test_LDFLAGS = $(apps_ldflags)
watchdir_generic_test_CPPFLAGS = -DWATCHDIR_TEST_FORCE_GENERIC $(AM_CPPFLAGS)

rename_test_SOURCES = rename-test.c $(TEST_SOURCES)
nodist_EXTRA_rename_test_SOURCES = dummy.cxx # force C++ linking
rename_test_LDADD = $(apps_ldadd)
rename_test_LDFLAGS = $(apps_ldflags)
