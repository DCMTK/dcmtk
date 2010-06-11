#! /bin/sh

parentdir=`pwd`
thisdir=$parentdir

# The following test constructs relative path from the module
# directory to the configuration directory. If you know this path
# you can substitute this with
# configdir=<relative_path>
# It is very important that the configdir path is relative.

configdir="configdir"

while test "$parentdir" != "/" -a "$configdir" = "configdir"; do
	if test -d "$parentdir/config" ; then
		configdir=$parentdir/config
	else
		parentdir=`echo $parentdir | sed 's/\/[^\/]*$//'`
	fi
done

if test "$configdir" = "configdir" ; then
	echo "Cannot find configure directory"
	exit 1
fi

if test $# != 0;  then
	case $1 in
	-a)
		shift
		cd "$configdir"
		echo "running configure in config-directory"
		./configure $*
		cd $thisdir
		echo "running configure for this module"
		sh "$configdir/confmod" --srcdir=. $*
		;;
	-c)
		shift
		cd "$configdir"
		echo "running configure in config-directory"
		./configure $*
		;;
	*)
		echo "running configure for this module"
		sh "$configdir/confmod" --srcdir=. $*
		;;
	esac
else
	echo "running configure for this module"
	sh "$configdir/confmod" --srcdir=. $*
fi
