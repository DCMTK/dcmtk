
dnl 
dnl Filename: aclocal.m4
dnl
dnl Purpose: additional M4 macros for GNU autoconf
dnl
dnl Authors: Andreas Barth, Marco Eichelberg
dnl
dnl Last Update:  $Author: meichel $
dnl Revision:     $Revision: 1.3 $
dnl Status:       $State: Exp $
dnl
dnl $Id: aclocal.m4,v 1.3 1996-03-28 11:05:22 meichel Exp $
dnl
dnl $Log: aclocal.m4,v $
dnl Revision 1.3  1996-03-28 11:05:22  meichel
dnl Added macro AC_CHECK_DECLARATION which checks if a type declaration exists
dnl in certain header files (e.g. for struct sembuf)
dnl
dnl Revision 1.2  1995/11/28 15:59:46  meichel
dnl Added macro AC_CHECK_PROTOTYPE which allows to check for
dnl the existance of prototype declarations. Added CVS log.
dnl
dnl

dnl AC_CHECK_GXXLIB works like AC_CHECK_LIB, but for libg++.
dnl   This additional macro is necessary because the /bin/sh will
dnl   not allow you to define a variable "ac_cv_lib_g++" and
dnl   thus AC_CHECK_LIB will fail for libg++.
dnl   All symbols #defined by this macro replace G++ by GXX.

dnl AC_CHECK_GXXLIB
AC_DEFUN(AC_CHECK_GXXLIB,
[AC_MSG_CHECKING([for -lg++])
AC_CACHE_VAL(ac_cv_lib_gxx,
[ac_save_LIBS="$LIBS"
LIBS="-lg++ $LIBS"
AC_TRY_LINK(, [main()], eval "ac_cv_lib_gxx=yes", eval "ac_cv_lib_gxx=no")dnl
LIBS="$ac_save_LIBS"
])dnl
if eval "test \"`echo '$ac_cv_lib_gxx'`\" = yes"; then
  AC_MSG_RESULT(yes)
  ifelse([$3], , 
[changequote(, )dnl
  ac_tr_lib=HAVE_LIBGXX
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_lib)
  LIBS="-lg++ $LIBS"
], [$3])
else
  AC_MSG_RESULT(no)
ifelse([$4], , , [$4
])dnl
fi
])

dnl AC_CHECK_PROTOTYPE checks if there is a prototype declaration
dnl   for the given function. If header file(s) are given as argument 2, they
dnl   are #included in the search. Otherwise only predefined functions will
dnl   be found.
dnl Note:
dnl   Since GNU autoheader does not support this macro, you must create entries
dnl   in your acconfig.h for each function which is tested.
dnl Examples:
dnl   in configure.in: 
dnl     AC_CHECK_PROTOTYPE(setsockopt, sys/types.h sys/socket.h)
dnl     AC_CHECK_PROTOTYPE(gethostid)
dnl   in acconfig.h:
dnl     #undef HAVE_PROTOTYPE_SETSOCKOPT
dnl     #undef HAVE_PROTOTYPE_GETHOSTID

dnl AC_CHECK_PROTOTYPE(FUNCTION, HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_PROTOTYPE,
[AC_MSG_CHECKING([ifelse([$2], , [predefined prototype for $1], [prototype for $1 (in $2)])])
ifelse([$2], , [ac_includes=""
],
[ac_includes=""
for ac_header in $2
do
  ac_includes="$ac_includes
#include<$ac_header>"
done])
AC_CACHE_VAL(ac_cv_prototype_$1,
[AC_TRY_COMPILE(
ifelse(AC_LANG, CPLUSPLUS, [#ifdef __cplusplus
extern "C" {
#endif
])
[$ac_includes
]
ifelse(AC_LANG, CPLUSPLUS, [#ifdef __cplusplus
}
#endif
])
[typedef union { int member; } dummyStruct;
]ifelse(AC_LANG, CPLUSPLUS, [#ifdef __cplusplus
extern "C"
#endif
])
[dummyStruct $1(dummyStruct);
]
, ,eval "ac_cv_prototype_$1=no", eval "ac_cv_prototype_$1=yes")])dnl
if eval "test \"`echo '$ac_cv_prototype_'$1`\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_prototype=HAVE_PROTOTYPE_`echo $1 | tr '[a-z]' '[A-Z]'`
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_prototype)
  ifelse([$3], , :, [$3])
else
  AC_MSG_RESULT(no)
ifelse([$4], , , [$4
])dnl
fi
])

dnl AC_CHECK_DECLARATION checks if a certain type is declared in the
dnl   include files given as argument 2.
dnl Note:
dnl   Since GNU autoheader does not support this macro, you must create entries
dnl   in your acconfig.h for each function which is tested.
dnl Examples:
dnl   in configure.in: 
dnl     AC_CHECK_DECLARATION(struct sembuf, sys/types.h sys/ipc.h sys/sem.h)
dnl   in acconfig.h:
dnl     #undef HAVE_DECLARATION_STRUCT_SEMBUF

dnl AC_CHECK_DECLARATION(FUNCTION, HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_DECLARATION,
[AC_MSG_CHECKING([ifelse([$2], , [predefined type $1], [declaration for $1 (in $2)])])
ifelse([$2], , [ac_includes=""
],
[ac_includes=""
for ac_header in $2
do
  ac_includes="$ac_includes
#include<$ac_header>"
done
ac_cv_declaration=`echo 'ac_cv_declaration_$1' | tr ' ' '_'`])
AC_CACHE_VAL($ac_cv_declaration,
[AC_TRY_COMPILE(
ifelse(AC_LANG, CPLUSPLUS, [#ifdef __cplusplus
extern "C" {
#endif
])
[$ac_includes
]
ifelse(AC_LANG, CPLUSPLUS, [#ifdef __cplusplus
}
#endif
])
[$1 dummy;
]
, ,eval "$ac_cv_declaration=yes", eval "$ac_cv_declaration=no")])dnl
if eval "test \"\$$ac_cv_declaration\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_declaration=HAVE_DECLARATION_`echo $1 | tr ' [a-z]' '_[A-Z]'`
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_declaration)
  ifelse([$3], , :, [$3])
else
  AC_MSG_RESULT(no)
ifelse([$4], , , [$4
])dnl
fi
unset ac_cv_declaration
])

