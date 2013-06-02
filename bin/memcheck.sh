#!/bin/bash

VALGRIND_OPTS="--show-reachable=yes --leak-check=full"
export PATH=.:${PATH}
for cmd in $@; do
    \valgrind ${VALGRIND_OPTS} ${cmd}
done
