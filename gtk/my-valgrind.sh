#!/bin/sh
export G_SLICE=always-malloc
export G_DEBUG=gc-friendly
export GLIBCXX_FORCE_NEW=1
#valgrind --tool=cachegrind ./transmission-og-gtk 2>&1 | tee runlog
#valgrind --tool=cachegrind ./transmission-og-gtk -p -g /tmp/transmission-og-test 2>&1 | tee runlog
valgrind --tool=memcheck --leak-check=full --leak-resolution=high --num-callers=48 --log-file=x-valgrind --show-reachable=no ./transmission-og-gtk -p 2>&1 | tee runlog
