#!/usr/bin/env sh

host_os=$( uname -s )
if [ "$host_os" = "Darwin" ] ; then
echo DARWIN= >config.mk
else
:>config.mk
fi
