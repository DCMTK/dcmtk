dnl This file is includes into configure.in and modules.in

dnl Get configuration directory
configdir=`pwd` 
AC_PROG_MAKE_SET
AC_CONFIG_AUX_DIR($configdir)
AC_PREFIX_DEFAULT(/usr/local/dicom)
AC_PROG_INSTALL
AC_SUBST(configdir)
common_makefile=Makefile.def
AC_SUBST(common_makefile)

