#!/bin/sh
#valgrind --tool=cachegrind ./transmission-og-qt 2>&1 | tee runlog
#valgrind --tool=massif --threshold=0.2 ./transmission-og-qt 2>&1 | tee runlog
valgrind --tool=memcheck --leak-check=full --leak-resolution=high --num-callers=16 --log-file=x-valgrind --show-reachable=no ./transmission-og-qt 2>&1 | tee runlog
