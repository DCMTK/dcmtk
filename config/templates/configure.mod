#! /bin/sh

if test -d config ; then
	configdir=config
elif test -d ../config ; then
	configdir=../config
else
	echo "Cannot find configure directory (config or ../config)"
	exit 1
fi
$configdir/configure -srcdir=. --cache-file=$configdir/config.cache $*
