#! /bin/sh

parentdir=`pwd`
configdir="configdir"

while test $parentdir != "/" -a $configdir = "configdir"; do
	if test -d $parentdir/config ; then
		configdir=$parentdir/config
	else
		parentdir=`echo $parentdir | sed 's/\/[^\/]*$//'`
	fi
done

if test $configdir = "configdir" ; then
	echo "Cannot find configure directory"
	exit 1
fi

src_dir=`pwd`

if test $# != 0;  then
	case $1 in
	-a)
		shift
		cd $configdir
		echo "running configure in config-direktory"
		./configure $*
		echo "running configure for this module"
		sh confmod -srcdir=$src_dir --cache-file=$src_dir/config.cache $* 
		;;
	-c)
		shift
		cd $configdir
		echo "running configure in config-direktory"
		./configure $*
		;;
	*)
		cd $configdir
		echo "run configure for this module"
		sh confmod -srcdir=$src_dir --cache-file=$src_dir/config.cache $* 
		;;
	esac
else
	cd $configdir
	echo "running configure for this module"
	sh confmod -srcdir=$src_dir --cache-file=$src_dir/config.cache $* 
fi	




