dnl
dnl Filename: aclocal.m4
dnl
dnl Purpose: additional M4 macros for GNU Autoconf
dnl
dnl Authors: Andreas Barth, Marco Eichelberg, Joerg Riesmeier, Jan Schlamelcher
dnl


dnl AC_CHECK_GXXLIB works like AC_CHECK_LIB, but for libg++.
dnl   This additional macro is necessary because the /bin/sh will
dnl   not allow you to define a variable "ac_cv_lib_g++" and
dnl   thus AC_CHECK_LIB will fail for libg++.
dnl   All symbols #defined by this macro replace G++ by GXX.
dnl
dnl AC_CHECK_GXXLIB
AC_DEFUN(AC_CHECK_GXXLIB,
[AC_MSG_CHECKING([for -lg++])
AC_CACHE_VAL(ac_cv_lib_gxx,
[ac_save_LIBS="$LIBS"
LIBS="-lg++ $LIBS"
AC_TRY_LINK(, [main()], eval "ac_cv_lib_gxx=yes", eval "ac_cv_lib_gxx=no")
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
dnl   be found. The header files are only included in the search if they have
dnl   already been found using the AC_CHECK_HEADERS(header) macro.
dnl Examples:
dnl   in configure.in:
dnl     AC_CHECK_PROTOTYPE(setsockopt, sys/types.h sys/socket.h)
dnl     AC_CHECK_PROTOTYPE(gethostid)
dnl   in osconfig.h.in:
dnl     #undef HAVE_PROTOTYPE_SETSOCKOPT
dnl     #undef HAVE_PROTOTYPE_GETHOSTID
dnl
dnl AC_CHECK_PROTOTYPE(FUNCTION, HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_PROTOTYPE,
[AC_MSG_CHECKING([ifelse([$2], , [predefined prototype for $1], [prototype for $1 (in $2)])])
AH_TEMPLATE(AS_TR_CPP(HAVE_PROTOTYPE_$1), [Define if your system has a prototype for $1 in $2.])
ifelse([$3], , :, [$3])
ifelse([$2], , [ac_includes=""
],
[ac_includes=""
for ac_header in $2
do
  ac_safe=`echo "$ac_header" | sed 'y%./+-%__p_%'`
  if eval "test \"`echo '$''{'ac_cv_header_$ac_safe'}'`\" = yes"; then
    ac_includes="$ac_includes
#include<$ac_header>"
  fi
done])
tmp_save_1=`echo $1 | tr ' :' '__'`
AC_CACHE_VAL(ac_cv_prototype_$tmp_save_1,
[AC_TRY_COMPILE(
[#ifdef __cplusplus
extern "C" {
#endif
$ac_includes
#ifdef __cplusplus
}
#endif
typedef union { int member; } dummyStruct;
#ifdef __cplusplus
extern "C"
#endif
dummyStruct $1(dummyStruct);
]
, ,eval "ac_cv_prototype_$tmp_save_1=no", eval "ac_cv_prototype_$tmp_save_1=yes")])dnl
if eval "test \"`echo '$''{'ac_cv_prototype_$tmp_save_1'}'`\" = yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(AS_TR_CPP(HAVE_PROTOTYPE_$1))
  ifelse([$3], , :, [$3])
else
  AC_MSG_RESULT(no)
ifelse([$4], , , [$4
])dnl
fi
])


dnl AC_CHECK_EXT_LIB_PROTOTYPE checks if there is a prototype declaration for
dnl   the given function in an external library. The name of the library has to
dnl   be specified as argument 2, the  header file(s) as argument 3. The header
dnl   files are only included in the search if they have already been found
dnl   using the AC_CHECK_HEADERS(header) macro.
dnl Examples:
dnl   in configure.in:
dnl     AC_CHECK_EXT_LIB_PROTOTYPE(DH_bits, OpenSSL, openssl/dh.h)
dnl     AC_CHECK_EXT_LIB_PROTOTYPE(EVP_PKEY_base_id, OpenSSL, openssl/evp.h)
dnl   in osconfig.h.in:
dnl     #undef HAVE_OPENSSL_PROTOTYPE_DH_BITS
dnl     #undef HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_BASE_ID
dnl
dnl AC_CHECK_EXT_LIB_PROTOTYPE(FUNCTION, LIBRARY, HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_EXT_LIB_PROTOTYPE,
[AC_MSG_CHECKING([whether $2 provides a prototype for $1 (in $3)])
AH_TEMPLATE(AS_TR_CPP(HAVE_$2_PROTOTYPE_$1), [Define if $2 provides a prototype for $1 in <$3>.])
ifelse([$4], , :, [$4])
ac_includes=""
for ac_header in $3
do
  ac_safe=`echo "$ac_header" | sed 'y%./+-%__p_%'`
  if eval "test \"`echo '$''{'ac_cv_header_$ac_safe'}'`\" = yes"; then
    ac_includes="$ac_includes
#include<$ac_header>"
  fi
done
tmp_save_1=`echo $1 | tr ' :' '__'`
AC_CACHE_VAL(ac_cv_prototype_$tmp_save_1,
[AC_TRY_COMPILE(
[#ifdef __cplusplus
extern "C" {
#endif
$ac_includes
#ifdef __cplusplus
}
#endif
typedef union { int member; } dummyStruct;
#ifdef __cplusplus
extern "C"
#endif
dummyStruct $1(dummyStruct);
]
, ,eval "ac_cv_prototype_$tmp_save_1=no", eval "ac_cv_prototype_$tmp_save_1=yes")])dnl
if eval "test \"`echo '$''{'ac_cv_prototype_$tmp_save_1'}'`\" = yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(AS_TR_CPP(HAVE_$2_PROTOTYPE_$1))
  ifelse([$4], , :, [$4])
else
  AC_MSG_RESULT(no)
ifelse([$5], , , [$5
])dnl
fi
])


dnl AC_CHECK_COMPILES verifies that the given code fragment can be compiled.
dnl   It is assumed that successfully compiling means that the specified
dnl   function is available. If header file(s) are given as argument 2, they
dnl   are included in the search. Otherwise only predefined functions will be
dnl   available. The header files are only included in the search if they have
dnl   already been found using the AC_CHECK_HEADERS(header) macro.
dnl
dnl AC_CHECK_COMPILES(FUNCTION, HEADER-FILE..., CODE, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_COMPILES,
[AC_MSG_CHECKING([ifelse([$2], , [for $1], [for $1 (in $2)])])
AH_TEMPLATE(AS_TR_CPP(HAVE_PROTOTYPE_$1), [Define if your system has a prototype for $1 in $2.])
ifelse([$2], , [ac_includes=""
],
[ac_includes=""
for ac_header in $2
do
  ac_safe=`echo "$ac_header" | sed 'y%./+-%__p_%'`
  if eval "test \"`echo '$''{'ac_cv_header_$ac_safe'}'`\" = yes"; then
    ac_includes="$ac_includes
#include<$ac_header>"
  fi
done])
tmp_save_1=`echo $1 | tr ' :' '__'`
AC_CACHE_VAL(ac_cv_compiles_$tmp_save_1,
[AC_TRY_COMPILE([$ac_includes], [$3],
eval "ac_cv_compiles_$tmp_save_1=yes", eval "ac_cv_compiles_$tmp_save_1=no")])dnl
if eval "test \"`echo '$''{'ac_cv_compiles_$tmp_save_1'}'`\" = yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(AS_TR_CPP(HAVE_PROTOTYPE_$1))
  ifelse([$4], , :, [$4])
else
  AC_MSG_RESULT(no)
ifelse([$5], , , [$5
])dnl
fi
])


dnl AC_CHECK_EXT_LIB_COMPILES verifies that the given code fragment can be
dnl   compiled. It is assumed that successfully compiling means that the
dnl   specified function is available in the header file(s) given as argument 3.
dnl   The name of the external library has to be specified as argument 2 but is
dnl   only used to generate the name of the define in osconfig.h.in.  The header
dnl   files are only included in the search if they have already been found
dnl   using the AC_CHECK_HEADERS(header) macro.
dnl
dnl AC_CHECK_EXT_LIB_COMPILES(FUNCTION, LIBRARY, HEADER-FILE..., CODE, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_EXT_LIB_COMPILES,
[AC_MSG_CHECKING(whether $2 provides a prototype for $1 (in $3))
AH_TEMPLATE(AS_TR_CPP(HAVE_$2_PROTOTYPE_$1), [Define if $2 provides a prototype for $1 in <$3>.])
ac_includes=""
for ac_header in $3
do
  ac_includes="$ac_includes
#include<$ac_header>"
done
tmp_save_1=`echo $1 | tr ' :' '__'`
AC_CACHE_VAL(ac_cv_compiles_$tmp_save_1,
[AC_TRY_COMPILE([$ac_includes], [$4],
eval "ac_cv_compiles_$tmp_save_1=yes", eval "ac_cv_compiles_$tmp_save_1=no")])dnl
if eval "test \"`echo '$''{'ac_cv_compiles_$tmp_save_1'}'`\" = yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(AS_TR_CPP(HAVE_$2_PROTOTYPE_$1))
  ifelse([$5], , :, [$5])
else
  AC_MSG_RESULT(no)
ifelse([$6], , , [$6
])dnl
fi
])


dnl AC_CHECK_DECLARATION checks if a certain type is declared in the include
dnl   files given as argument 2 or 3.
dnl   Files given as argument 2 are included extern "C" in C++ mode,
dnl   files given as argument 3 are included "as is".
dnl   Header files are only included in the search if they
dnl   have already been found using the AC_CHECK_HEADERS(header) macro.
dnl
dnl AC_CHECK_DECLARATION(FUNCTION, C-HEADER-FILE..., C++-HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_DECLARATION,
[
AC_MSG_CHECKING([ifelse([$2 $3], , [predefined type $1], [ifelse([$2], , [declaration for $1 (in $3)], [ifelse([$3], , [declaration for $1 (in $2)], [declaration for $1 (in $2 $3)])])])])
AH_TEMPLATE(AS_TR_CPP(HAVE_DECLARATION_$1), [Define if your system has a declaration for $1 in $2 $3.])
ifelse([$2], , [ac_includes=""
],
[ac_includes=""
for ac_header in $2
do
  ac_safe=`echo "$ac_header" | sed 'y%./+-%__p_%'`
  if eval "test \"`echo '$''{'ac_cv_header_$ac_safe'}'`\" = yes"; then
    ac_includes="$ac_includes
#include<$ac_header>"
  fi
done
])
ifelse([$3], , [ac_cpp_includes=""
],
[ac_cpp_includes=""
for ac_header in $3
do
  ac_safe=`echo "$ac_header" | sed 'y%./+-%__p_%'`
  if eval "test \"`echo '$''{'ac_cv_header_$ac_safe'}'`\" = yes"; then
    ac_cpp_includes="$ac_cpp_includes
#include<$ac_header>"
  fi
done
])
[ac_cv_declaration=`echo 'ac_cv_declaration_$1' | tr ' :' '__'`
]
AC_CACHE_VAL($ac_cv_declaration,
[AC_TRY_COMPILE(
[#ifdef __cplusplus
extern "C" {
#endif
$ac_includes
#ifdef __cplusplus
}
#endif
$ac_cpp_includes
$1 dummy;
]
, ,eval "$ac_cv_declaration=yes", eval "$ac_cv_declaration=no")])dnl
if eval "test \"\$$ac_cv_declaration\" = yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(AS_TR_CPP(HAVE_DECLARATION_$1))
  ifelse([$4], , :, [$4])
else
  AC_MSG_RESULT(no)
ifelse([$5], , , [$5
])dnl
fi
unset ac_cv_declaration
])


dnl AC_CHECK_TRY_COMPILE Try to compile the given source and set define
dnl   depending on the result. Additional includes are used if specified.
dnl
dnl AC_CHECK_TRY_COMPILE(NAME, DEFINE, INCLUDES, SOURCE)
AC_DEFUN(AC_CHECK_TRY_COMPILE,
 [AC_MSG_CHECKING([whether the compiler supports [$1]])
 AH_TEMPLATE($2, [Define if the compiler supports $1.])
 AC_TRY_COMPILE([$3],
   [$4],
   [ AC_MSG_RESULT(yes)
     AC_DEFINE($2) ],
   [AC_MSG_RESULT(no)])
])


dnl AC_TRY_COMPILE_AND_LINK compiles a Source file into an object file
dnl  and links the object file. This can create a different behaviour
dnl  than compiling and linking the object file directly (e.g.
dnl  Sun C++ 3.0.1 with template functions)
dnl
dnl AC_TRY_COMPILE_AND_LINK(SOURCE, MAIN_BODY
dnl                         [, ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
AC_DEFUN(AC_TRY_COMPILE_AND_LINK,
[ac_link_o='${CXX-g++} -o conftest $CXXFLAGS $CPPFLAGS $LDFLAGS conftest.o $LIBS 1>&AC_FD_CC'
cat > conftest.$ac_ext <<EOF
dnl This sometimes fails to find confdefs.h, for some reason.
dnl [#]line __oline__ "[$]0"
[#]line __oline__ "configure"
#include "confdefs.h"
[$1]
int main() {
[$2]
; return 0; }
EOF
if AC_TRY_EVAL(ac_compile); then
  if AC_TRY_EVAL(ac_link_o); then
    ifelse([$3], , :, [rm -rf conftest*
    $3])
  else
    echo "configure: failed link was:" >&AC_FD_CC
    cat conftest.$ac_ext >&AC_FD_CC
    ifelse([$4], , , [  rm -rf conftest*
      $4
    ])dnl
  fi
else
  echo "configure: failed compile was:" >&AC_FD_CC
  cat conftest.$ac_ext >&AC_FD_CC
ifelse([$4], , , [  rm -rf conftest*
  $4
])dnl
fi
rm -f conftest*])


dnl AC_CHECK_GNU_LIBTOOL checks whether libtool is GNU libtool.
dnl   This macro requires that 'libtool' exists in the current path,
dnl   i.e. AC_CHECK_PROGS(LIBTOOL, libtool, :) should be executed and evaluated
dnl   before performing this test.
dnl   If libtool is identified as GNU libtool, the environment variable
dnl   $ac_cv_check_gnu_libtool is set to the value "yes", otherwise to "no".
dnl
dnl AC_CHECK_GNU_LIBTOOL
AC_DEFUN(AC_CHECK_GNU_LIBTOOL,
[AC_MSG_CHECKING([whether libtool is GNU libtool])
AC_CACHE_VAL(ac_cv_check_gnu_libtool,
[ ac_cv_check_gnu_libtool=`libtool --version 2>/dev/null | grep "GNU libtool" | sed -e "s/.*\(GNU libtool\).*/\1/" `
  if test "${ac_cv_check_gnu_libtool}set" = "GNU libtoolset" ; then
    ac_cv_check_gnu_libtool=yes
  else
    ac_cv_check_gnu_libtool=no
  fi
])
  if test $ac_cv_check_gnu_libtool = yes ; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
])


dnl AC_CHECK_HEADER_WITH_INCLUDES works like AC_CHECK_HEADER but allows to specify
dnl additional code to be put into the test program before the #include statement
dnl generated by AC_CHECK_HEADER.
dnl Typically this additional code will be additional #include statements.
dnl For example, on Irix 6, <netinet/tcp.h> cannot be included if <netinet/in.h> is not
dnl included before (leads to parse error).
dnl
dnl AC_CHECK_HEADER_WITH_INCLUDES(HEADER-FILE, [INCLUDES, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
AC_DEFUN(AC_CHECK_HEADER_WITH_INCLUDES,
[dnl Do the transliteration at runtime so arg 1 can be a shell variable.
ac_safe=`echo "$1" | sed 'y%./+-%__p_%'`
AC_MSG_CHECKING([for $1])
AC_CACHE_VAL(ac_cv_header_$ac_safe,
[AC_TRY_CPP([$2
#include <$1>], eval "ac_cv_header_$ac_safe=yes",
  eval "ac_cv_header_$ac_safe=no")])dnl
if eval "test \"`echo '$ac_cv_header_'$ac_safe`\" = yes"; then
  AC_MSG_RESULT(yes)
  ifelse([$3], , :, [$3])
else
  AC_MSG_RESULT(no)
ifelse([$4], , , [$4
])dnl
fi
])


dnl AC_CHECK_HEADERS_WITH_INCLUDES works like AC_CHECK_HEADERS but allows to specify
dnl additional code to be put into the test program before the #include statement
dnl generated by AC_CHECK_HEADERS.
dnl Typically this additional code will be additional #include statements.
dnl For example, on Irix 6, <netinet/tcp.h> cannot be included if <netinet/in.h> is not
dnl included before (leads to parse error).
dnl
dnl AC_CHECK_HEADERS_WITH_INCLUDES(HEADER-FILE..., [INCLUDES, [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
AC_DEFUN(AC_CHECK_HEADERS_WITH_INCLUDES,
[for ac_hdr in $1
do
AC_CHECK_HEADER_WITH_INCLUDES($ac_hdr, $2,
[changequote(, )dnl
  ac_tr_hdr=HAVE_`echo $ac_hdr | sed 'y%abcdefghijklmnopqrstuvwxyz./-%ABCDEFGHIJKLMNOPQRSTUVWXYZ___%'`
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_hdr) $3], $4)dnl
done
])


dnl AC_CHECK_POLL_H checks if we have a usable <poll.h>.
dnl poll on macOS is unreliable, it first did not exist, then was broken until
dnl fixed in 10.9 only to break again in 10.12.
dnl
dnl AC_CHECK_POLL_H([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
AC_DEFUN(AC_CHECK_POLL_H,
[
AC_MSG_CHECKING([for usable poll.h])
AH_TEMPLATE(DCMTK_HAVE_POLL, [Define if your system has a usable <poll.h>.])
AC_CACHE_VAL(ac_cv_header_poll_h,
[
case "${host}" in
    *-*-darwin*)
      eval "ac_cv_header_poll_h=no"
      ;;
    *)
      AC_TRY_CPP([#include <poll.h>], eval "ac_cv_header_poll_h=yes", eval "ac_cv_header_poll_h=no")dnl
      ;;
esac
])
if eval "test \"`echo '$ac_cv_header_poll_h'`\" = yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(DCMTK_HAVE_POLL)
  ifelse([$1], , :, [$1])
else
  AC_MSG_RESULT(no)
  ifelse([$2], , , [$2])
fi
])


dnl AC_CHECK_TCP_H
dnl checks for the presence of three system include files:
dnl    <netinet/in_systm.h>
dnl    <netinet/in.h>
dnl    <netinet/tcp.h>
dnl On certain systems like Irix or NeXT these are interdependent, i.e.
dnl tcp.h can only be included if in.h is included before. This script
dnl accounts for these dependencies.
dnl
dnl Note: GNU autoheader recognizes the includes tested by this macro.
dnl   No manual intervention required.
dnl
AC_DEFUN(AC_CHECK_TCP_H,
[AC_CHECK_HEADERS(netinet/in_systm.h)

if eval "test \"`echo '$ac_cv_header_netinet_in_systm_h'`\" = yes"; then
  AC_CHECK_HEADERS_WITH_INCLUDES(netinet/in.h,
#include <netinet/in_systm.h>
)
  if eval "test \"`echo '$ac_cv_header_netinet_in_h'`\" = yes"; then
    AC_CHECK_HEADERS_WITH_INCLUDES(netinet/tcp.h,
#include <netinet/in_systm.h>
#include <netinet/in.h>
)
  else
    AC_CHECK_HEADERS_WITH_INCLUDES(netinet/tcp.h,
#include <netinet/in_systm.h>
)
  fi
else
  AC_CHECK_HEADERS(netinet/in.h)
  if eval "test \"`echo '$ac_cv_header_netinet_in_h'`\" = yes"; then
    AC_CHECK_HEADERS_WITH_INCLUDES(netinet/tcp.h,
#include <netinet/in.h>
)
  else
    AC_CHECK_HEADERS(netinet/tcp.h)
  fi
fi
])


dnl AC_CHECK_PTHREAD_OPTION checks whether the compiler requires the
dnl -pthread option to correctly link code containing posix thread calls.
dnl This is true for example on FreeBSD.
dnl This test assumes that <pthread.h> is available.
dnl
dnl If the test is positive, -pthread is added to CFLAGS and CXXFLAGS.
dnl
dnl AC_CHECK_PTHREAD_OPTION
AC_DEFUN(AC_CHECK_PTHREAD_OPTION,
[AC_MSG_CHECKING(whether compiler requires -pthread option for posix threads)
REQUIRES_PTHREAD_OPTION="no"
AC_TRY_LINK(
[#ifdef __cplusplus
extern "C" {
#endif
]
#include <pthread.h>
[#ifdef __cplusplus
}
#endif
]
,
[
  (void) pthread_create(NULL, NULL, NULL, NULL);
], ,
SAVE_CXXFLAGS="$CXXFLAGS"
SAVE_CFLAGS="$CFLAGS"
CXXFLAGS="-pthread $CXXFLAGS"
CFLAGS="-pthread $CFLAGS"
AC_TRY_LINK(
[#ifdef __cplusplus
extern "C" {
#endif
]
#include <pthread.h>
[#ifdef __cplusplus
}
#endif
]
,
[
  (void) pthread_create(NULL, NULL, NULL, NULL);
],
REQUIRES_PTHREAD_OPTION="yes"
, )
CXXFLAGS="$SAVE_CXXFLAGS"
CFLAGS="$SAVE_CFLAGS"
)

if test $REQUIRES_PTHREAD_OPTION = yes ; then
  AC_MSG_RESULT([yes])
  CXXFLAGS="-pthread $CXXFLAGS"
  CFLAGS="-pthread $CFLAGS"
else
  AC_MSG_RESULT([no])
fi
])


dnl AC_MY_C_RIGHTSHIFT_UNSIGNED checks whether the right shift operation
dnl is unsigned and, if yes, defines C_RIGHTSHIFT_UNSIGNED.
dnl
AC_DEFUN(AC_MY_C_RIGHTSHIFT_UNSIGNED,
[AH_TEMPLATE(C_RIGHTSHIFT_UNSIGNED, [Define if >> is unsigned on the C compiler.])
AC_CACHE_CHECK(whether right shift is unsigned, ac_cv_my_c_rightshift_unsigned,
[
AC_TRY_RUN(
[/* See whether right-shift on a long is signed or not. */
int is_shifting_signed (long arg)
{
  long res = arg >> 4;

  if (res == -0x7F7E80CL) {	/* expected result for signed shift */
    return 1;			/* right shift is signed */
  }
  /* see if unsigned-shift hack will fix it. */
  /* we can't just test exact value since it depends on width of long... */
  res |= (~0L) << (32-4);
  if (res == -0x7F7E80CL) {	/* expected result now? */
    return 0;			/* right shift is unsigned */
  }
  /* Right shift isn't acting as I expect it to, try it with unsigned anyway */
  return 0;
}

int main()
{
  exit(is_shifting_signed(-0x7F7E80B1L));
}
], ac_cv_my_c_rightshift_unsigned=yes, ac_cv_my_c_rightshift_unsigned=no)
])
if test $ac_cv_my_c_rightshift_unsigned = yes ; then
  AC_DEFINE(C_RIGHTSHIFT_UNSIGNED)
fi
])


dnl AC_CXX_STATIC_ASSERT checks if the compiler supports static_assert.
dnl
AC_DEFUN([AC_CXX_STATIC_ASSERT],
[AH_TEMPLATE(HAVE_STATIC_ASSERT, [Define if the compiler supports static_assert.])
AC_CACHE_CHECK(whether the compiler supports static_assert,
ac_cv_cxx_static_assert,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <cassert>],[static_assert(true, "good")],
 ac_cv_cxx_static_assert=yes, ac_cv_cxx_static_assert=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_static_assert" = yes; then
  AC_DEFINE(HAVE_STATIC_ASSERT)
fi
])


dnl AC_CHECK_POINTER_TYPE checks if a given type is a pointer type.
dnl   If header file(s) are given as argument 2, they
dnl   are #included in the search. The header files are only included in the search if they
dnl   have already been found using the AC_CHECK_HEADERS(header) macro.
dnl Examples:
dnl     AC_CHECK_POINTER_TYPE(pthread_d, pthread.h)
dnl     AC_CHECK_POINTER_TYPE(void *)
dnl
dnl AC_CHECK_POINTER_TYPE(FUNCTION, HEADER-FILE...)
AC_DEFUN(AC_CHECK_POINTER_TYPE,
[AC_MSG_CHECKING([ifelse([$2], , [if $1 is a pointer type], [if $1 is a pointer type (in $2)])])
AH_TEMPLATE(AS_TR_CPP(HAVE_POINTER_TYPE_$1), [Define if $1 is a pointer type on your system.])
ifelse([$2], , [ac_includes=""
],
[ac_includes=""
cpp_code=""
if eval "test \"`echo '$ac_cv_cxx_static_cast'`\" = yes"; then
  cpp_code="unsigned long l = static_cast<unsigned long>(p)"
else
  cpp_code="unsigned long l = p"
fi
for ac_header in $2
do
  ac_safe=`echo "$ac_header" | sed 'y%./+-%__p_%'`
  if eval "test \"`echo '$''{'ac_cv_header_$ac_safe'}'`\" = yes"; then
    ac_includes="$ac_includes
#include<$ac_header>"
  fi
done])
tmp_save_1=`echo $1 | tr ' :' '__'`
AC_CACHE_VAL(ac_cv_pointer_type_$tmp_save_1,
[AC_TRY_COMPILE(
[#ifdef __cplusplus
extern "C" {
#endif
$ac_includes
#ifdef __cplusplus
}
#endif
],[#ifdef __cplusplus
$1 p; $cpp_code
#else
$1 p; unsigned long l = p
#endif
],
eval "ac_cv_pointer_type_$tmp_save_1=no", eval "ac_cv_pointer_type_$tmp_save_1=yes")])dnl
if eval "test \"`echo '$''{'ac_cv_pointer_type_$tmp_save_1'}'`\" = yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(AS_TR_CPP(HAVE_POINTER_TYPE_$1))
  ifelse([$3], , :, [$3])
else
  AC_MSG_RESULT(no)
fi
])


dnl CHECK_VLA checks if the C Compiler supports variable-length arrays
dnl
AC_DEFUN(CHECK_VLA,
[AC_MSG_CHECKING([whether variable-length arrays are supported])
AH_TEMPLATE(HAVE_VLA, [Define if variable-length arrays are supported in C.])
AC_LANG_SAVE
AC_LANG_C
AC_TRY_COMPILE([], [
 int n; int foo[n];
],
[AC_MSG_RESULT(yes); AC_DEFINE(HAVE_VLA)],
AC_MSG_RESULT(no))
AC_LANG_RESTORE
])


#    AC_TYPEDEF_HELPER(TYPE,
#                  [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#                  [INCLUDES])

m4_define([AC_TYPEDEF_HELPER],
[AS_VAR_PUSHDEF([ac_Type], [ac_cv_type_$1])dnl
AC_CACHE_CHECK([for $1], ac_Type,
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([AC_INCLUDES_DEFAULT([$4])],
[if (($1 *) 0)
  return 0;
if (sizeof ($1))
  return 0;])],
                   [AS_VAR_SET(ac_Type, yes)],
                   [AS_VAR_SET(ac_Type, no)])])
AS_IF([test AS_VAR_GET(ac_Type) = yes], [$2], [$3])[]dnl
AS_VAR_POPDEF([ac_Type])dnl
])# AC_TYPEDEF_HELPER

AC_DEFUN(AC_TYPEDEF_HELPER2,[
AH_VERBATIM([$3[_TYPEDEF]], [#ifdef ]$3[
typedef $2 $1;
#endif])])# AC_TYPEDEF_HELPER2

#
#  AC_TYPEDEF(type, default)
#
#  This macro works similar to the old (deprecated, pre-2.13) AC_CHECK_TYPE
#  macro, but instead of creating a #define for the type if absent, it
#  creates a clean typedef.
#
AC_DEFUN(AC_TYPEDEF,[
m4_define(AC_TYPEDEF_TEMP,[AS_TR_CPP(HAVE_NO_TYPEDEF_$1)])
AC_TYPEDEF_HELPER2([$1],[$2],AC_TYPEDEF_TEMP)
AC_TYPEDEF_HELPER([$1],[],[AC_DEFINE_UNQUOTED(AC_TYPEDEF_TEMP, [1], [Define `$1' to `$2' if <sys/types.h> does not define.])])
])# AC_TYPEDEF


#
# AC_LFS64
#
# This macro checks with the C compiler whether fopen64 is declared in <stdio.h>
# when _LARGEFILE64_SOURCE is defined
#
AC_DEFUN([AC_LFS64],
[ AH_TEMPLATE(DCMTK_ENABLE_LFS, [Select LFS mode (defined above) that shall be used or don't define it.])
  AH_TEMPLATE(_LARGEFILE64_SOURCE, [Define to enable LFS64 (explicit large file support) if available.])
  AC_MSG_CHECKING([for explicit large file support])
  ac_cv_lfs64_support=no
  AC_LANG_SAVE
  AC_LANG_C
  AC_TRY_LINK([#define _LARGEFILE64_SOURCE
    #include <stdio.h>],[FILE *f = fopen64("name", "r");],
  ac_cv_lfs64_support=yes, ac_cv_lfs64_support=no)
  AC_LANG_RESTORE

  if test "$ac_cv_lfs64_support" = yes; then
    AC_MSG_RESULT([yes])
    AC_DEFINE(DCMTK_ENABLE_LFS, DCMTK_LFS64)
    AC_DEFINE(_LARGEFILE64_SOURCE)
  else
    AC_MSG_RESULT([no])
  fi
])


# MY_AC_SYS_LARGEFILE_TEST_INCLUDES
# Copied from autoconf 2.60 repository of specific tests and renamed
# -------------------------------
m4_define([MY_AC_SYS_LARGEFILE_TEST_INCLUDES],
[@%:@include <sys/types.h>
 /* Check that off_t can represent 2**63 - 1 correctly.
    We can't simply define LARGE_OFF_T to be 9223372036854775807,
    since some C++ compilers masquerading as C compilers
    incorrectly reject 9223372036854775807.  */
@%:@define LARGE_OFF_T (((off_t) 1 << 62) - 1 + ((off_t) 1 << 62))
  int off_t_is_large[[(LARGE_OFF_T % 2147483629 == 721
                    && LARGE_OFF_T % 2147483647 == 1)
                    ? 1 : -1]];[]dnl
])


# MY_AC_SYS_LARGEFILE_MACRO_VALUE(C-MACRO, VALUE,
#                                 CACHE-VAR,
#                                 DESCRIPTION,
#                                 PROLOGUE, [FUNCTION-BODY])
# Copied from autoconf 2.60 repository of specific tests and renamed
# ----------------------------------------------------------
m4_define([MY_AC_SYS_LARGEFILE_MACRO_VALUE],
[AC_CACHE_CHECK([for $1 value needed for large files], [$3],
[while :; do
  $3=no
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([$5], [$6])],
    [ac_cv_lfs_support=yes; break])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([@%:@define $1 $2
$5], [$6])],
    [$3=$2; ac_cv_lfs_support=yes; break])
  break
done])
if test "$$3" != no; then
  AC_DEFINE_UNQUOTED([$1], [$$3], [$4])
fi
rm -f conftest*[]dnl
])# MY_AC_SYS_LARGEFILE_MACRO_VALUE


# MY_AC_SYS_LARGEFILE
# ----------------
# Copied from autoconf 2.60 repository of specific tests and modified
#
# By default, many hosts won't let programs access large files;
# one must use special compiler options to get large-file access to work.
# For more details about this brain damage please see:
# http://www.unix-systems.org/version2/whatsnew/lfs20mar.html
AC_DEFUN([MY_AC_SYS_LARGEFILE],
[AC_CACHE_CHECK([for special C compiler options needed for large files],
    ac_cv_sys_largefile_CC,
    [ac_cv_sys_largefile_CC=no
     if test "$GCC" != yes; then
       AC_LANG_SAVE
       AC_LANG_C
       ac_save_CC=$CC
       while :; do
        # IRIX 6.2 and later do not support large files by default,
        # so use the C compiler's -n32 option if that helps.
        AC_LANG_CONFTEST([AC_LANG_PROGRAM([MY_AC_SYS_LARGEFILE_TEST_INCLUDES])])
        AC_COMPILE_IFELSE([], [break])
        CC="$CC -n32"
        AC_COMPILE_IFELSE([], [ac_cv_sys_largefile_CC=' -n32'; break])
        break
       done
       CC=$ac_save_CC
       rm -f conftest.$ac_ext
       AC_LANG_RESTORE
    fi])
  if test "$ac_cv_sys_largefile_CC" != no; then
    CC=$CC$ac_cv_sys_largefile_CC
    CXX=$CXX$ac_cv_sys_largefile_CC
  fi

  ac_cv_lfs64_support=no
  AH_TEMPLATE(DCMTK_ENABLE_LFS, [Select LFS mode (defined above) that shall be used or don't define it.])
  MY_AC_SYS_LARGEFILE_MACRO_VALUE(_FILE_OFFSET_BITS, 64,
    ac_cv_sys_file_offset_bits,
    [Number of bits in a file offset, on hosts where this is settable.],
    [MY_AC_SYS_LARGEFILE_TEST_INCLUDES])
  MY_AC_SYS_LARGEFILE_MACRO_VALUE(_LARGE_FILES, 1,
    ac_cv_sys_large_files,
    [Define for large files, on AIX-style hosts.],
    [MY_AC_SYS_LARGEFILE_TEST_INCLUDES])

  if test "$ac_cv_lfs_support" = yes; then
    AC_DEFINE(DCMTK_ENABLE_LFS, DCMTK_LFS)
  fi

])# MY_AC_SYS_LARGEFILE


dnl AC_CHECK_CHARP_STRERROR_R checks if the prototype for strerror_r()
dnl specifies a return type of char * instead of int. This is the case
dnl for the GNU version, whereas the XOPEN (XSI-compliant) version returns int.

dnl AC_CHECK_CHARP_STRERROR_R(HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_CHARP_STRERROR_R,
[AC_MSG_CHECKING([ifelse([$1], , [if strerror_r() returns a char *],
[if strerror_r() returns a char * (in $1)])])
AH_TEMPLATE(HAVE_CHARP_STRERROR_R, [Define if your system declares the return type of strerror_r
   as char * instead of int.])
ifelse([$1], , [ac_includes=""
],
[ac_includes=""
for ac_header in $1
do
  ac_includes="$ac_includes
#include<$ac_header>"
done])
AC_CACHE_VAL(ac_cv_prototype_charp_strerror_r,
[AC_TRY_COMPILE(
[#ifdef __cplusplus
extern "C" {
#endif
$ac_includes
#ifdef __cplusplus
}
#endif
]
,
[
  char *buf = 0;
  int i = strerror_r(0, buf, 100)
],
eval "ac_cv_prototype_charp_strerror_r=no",
eval "ac_cv_prototype_charp_strerror_r=yes"
)])
if eval "test \"`echo $ac_cv_prototype_charp_strerror_r`\" = yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_CHARP_STRERROR_R)
  ifelse([$2], , :, [$2])
else
  AC_MSG_RESULT(no)
  ifelse([$3], , , [$3])
fi
])

dnl
dnl This macro adds the --disable-rpath option to configure
dnl and sets the variable $dcmtk_cv_rpath_works to "no", if needed.
dnl If that option isn't given, the availability of the rpath option is tested
AC_DEFUN([AC_MY_LIB_PATH_RPATH],
[
  AC_CACHE_CHECK([whether -Wl,-rpath is supported], [dcmtk_cv_rpath_works],
                 [AC_ARG_ENABLE([rpath],
                                [AS_HELP_STRING([--disable-rpath], [do not hardcode runtime library paths])],
                                [dcmtk_cv_rpath_works=$enableval],
                                [old_LDFLAGS="$LDFLAGS"
                                 LDFLAGS="-Wl,-rpath,$srcdir $LDFLAGS"
                                 AC_LINK_IFELSE([AC_LANG_PROGRAM([], [])],
                                                [dcmtk_cv_rpath_works=yes],
                                                [dcmtk_cv_rpath_works=no])
                                 LDFLAGS="$old_LDFLAGS"])
                 ])
])

dnl
dnl This macro adds the option --with-[OPTION_NAME]inc to configure. If this option
dnl is specified, include/ and lib/ are added to CPPFLAGS / LDFLAGS.
dnl
dnl AC_MY_LIB_PATH(OPTION_NAME, LIB_NAME)
AC_DEFUN([AC_MY_LIB_PATH],
[
  AC_REQUIRE([AC_MY_LIB_PATH_RPATH])dnl
  m4_pushdef([OPTION], [$1inc])dnl
  m4_pushdef([LONGOPTION], [--with-$1inc])dnl
  m4_pushdef([LIBNAME], [m4_default([$2], [$1])])dnl
  AC_ARG_WITH([OPTION], dnl
dnl The following line is underquoted on purpose, else the help line will be
dnl discarded because it is equal to an earlier help line.
AS_HELP_STRING([LONGOPTION=DIR], [location of LIBNAME includes and libraries]),
    [AS_CASE([$withval],
      [yes|no], [
        AC_MSG_WARN([LONGOPTION called without argument - will use default])
      ],
      [
        if test ! -d ${withval}; then
          AC_MSG_ERROR([called with LONGOPTION but LIBNAME base directory ${withval} does not exist or is not a directory.])
        fi

        CPPFLAGS="-I${withval}/include $CPPFLAGS"
        LDFLAGS="-L${withval}/lib $LDFLAGS"
        if test "x$dcmtk_cv_rpath_works" = "xyes"; then
          LDFLAGS="-Wl,-rpath,${withval}/lib $LDFLAGS"
        fi
      ])
    ])dnl
  m4_popdef([OPTION])dnl
  m4_popdef([LONGOPTION])dnl
  m4_popdef([LIBNAME])dnl
])

dnl
dnl This macro adds the option --with-openjpeginc to configure. If this option
dnl is specified, include/ and lib/ are added to CPPFLAGS / LDFLAGS.
dnl
dnl AC_MY_OPENJPEG_PATH()
AC_DEFUN([AC_MY_OPENJPEG_PATH],
[
  AC_REQUIRE([AC_MY_LIB_PATH_RPATH])dnl
  m4_pushdef([OPTION], [openjpeginc])dnl
  m4_pushdef([LONGOPTION], [--with-openjpeginc])dnl
  m4_pushdef([LIBNAME], [m4_default([OpenJPEG], [openjpeg])])dnl
  AC_ARG_WITH([OPTION], dnl
dnl The following line is underquoted on purpose, else the help line will be
dnl discarded because it is equal to an earlier help line.
AS_HELP_STRING([LONGOPTION=DIR], [location of LIBNAME includes and libraries (MUST be specified, otherwise OpenJPEG will not be found)]),
    [AS_CASE([$withval],
      [yes|no], [
        AC_MSG_WARN([LONGOPTION called without argument - will use default])
      ],
      [
        if test ! -d ${withval}; then
          AC_MSG_ERROR([called with LONGOPTION but LIBNAME base directory ${withval} does not exist or is not a directory.])
        fi

        dnl try to find openjpeg subdirectory within given include path
        OPENJPEGINCLUDEPATH=`(
          eval echo "${withval}/include/openjpeg*"
        )`

        dnl if not found, use include path following standard conventions
        if test ! -d "${OPENJPEGINCLUDEPATH}"; then
          OPENJPEGINCLUDEPATH="${withval}/include"
        fi

        CPPFLAGS="-I${OPENJPEGINCLUDEPATH} $CPPFLAGS"
        LDFLAGS="-L${withval}/lib $LDFLAGS"
        if test "x$dcmtk_cv_rpath_works" = "xyes"; then
          LDFLAGS="-Wl,-rpath,${withval}/lib $LDFLAGS"
        fi
      ])
    ])dnl
  m4_popdef([OPTION])dnl
  m4_popdef([LONGOPTION])dnl
  m4_popdef([LIBNAME])dnl
])

AC_DEFUN([AC_CHECK_SYNC_FN],
[
    AC_MSG_CHECKING([for $1])
    AC_LINK_IFELSE(
    [
        AC_LANG_SOURCE(
        [
            int main(){return $1((int *)0, 0);}
        ])
    ],
    [dcmtk_have_sync_fn=[yes]],
    [dcmtk_have_sync_fn=[no]]
    )
    if test "$dcmtk_have_sync_fn" = yes; then
        AC_MSG_RESULT([yes])
        AC_DEFINE($2,[1],[Define if $1 is available.])
    else
        AC_MSG_RESULT([no])
    fi
])

AC_DEFUN([AC_CHECK_ITERATOR_CATEGORY],
[
    AC_MSG_CHECKING([whether iterator category $1 is declared])
    AC_COMPILE_IFELSE(
    [
        AC_LANG_SOURCE(
        [
            #include <iterator>
            int main(){typedef std::$1_iterator_tag category;return 0;}
        ])
    ],
    [dcmtk_have_iter_cat=[yes]],
    [dcmtk_have_iter_cat=[no]]
    )
    if test "$dcmtk_have_iter_cat" = yes; then
        AC_MSG_RESULT([yes])
        AC_DEFINE($2,[1],[Define if the $1 iterator category is supported.])
    else
        AC_MSG_RESULT([no])
    fi
])

AC_DEFUN([AC_CHECK_ALIGNOF],
[
    AC_MSG_CHECKING([for __alignof__])
    AC_LINK_IFELSE(
    [
        AC_LANG_SOURCE(
        [
            int main(){char c[[__alignof__(int)]];return 0;}
        ])
    ],
    [dcmtk_have_alignof=[yes]],
    [dcmtk_have_alignof=[no]]
    )
    if test "$dcmtk_have_alignof" = yes; then
        AC_MSG_RESULT([yes])
        AC_DEFINE($1,[1],[Define if __alignof__ is available.])
    else
        AC_MSG_RESULT([no])
    fi
])

AC_DEFUN([AC_CHECK_ATTRIBUTE_ALIGNED],
[
    AC_MSG_CHECKING([for __attribute__((aligned))])
    AC_LINK_IFELSE(
    [
        AC_LANG_SOURCE(
        [
            int main(){__attribute__((aligned(4))) char c[[16]];return 0;}
        ])
    ],
    [dcmtk_have_attribute_aligned=[yes]],
    [dcmtk_have_attribute_aligned=[no]]
    )
    if test "$dcmtk_have_attribute_aligned" = yes; then
        AC_MSG_RESULT([yes])
        AC_DEFINE($1,[1],[Define if __attribute__((aligned)) is available.])
    else
        AC_MSG_RESULT([no])
    fi
])

AC_DEFUN([AC_CHECK_ATTRIBUTE_ALIGNED_SUPPORTS_TEMPLATES],
[
    AC_MSG_CHECKING([whether __attribute__((aligned)) supports templates])
    AC_LINK_IFELSE(
    [
        AC_LANG_SOURCE(
        [
            template<typename T>
            struct test { typedef T type __attribute__((aligned(4))); };
            int main()
            {
                test<char>::type i;
                return 0;
            }
        ])
    ],
    [dcmtk_attribute_aligned_supports_templates=[yes]],
    [dcmtk_attribute_aligned_supports_templates=[no]]
    )
    if test "$dcmtk_attribute_aligned_supports_templates" = yes; then
        AC_MSG_RESULT([yes])
        AC_DEFINE($1,[1],[Define if __attribute__((aligned)) supports templates.])
    else
        AC_MSG_RESULT([no])
    fi
])

AC_DEFUN([AC_CHECK_DEFAULT_CONSTRUCTOR_DETECTION_VIA_SFINAE],
[
    AC_MSG_CHECKING([whether the compiler supports default constructor detection via SFINAE])
    AC_LINK_IFELSE(
    [
        AC_LANG_SOURCE(
        [
            struct no_type {};
            struct yes_type {double d;};
            template<unsigned>
            struct consume{};
            template<typename X>
            static yes_type sfinae(consume<sizeof *new X>*);
            template<typename X>
            static no_type sfinae(...);
            struct test { test( int ); };
            int main()
            {
                return sizeof(sfinae<test>(0)) == sizeof(yes_type);
            }
        ])
    ],
    [dcmtk_default_constructor_detection_via_sfinae=[yes]],
    [dcmtk_default_constructor_detection_via_sfinae=[no]]
    )
    if test "$dcmtk_default_constructor_detection_via_sfinae" = yes; then
        AC_MSG_RESULT([yes])
        AC_DEFINE($1,[1],[Define if the compiler supports default constructor detection via SFINAE.])
    else
        AC_MSG_RESULT([no])
    fi
])


dnl
dnl This macro checks if a given preprocessor symbol exists and is a string
dnl
dnl AC_MY_SYMBOL_EXISTS(SYMBOL)
AC_DEFUN([AC_MY_SYMBOL_EXISTS],
[
    AH_TEMPLATE(HAVE_$1_MACRO, [Define if the compiler supports $1.])dnl
    AC_CACHE_CHECK([for $1 macro], [ac_cv_have_$1_macro], [dnl
        AC_TRY_COMPILE([], [const char * func = $1;], [ac_cv_have_$1_macro=yes], [ac_cv_have_$1_macro=no])dnl
    ])
    if test "x$ac_cv_have_$1_macro" = "xyes"; then
        AC_DEFINE(HAVE_$1_MACRO)
    fi
])
