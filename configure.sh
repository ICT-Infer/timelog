#!/usr/bin/env sh

host_os=$( uname -s )
if [ "$host_os" = "Darwin" ] ; then
echo DARWIN=true >config.mk
else
:>config.mk
fi

patch -o GNUmakefile BSDmakefile GNUmakefile.patch
