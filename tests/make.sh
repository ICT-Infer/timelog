#!/usr/bin/env sh

rm -rf ../build/unit/

find unit -type d -exec mkdir -p ../build/{} \;
find unit -type f -name \*.c -exec echo ../build/{}.o {} \; \
  | sed 's/\.c\.o/.o/' \
  | xargs -L1 cc -c -Wall -ansi -pedantic -O0 -g -Iinclude/ -o
find ../build/unit/ -type f -name \*.o \
  | xargs cc -Wall -pedantic -O0 -g -o ../build/unit/test-runner
