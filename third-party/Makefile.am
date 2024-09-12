if BUILD_DHT
  DHT_DIR = dht
endif
if BUILD_EVENT
  EVENT_DIR = libevent
endif
if BUILD_B64
  B64_DIR = libb64
endif
if BUILD_UTP
  UTP_DIR = libutp
endif
if BUILD_MINIUPNP
  MINIUPNP_DIR = miniupnpc
endif
if BUILD_NATPMP 
  NATPMP_DIR = libnatpmp 
endif 

SUBDIRS = \
  $(DHT_DIR) \
  $(EVENT_DIR) \
  $(B64_DIR) \
  $(UTP_DIR) \
  $(MINIUPNP_DIR) \
  $(NATPMP_DIR)

DIST_SUBDIRS = \
  dht \
  libevent \
  libb64 \
  miniupnpc \
  libnatpmp \
  libutp

EXTRA_DIST = \
  macosx-libevent-config.h \
  macosx-libevent-event-config.h

# We do not want to install anything from our third party subdirs, so we
# override the "install" target with an empty one.
# ("libevent" is the only one giving us trouble here)
install:

