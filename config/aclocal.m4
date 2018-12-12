dnl
dnl Filename: aclocal.m4
dnl
dnl Purpose: additional M4 macros for GNU autoconf
dnl
dnl Authors: Andreas Barth, Marco Eichelberg
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
dnl   be found.  The header files are only included in the search if they
dnl   have already been found using the AC_CHECK_HEADERS(header) macro.
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
AH_TEMPLATE(AS_TR_CPP(HAVE_PROTOTYPE_$1), [Define if your system has a prototype for $1 in $2])
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
changequote(, )dnl
  ac_tr_prototype=HAVE_PROTOTYPE_`echo $tmp_save_1 | tr '[a-z]' '[A-Z]'`
changequote([, ])dnl
  AC_DEFINE_UNQUOTED([$ac_tr_prototype])
  ifelse([$3], , :, [$3])
else
  AC_MSG_RESULT(no)
ifelse([$4], , , [$4
])dnl
fi
])

dnl AC_CHECK_COMPILES verifies that the given code fragment can be compiled.
dnl   It is assumed that successfully compiling means that the specified
dnl   function is available. If header file(s) are given as argument 2, they
dnl   #included in the search. Otherwise only predefined functions will be
dnl   available. The header files are only included in the search if they have
dnl   already been found using the AC_CHECK_HEADERS(header) macro.

dnl AC_CHECK_COMPILES(FUNCTION, HEADER-FILE..., CODE, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_COMPILES,
[AC_MSG_CHECKING([ifelse([$2], , [for $1], [for $1 (in $2)])])
AH_TEMPLATE(AS_TR_CPP(HAVE_PROTOTYPE_$1), [Define if your system has a prototype for $1 in $2])
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
changequote(, )dnl
  ac_tr_prototype=HAVE_PROTOTYPE_`echo $tmp_save_1 | tr '[a-z]' '[A-Z]'`
changequote([, ])dnl
  AC_DEFINE_UNQUOTED([$ac_tr_prototype])
  ifelse([$4], , :, [$4])
else
  AC_MSG_RESULT(no)
ifelse([$5], , , [$5
])dnl
fi
])

dnl AC_CHECK_DECLARATION checks if a certain type is declared in the include files given as argument 2 or 3.
dnl   Files given as argument 2 are included extern "C" in C++ mode,
dnl   files given as argument 3 are included "as is".
dnl   Header files are only included in the search if they
dnl   have already been found using the AC_CHECK_HEADERS(header) macro.

dnl AC_CHECK_DECLARATION(FUNCTION, C-HEADER-FILE..., C++-HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_DECLARATION,
[
AC_MSG_CHECKING([ifelse([$2 $3], , [predefined type $1], [ifelse([$2], , [declaration for $1 (in $3)], [ifelse([$3], , [declaration for $1 (in $2)], [declaration for $1 (in $2 $3)])])])])
AH_TEMPLATE(AS_TR_CPP(HAVE_DECLARATION_$1), [Define if your system has a declaration for $1 in $2 $3])
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
changequote(, )dnl
  ac_tr_declaration=HAVE_DECLARATION_`echo $1 | tr ' :[a-z]' '__[A-Z]'`
changequote([, ])dnl
  AC_DEFINE_UNQUOTED([$ac_tr_declaration])
  ifelse([$4], , :, [$4])
else
  AC_MSG_RESULT(no)
ifelse([$5], , , [$5
])dnl
fi
unset ac_cv_declaration
])


dnl AC_CHECK_INTP_SELECT checks if the prototype for select()
dnl   specifies arguments 2-4 to be int* instead of struct fd_set *.
dnl   This is true for HP UX 9.x and causes C++ code to break.

dnl AC_CHECK_INTP_SELECT(HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_INTP_SELECT,
[AC_MSG_CHECKING([ifelse([$1], , [if select() needs int* parameters],
[if select() needs int* parameters (in $1)])])
AH_TEMPLATE([HAVE_INTP_SELECT], [Define if your system declares argument 2-4 of select()
   as int * instead of struct fd_set *])
ifelse([$1], , [ac_includes=""
],
[ac_includes=""
for ac_header in $1
do
  ac_safe=`echo "$ac_header" | sed 'y%./+-%__p_%'`
  if eval "test \"`echo '$''{'ac_cv_header_$ac_safe'}'`\" = yes"; then
    ac_includes="$ac_includes
#include<$ac_header>"
  fi
done])
AC_CACHE_VAL(ac_cv_prototype_intp_select,
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
  int i;
  fd_set fds;
  i = select(1, &fds, &fds, &fds, NULL);
],
eval "ac_cv_prototype_intp_select=no",
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
  int i;
  int fds;
  i = select(1, &fds, &fds, &fds, NULL);
],
eval "ac_cv_prototype_intp_select=yes", eval "ac_cv_prototype_intp_select=no")])])
if eval "test \"`echo $ac_cv_prototype_intp_select`\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_prototype=HAVE_INTP_SELECT
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_prototype)
  ifelse([$2], , :, [$2])
else
  AC_MSG_RESULT(no)
  ifelse([$3], , , [$3])
fi
])



dnl AC_TRY_COMPILE_AND_LINK compiles a Source file into an object file
dnl  and links the object file. This can create a different behaviour
dnl  than compiling and linking the object file directly (e.g.
dnl  Sun C++ 3.0.1 with template functions)

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

dnl AC_CHECK_STD_NAMESPACE checks if the C++-Compiler supports the
dnl   standard name space.

dnl AC_CHECK_STD_NAMESPACE
AC_DEFUN(AC_CHECK_STD_NAMESPACE,
[AC_MSG_CHECKING([for C++ standard namespace])
AH_TEMPLATE([HAVE_STD_NAMESPACE], [Define if ANSI standard C++ includes use std namespace])
AC_CACHE_VAL(ac_cv_check_std_namespace,
[AC_TRY_COMPILE_AND_LINK([
#include <iostream>
using namespace std;
],[
  cout << "Hello World" << endl;
], eval "ac_cv_check_std_namespace=yes", eval "ac_cv_check_std_namespace=no")dnl
])dnl
if eval "test \"`echo '$ac_cv_check_std_namespace'`\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_std_namespace=HAVE_STD_NAMESPACE
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_std_namespace)
else
  AC_MSG_RESULT(no)
fi
])



dnl AC_CHECK_CLASS_TEMPLATE checks if the C++-Compiler is capable of
dnl   using class templates in the easiest form i. e. all methods are
dnl   inline, no template methods and no typedefs in the class

dnl AC_CHECK_CLASS_TEMPLATE
AC_DEFUN(AC_CHECK_CLASS_TEMPLATE,
[AC_MSG_CHECKING([for C++ class template])
AH_TEMPLATE([HAVE_CLASS_TEMPLATE], [Define if your C++ compiler can work with class templates])
AC_CACHE_VAL(ac_cv_check_class_template,
[AC_TRY_COMPILE_AND_LINK([
template <class T>
class x
{
private:
	T a;
public:
	void set(T i) { a = i; }
	x(T i) { set(i); }
	T get() { return a; }
};
],[
  int i;
  x<int> a(4);
  i = a.get();
  a.set(18);
  i = a.get();
  a.set(i-1);
], eval "ac_cv_check_class_template=yes", eval "ac_cv_check_class_template=no")dnl
])dnl
if eval "test \"`echo '$ac_cv_check_class_template'`\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_class_template=HAVE_CLASS_TEMPLATE
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_class_template)
else
  AC_MSG_RESULT(no)
fi
])



dnl AC_CHECK_FUNCTION_TEMPLATE checks if the C++-Compiler is capable of
dnl   using function templates.

dnl AC_CHECK_FUNCTION_TEMPLATE
AC_DEFUN(AC_CHECK_FUNCTION_TEMPLATE,
[AC_MSG_CHECKING([for C++ function template])
AH_TEMPLATE([HAVE_FUNCTION_TEMPLATE], [Define if your C++ compiler can work with function templates])
AC_CACHE_VAL(ac_cv_check_function_template,
[AC_TRY_COMPILE_AND_LINK([
template <class T>
int f(T* a)
{
 if (a) return 1;
 return 0;
}
],[
  int i, a;
  i = f(&a);
], eval "ac_cv_check_function_template=yes", eval "ac_cv_check_function_template=no")dnl
])dnl
if eval "test \"`echo '$ac_cv_check_function_template'`\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_function_template=HAVE_FUNCTION_TEMPLATE
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_function_template)
else
  AC_MSG_RESULT(no)
fi
])


dnl AC_CHECK_STATIC_TEMPLATE_METHOD checks if the C++-Compiler is capable of
dnl   using static methods in template classes

dnl AC_CHECK_STATIC_TEMPLATE_METHOD
AC_DEFUN(AC_CHECK_STATIC_TEMPLATE_METHOD,
[AC_MSG_CHECKING([for C++ static methods in class templates])
AH_TEMPLATE([HAVE_STATIC_TEMPLATE_METHOD], [Define if your C++ compiler can work with static methods in class templates])
AC_CACHE_VAL(ac_cv_check_static_template_method,
[AC_TRY_COMPILE_AND_LINK([
void additive(int & i)
{
        i++;
}

template <class T, class Function>
class x
{
public:
	x() { }
	static void do1(Function f, int & a) { f(a); }
        static void do2(T b) { T a = b; };
};
],[
  int a = 1;
  x<int, void (*)(int &)>::do1(additive, a);
  x<int, void (*)(int &)>::do2(a);
], eval "ac_cv_check_static_template_method=yes", eval "ac_cv_check_static_template_method=no")dnl
])dnl
if eval "test \"`echo '$ac_cv_check_static_template_method'`\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_static_template_method=HAVE_STATIC_TEMPLATE_METHOD
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_static_template_method)
else
  AC_MSG_RESULT(no)
fi
])


dnl AC_CHECK_EXPLICIT_TEMPLATE_SPECIALIZATION checks if the C++-Compiler
dnl   supports the explicit template specialization syntax, i.e.
dnl     template<> int classname<int>::functionname()

dnl AC_CHECK_EXPLICIT_TEMPLATE_SPECIALIZATION
AC_DEFUN(AC_CHECK_EXPLICIT_TEMPLATE_SPECIALIZATION,
[AC_MSG_CHECKING([for C++ explicit template specialization syntax])
AH_TEMPLATE([HAVE_EXPLICIT_TEMPLATE_SPECIALIZATION], [Define if your C++ compiler supports the explicit template specialization syntax])
AC_CACHE_VAL(ac_cv_check_explicit_template_specialization,
[AC_TRY_COMPILE([
template<class T>
class X
{
  public:
    int fn();
};

template<>
int X<int>::fn()
{
    return 0;
}
],[X<int> x], eval "ac_cv_check_explicit_template_specialization=yes", eval "ac_cv_check_explicit_template_specialization=no")dnl
])dnl
if eval "test \"`echo '$ac_cv_check_explicit_template_specialization'`\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_explicit_template_specialization=HAVE_EXPLICIT_TEMPLATE_SPECIALIZATION
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_explicit_template_specialization)
else
  AC_MSG_RESULT(no)
fi
])


dnl AC_CHECK_GNU_LIBTOOL checks whether libtool is GNU libtool.
dnl   This macro requires that 'libtool' exists in the current path,
dnl   i.e. AC_CHECK_PROGS(LIBTOOL, libtool, :) should be executed and evaluated
dnl   before performing this test.
dnl   If libtool is identified as GNU libtool, the environment variable $ac_cv_check_gnu_libtool
dnl   is set to the value "yes", otherwise to "no".
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
AH_TEMPLATE(AS_TR_CPP(DCMTK_HAVE_POLL), [Define if your system has a usable <poll.h>])
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
  AC_DEFINE_UNQUOTED(DCMTK_HAVE_POLL)
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

dnl AC_CHECK_INTP_ACCEPT checks if the prototype for accept()
dnl   specifies arguments 2-4 to be int* instead of size_t *.

dnl AC_CHECK_INTP_ACCEPT(HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_INTP_ACCEPT,
[AC_MSG_CHECKING([ifelse([$1], , [if accept() needs int* parameters],
[if accept() needs int* parameters (in $1)])])
AH_TEMPLATE([HAVE_INTP_ACCEPT], [Define if your system declares argument 3 of accept()
   as int * instead of size_t * or socklen_t *])
ifelse([$1], , [ac_includes=""
],
[ac_includes=""
for ac_header in $1
do
  ac_includes="$ac_includes
#include<$ac_header>"
done])
AC_CACHE_VAL(ac_cv_prototype_intp_accept,
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
  int i;
  struct sockaddr *addr;
  size_t addrlen;

  addr = 0;
  addrlen = 0;
  i = accept(1, addr, &addrlen);
],
eval "ac_cv_prototype_intp_accept=no",
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
  int i;
  struct sockaddr *addr;
  int addrlen;

  addr = 0;
  addrlen = 0;
  i = accept(1, addr, &addrlen);
],
eval "ac_cv_prototype_intp_accept=yes", eval "ac_cv_prototype_intp_accept=no")])])
if eval "test \"`echo $ac_cv_prototype_intp_accept`\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_prototype=HAVE_INTP_ACCEPT
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_prototype)
  ifelse([$2], , :, [$2])
else
  AC_MSG_RESULT(no)
  ifelse([$3], , , [$3])
fi
])




dnl AC_CHECK_PTHREAD_OPTION checks whether the compiler requires the
dnl -pthread option to correctly link code containing posix thread calls.
dnl This is true for example on FreeBSD.
dnl This test assumes that <pthread.h> is available.

dnl If the test is positive, -pthread is added to CFLAGS and CXXFLAGS.

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



dnl AC_MY_C_INLINE works like the standard script AC_C_INLINE
dnl but defines C_INLINE instead of redefining "inline" directly.

AC_DEFUN(AC_MY_C_INLINE,
[AH_TEMPLATE([C_INLINE], [Define to the inline keyword supported by the C compiler, if any, or to the empty string])
AC_CACHE_CHECK([for inline], ac_cv_my_c_inline,
[ac_cv_my_c_inline=no
for ac_kw in inline __inline__ __inline; do
  AC_TRY_COMPILE(, [} $ac_kw foo() {], [ac_cv_my_c_inline=$ac_kw; break])
done
])
case "$ac_cv_my_c_inline" in
  inline | yes) AC_DEFINE(C_INLINE, inline) ;;
  no) AC_DEFINE(C_INLINE, ) ;;
  *)  AC_DEFINE_UNQUOTED(C_INLINE, $ac_cv_my_c_inline) ;;
esac
])


dnl AC_MY_C_CONST works like the standard script AC_C_CONST
dnl but defines HAVE_C_CONST instead of redefining "const" directly.

AC_DEFUN(AC_MY_C_CONST,
[AH_TEMPLATE([HAVE_C_CONST], [Define if "const" is supported by the C compiler])
dnl This message is consistent in form with the other checking messages,
dnl and with the result message.
AC_CACHE_CHECK([for working const], ac_cv_my_c_const,
[AC_TRY_COMPILE(,
changequote(<<, >>)dnl
<<
/* Ultrix mips cc rejects this.  */
typedef int charset[2]; const charset x;
/* SunOS 4.1.1 cc rejects this.  */
char const *const *ccp;
char **p;
/* NEC SVR4.0.2 mips cc rejects this.  */
struct point {int x, y;};
static struct point const zero = {0,0};
/* AIX XL C 1.02.0.0 rejects this.
   It does not let you subtract one const X* pointer from another in an arm
   of an if-expression whose if-part is not a constant expression */
const char *g = "string";
ccp = &g + (g ? g-g : 0);
/* HPUX 7.0 cc rejects these. */
++ccp;
p = (char**) ccp;
ccp = (char const *const *) p;
{ /* SCO 3.2v4 cc rejects this.  */
  char *t;
  char const *s = 0 ? (char *) 0 : (char const *) 0;

  *t++ = 0;
}
{ /* Someone thinks the Sun supposedly-ANSI compiler will reject this.  */
  int x[] = {25, 17};
  const int *foo = &x[0];
  ++foo;
}
{ /* Sun SC1.0 ANSI compiler rejects this -- but not the above. */
  typedef const int *iptr;
  iptr p = 0;
  ++p;
}
{ /* AIX XL C 1.02.0.0 rejects this saying
     "k.c", line 2.27: 1506-025 (S) Operand must be a modifiable lvalue. */
  struct s { int j; const int *ap[3]; };
  struct s *b; b->j = 5;
}
{ /* ULTRIX-32 V3.1 (Rev 9) vcc rejects this */
  const int foo = 10;
}
>>,
changequote([, ])dnl
ac_cv_my_c_const=yes, ac_cv_my_c_const=no)])
if test $ac_cv_my_c_const = yes; then
  AC_DEFINE_UNQUOTED(HAVE_C_CONST)
fi
])


dnl AC_MY_C_CHAR_UNSIGNED works like the standard script AC_C_CHAR_UNSIGNED
dnl but defines C_CHAR_UNSIGNED instead of __CHAR_UNSIGNED__.

AC_DEFUN(AC_MY_C_CHAR_UNSIGNED,
[AH_TEMPLATE([C_CHAR_UNSIGNED], [Define if char is unsigned on the C compiler])
AC_CACHE_CHECK(whether char is unsigned, ac_cv_my_c_char_unsigned,
[if test "$GCC" = yes; then
  # GCC predefines this symbol on systems where it applies.
AC_EGREP_CPP(yes,
[#ifdef __CHAR_UNSIGNED__
  yes
#endif
], ac_cv_my_c_char_unsigned=yes, ac_cv_my_c_char_unsigned=no)
else
AC_TRY_RUN(
[/* volatile prevents gcc2 from optimizing the test away on sparcs.  */
#if !defined(__STDC__) || __STDC__ != 1
#define volatile
#endif
main() {
  volatile char c = 255; exit(c < 0);
}], ac_cv_my_c_char_unsigned=yes, ac_cv_my_c_char_unsigned=no)
fi])
if test $ac_cv_my_c_char_unsigned = yes ; then
  AC_DEFINE(C_CHAR_UNSIGNED)
fi
])


dnl AC_MY_C_RIGHTSHIFT_UNSIGNED checks whether the right shift operation
dnl is unsigned and, if yes, defines C_RIGHTSHIFT_UNSIGNED.

AC_DEFUN(AC_MY_C_RIGHTSHIFT_UNSIGNED,
[AH_TEMPLATE([C_RIGHTSHIFT_UNSIGNED], [Define if >> is unsigned on the C compiler])
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
  AC_DEFINE(C_CHAR_UNSIGNED)
fi
])


dnl AC_CHECK_IOS_NOCREATE checks if the flag ios::nocreate is defined.

dnl AC_CHECK_IOS_NOCREATE(IOS-Name, header [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
AC_DEFUN(AC_CHECK_IOS_NOCREATE,
[
AC_MSG_CHECKING([declaration of ios::nocreate (in $2)])
AH_TEMPLATE([HAVE_IOS_NOCREATE], [Define if your system defines ios::nocreate in iostream.h])
ac_cv_declaration=ac_cv_declaration_ios_nocreate
AC_CACHE_VAL($ac_cv_declaration,
[AC_TRY_COMPILE([
#include <$2>
], [namespace std { }
using namespace std;
ifstream file("name", $1::nocreate)] ,eval "$ac_cv_declaration=yes", eval "$ac_cv_declaration=no")])dnl
if eval "test \"\$$ac_cv_declaration\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_declaration=HAVE_IOS_NOCREATE
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



dnl AC_CHECK_OLD_READDIR_R checks if there is a function readdir_r
dnl   conforming to the Posix 1.c Draft 6 interface, i.e.
dnl     struct dirent *readdir_r(DIR *dirp, struct dirent *entry);
dnl   instead of the Posix 1.c interface, i.e.
dnl     int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);
dnl
dnl  The test macro AC_HEADER_DIRENT must be run before this test!
dnl
dnl AC_CHECK_OLD_READDIR_R([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
AC_DEFUN(AC_CHECK_OLD_READDIR_R,
[
AC_MSG_CHECKING([if declaration of readdir_r conforms to Posix 1.c draft 6])
AH_TEMPLATE([HAVE_OLD_READDIR_R], [Define if your system supports readdir_r with the obsolete
   Posix 1.c draft 6 declaration (2 arguments) instead of the
   Posix 1.c declaration with 3 arguments.])
ac_cv_result=ac_cv_old_readdir_r
AC_CACHE_VAL($ac_cv_result,
[AC_TRY_COMPILE([
#ifdef __cplusplus
extern "C" {
#endif
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#else
#define dirent direct
#ifdef HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif
#ifdef HAVE_NDIR_H
#include <ndir.h>
#endif
#endif
#ifdef __cplusplus
}
#endif
], [
  DIR *dirp;
  struct dirent *dp;
  dp = readdir_r(dirp, dp);
], eval "$ac_cv_result=yes", eval "$ac_cv_result=no")])dnl
if eval "test \"\$$ac_cv_result\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_cv_result=HAVE_OLD_READDIR_R
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_cv_result)
  ifelse([$1], , , [$1
])dnl
else
  AC_MSG_RESULT(no)
  ifelse([$2], , , [$2
])dnl
fi
unset ac_cv_result
])


dnl AC_CHECK_INTP_GETSOCKOPT checks if the prototype for getsockopt()
dnl   specifies arguments 5 to be int* instead of size_t *.

dnl AC_CHECK_INTP_GETSOCKOPT(HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_INTP_GETSOCKOPT,
[AC_MSG_CHECKING([ifelse([$1], , [if getsockopt() needs int* parameters],
[if getsockopt() needs int* parameters (in $1)])])
AH_TEMPLATE([HAVE_INTP_GETSOCKOPT], [Define if your system declares argument 5 of getsockopt()
   as int * instead of size_t * or socklen_t])
ifelse([$1], , [ac_includes=""
],
[ac_includes=""
for ac_header in $1
do
  ac_includes="$ac_includes
#include<$ac_header>"
done])
AC_CACHE_VAL(ac_cv_prototype_intp_getsockopt,
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
  int i;
  size_t optlen;
  i = getsockopt(0, 0, 0, 0, &optlen);
],
eval "ac_cv_prototype_intp_getsockopt=no",
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
  int i;
  int optlen;
  i = getsockopt(0, 0, 0, 0, &optlen);
],
eval "ac_cv_prototype_intp_getsockopt=yes", eval "ac_cv_prototype_intp_getsockopt=no")])])
if eval "test \"`echo $ac_cv_prototype_intp_getsockopt`\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_prototype=HAVE_INTP_GETSOCKOPT
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_prototype)
  ifelse([$2], , :, [$2])
else
  AC_MSG_RESULT(no)
  ifelse([$3], , , [$3])
fi
])


dnl AC_CHECK_ELLIPSE_SIGNAL_HANDLER checks if the prototype for the
dnl   callback function passed to signal() needs an ellipse (...)
dnl   as parameter.  Needed for example on Irix 5.
dnl   The header files for signal() have to be specified.

dnl AC_CHECK_ELLIPSE_SIGNAL_HANDLER(HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_ELLIPSE_SIGNAL_HANDLER,
[AC_MSG_CHECKING([ifelse([$1], , [if signal() callback needs ellipse],
[if signal() callback needs ellipse (in $1)])])
AH_TEMPLATE([SIGNAL_HANDLER_WITH_ELLIPSE], [Define if signal handlers need ellipse (...) parameters])
ifelse([$1], , [ac_includes=""
],
[ac_includes=""
for ac_header in $1
do
  ac_includes="$ac_includes
#include<$ac_header>"
done])
AC_CACHE_VAL(ac_cv_signal_handler_with_ellipse,
[AC_TRY_COMPILE(
[#ifdef __cplusplus
extern "C" {
#endif
$ac_includes

  void signal_handler(int)
  {
  }
#ifdef __cplusplus
}
#endif
]
,
[
 (void) signal(0, signal_handler);
],
eval "ac_cv_signal_handler_with_ellipse=no",
[AC_TRY_COMPILE(
[#ifdef __cplusplus
extern "C" {
#endif
$ac_includes

  void signal_handler(...)
  {
  }
#ifdef __cplusplus
}
#endif
]
,
[
 (void) signal(0, signal_handler);
],
eval "ac_cv_signal_handler_with_ellipse=yes", eval "ac_cv_signal_handler_with_ellipse=no")])])
if eval "test \"`echo $ac_cv_signal_handler_with_ellipse`\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_prototype=SIGNAL_HANDLER_WITH_ELLIPSE
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_prototype)
  ifelse([$2], , :, [$2])
else
  AC_MSG_RESULT(no)
  ifelse([$3], , , [$3])
fi
])



dnl AC_INCLUDE_MATH_H_AS_CXX checks if <math.h> must be included as a C++
dnl   include file (i.e. without extern "C"). Some sytems (Win32, HP/UX 10)
dnl   use C++ language features in <math.h>.

dnl AC_INCLUDE_MATH_H_AS_CXX(HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_INCLUDE_MATH_H_AS_CXX,
[AC_MSG_CHECKING([if <math.h> fails if included extern "C"])
AH_TEMPLATE([INCLUDE_MATH_H_AS_CXX], [Define if <math.h> fails if included extern "C"])
AC_CACHE_VAL(ac_cv_include_math_h_as_cxx,
[AC_TRY_COMPILE([
extern "C"
{
#include<math.h>
}
],
[
],
eval "ac_cv_include_math_h_as_cxx=no",
[AC_TRY_COMPILE([
#include<math.h>
],
[
],
eval "ac_cv_include_math_h_as_cxx=yes", eval "ac_cv_include_math_h_as_cxx=no")])])
if eval "test \"`echo $ac_cv_include_math_h_as_cxx`\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_prototype=INCLUDE_MATH_H_AS_CXX
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_prototype)
  ifelse([$2], , :, [$2])
else
  AC_MSG_RESULT(no)
  ifelse([$3], , , [$3])
fi
])


dnl AC_CHECK_CXX_BOOL checks if bool is a built-in C++ type
dnl   (which is not the case on older compilers).

dnl AC_CHECK_CXX_BOOL(ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_CXX_BOOL,
[AC_MSG_CHECKING([if bool is built-in type])
AH_TEMPLATE([HAVE_CXX_BOOL], [Define if bool is a built-in type])
AC_CACHE_VAL(ac_cv_have_cxx_bool,
[AC_TRY_COMPILE([],[
bool b1 = true;
bool b2 = false;
],
eval "ac_cv_have_cxx_bool=yes",
eval "ac_cv_have_cxx_bool=no")])
if eval "test \"`echo $ac_cv_have_cxx_bool`\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_prototype=HAVE_CXX_BOOL
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_prototype)
  ifelse([$1], , :, [$1])
else
  AC_MSG_RESULT(no)
  ifelse([$2], , , [$2])
fi
])


dnl AC_CHECK_CXX_VOLATILE checks if volatile is a built-in C++ keyword
dnl   (which is not the case on older compilers).

dnl AC_CHECK_CXX_VOLATILE(ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_CXX_VOLATILE,
[AC_MSG_CHECKING([if volatile is known keyword])
AH_TEMPLATE([HAVE_CXX_VOLATILE], [Define if volatile is a known keyword])
AC_CACHE_VAL(ac_cv_have_cxx_volatile,
[AC_TRY_COMPILE([],[
volatile int i=0;
],
eval "ac_cv_have_cxx_volatile=yes",
eval "ac_cv_have_cxx_volatile=no")])
if eval "test \"`echo $ac_cv_have_cxx_volatile`\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_prototype=HAVE_CXX_VOLATILE
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_prototype, ,[Define if volatile is a known keyword])
  ifelse([$1], , :, [$1])
else
  AC_MSG_RESULT(no)
  ifelse([$2], , , [$2])
fi
])


dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_typename.html
dnl
AC_DEFUN([AC_CXX_TYPENAME],
[AH_TEMPLATE([HAVE_TYPENAME], [Define if typename is a known keyword])
AC_CACHE_CHECK(whether the compiler recognizes typename,
ac_cv_cxx_typename,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([template<typename T>class X {public:X(){}};],
[X<float> z; return 0;],
 ac_cv_cxx_typename=yes, ac_cv_cxx_typename=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_typename" = yes; then
  AC_DEFINE(HAVE_TYPENAME,,[define if the compiler recognizes typename])
fi
])


dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_const_cast.html
dnl
AC_DEFUN([AC_CXX_CONST_CAST],
[AH_TEMPLATE([HAVE_CONST_CAST], [Define if the compiler supports const_cast<>])
AC_CACHE_CHECK(whether the compiler supports const_cast<>,
ac_cv_cxx_const_cast,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE(,[int x = 0;const int& y = x;int& z = const_cast<int&>(y);return z;],
 ac_cv_cxx_const_cast=yes, ac_cv_cxx_const_cast=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_const_cast" = yes; then
  AC_DEFINE(HAVE_CONST_CAST,,[define if the compiler supports const_cast<>])
fi
])


dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_dynamic_cast.html
dnl
AC_DEFUN([AC_CXX_DYNAMIC_CAST],
[AH_TEMPLATE([HAVE_DYNAMIC_CAST], [Define if the compiler supports dynamic_cast<>])
AC_CACHE_CHECK(whether the compiler supports dynamic_cast<>,
ac_cv_cxx_dynamic_cast,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <typeinfo>
class Base { public : Base () {} virtual void f () = 0;};
class Derived : public Base { public : Derived () {} virtual void f () {} };],[
Derived d; Base& b=d; return dynamic_cast<Derived*>(&b) ? 0 : 1;],
 ac_cv_cxx_dynamic_cast=yes, ac_cv_cxx_dynamic_cast=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_dynamic_cast" = yes; then
  AC_DEFINE(HAVE_DYNAMIC_CAST,,[define if the compiler supports dynamic_cast<>])
fi
])


dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_reinterpret_cast.html
dnl
AC_DEFUN([AC_CXX_REINTERPRET_CAST],
[AH_TEMPLATE([HAVE_REINTERPRET_CAST], [Define if the compiler supports reinterpret_cast<>])
AC_CACHE_CHECK(whether the compiler supports reinterpret_cast<>,
ac_cv_cxx_reinterpret_cast,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <typeinfo>
class Base { public : Base () {} virtual void f () = 0;};
class Derived : public Base { public : Derived () {} virtual void f () {} };
class Unrelated { public : Unrelated () {} };
int g (Unrelated&) { return 0; }],[
Derived d;Base& b=d;Unrelated& e=reinterpret_cast<Unrelated&>(b);return g(e);],
 ac_cv_cxx_reinterpret_cast=yes, ac_cv_cxx_reinterpret_cast=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_reinterpret_cast" = yes; then
  AC_DEFINE(HAVE_REINTERPRET_CAST,,
            [define if the compiler supports reinterpret_cast<>])
fi
])


dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_static_cast.html
dnl
AC_DEFUN([AC_CXX_STATIC_CAST],
[AH_TEMPLATE([HAVE_STATIC_CAST], [Define if the compiler supports static_cast<>])
AC_CACHE_CHECK(whether the compiler supports static_cast<>,
ac_cv_cxx_static_cast,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <typeinfo>
class Base { public : Base () {} virtual void f () = 0; };
class Derived : public Base { public : Derived () {} virtual void f () {} };
int g (Derived&) { return 0; }],[
Derived d; Base& b = d; Derived& s = static_cast<Derived&> (b); return g (s);],
 ac_cv_cxx_static_cast=yes, ac_cv_cxx_static_cast=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_static_cast" = yes; then
  AC_DEFINE(HAVE_STATIC_CAST,,
            [define if the compiler supports static_cast<>])
fi
])


dnl AC_CXX_STD_NOTHROW checks if the compiler supports non-throwing new using
dnl std::nothrow.

AC_DEFUN([AC_CXX_STD_NOTHROW],
[AH_TEMPLATE([HAVE_STD__NOTHROW], [Define if the compiler supports std::nothrow])
AC_CACHE_CHECK(whether the compiler supports std::nothrow,
ac_cv_cxx_std_nothrow,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <new>],[int *i = new (std::nothrow) int],
 ac_cv_cxx_std_nothrow=yes, ac_cv_cxx_std_nothrow=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_std_nothrow" = yes; then
  AC_DEFINE(HAVE_STD__NOTHROW,, [Define if the compiler supports std::nothrow])
fi
])


dnl AC_CXX_NOTHROW_DELETE checks if the compiler supports non-throwing delete using
dnl std::nothrow.

AC_DEFUN([AC_CXX_NOTHROW_DELETE],
[AH_TEMPLATE([HAVE_NOTHROW_DELETE], [Define if the compiler supports operator delete (std::nothrow)])
AC_CACHE_CHECK(whether the compiler supports operator delete (std::nothrow),
ac_cv_cxx_nothrow_delete,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <new>],[int *i = new (std::nothrow) int; operator delete (i,std::nothrow)],
 ac_cv_cxx_nothrow_delete=yes, ac_cv_cxx_nothrow_delete=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_nothrow_delete" = yes; then
  AC_DEFINE(HAVE_NOTHROW_DELETE,, [Define if the compiler supports operator delete (std::nothrow)])
fi
])


dnl AC_CXX_NOTHROW_DELETE checks if the compiler supports non-throwing delete using
dnl std::nothrow.

AC_DEFUN([AC_CXX_STATIC_ASSERT],
[AH_TEMPLATE([HAVE_STATIC_ASSERT], [Define if the compiler supports static_assert])
AC_CACHE_CHECK(whether the compiler supports static_assert,
ac_cv_cxx_static_assert,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <cassert>],[static_assert(true, "good")],
 ac_cv_cxx_static_assert=yes, ac_cv_cxx_static_assert=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_static_assert" = yes; then
  AC_DEFINE(HAVE_STATIC_ASSERT,, [Define if the compiler supports static_assert])
fi
])


dnl AC_CXX_LIBC_H_EXTERN_C checks if <libc.h> and <math.h> cause a problem if
dnl   libc.h is included extern "C" and math.h is not. This is the case on QNX
dnl   6.2.x and 6.5.x.

AC_DEFUN([AC_CXX_LIBC_H_EXTERN_C],
[AH_TEMPLATE([INCLUDE_LIBC_H_AS_CXX], [Define if libc.h should be treated as a C++ header])
AC_CACHE_CHECK(whether libc.h should be treated as a C++ header,
ac_cv_cxx_libc_h_is_cxx,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([extern "C" {
#include <libc.h>
}
#include <math.h>],[int i = 0],
 ac_cv_cxx_libc_h_is_cxx=no, ac_cv_cxx_libc_h_is_cxx=yes)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_libc_h_is_cxx" = yes; then
  AC_DEFINE(INCLUDE_LIBC_H_AS_CXX,, [Define if libc.h should be treated as a C++ header])
fi
])


dnl AC_CHECK_POINTER_TYPE checks if a given type is a pointer type.
dnl   If header file(s) are given as argument 2, they
dnl   are #included in the search. The header files are only included in the search if they
dnl   have already been found using the AC_CHECK_HEADERS(header) macro.
dnl Examples:
dnl     AC_CHECK_POINTER_TYPE(pthread_d, pthread.h)
dnl     AC_CHECK_POINTER_TYPE(void *)

dnl AC_CHECK_POINTER_TYPE(FUNCTION, HEADER-FILE...)
AC_DEFUN(AC_CHECK_POINTER_TYPE,
[AC_MSG_CHECKING([ifelse([$2], , [if $1 is a pointer type], [if $1 is a pointer type (in $2)])])
AH_TEMPLATE(AS_TR_CPP(HAVE_POINTER_TYPE_$1), [Define if $1 is a pointer type on your system])
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
changequote(, )dnl
  ac_tr_prototype=HAVE_POINTER_TYPE_`echo $tmp_save_1 | tr '[a-z]' '[A-Z]'`
changequote([, ])dnl
  AC_DEFINE_UNQUOTED([$ac_tr_prototype])
  ifelse([$3], , :, [$3])
else
  AC_MSG_RESULT(no)
fi
])


dnl
dnl CHECK_VLA checks if the C Compiler supports variable-length arrays
dnl
AC_DEFUN(CHECK_VLA,
[AC_MSG_CHECKING([whether variable-length arrays are supported])
AH_TEMPLATE([HAVE_VLA], [Define if variable-length arrays are supported in C])
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
[ AH_TEMPLATE([DCMTK_ENABLE_LFS], [Select LFS mode (defined above) that shall be used or don't define it])
  AH_TEMPLATE([_LARGEFILE64_SOURCE], [Define to enable LFS64 (explicit large file support) if available])
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
    AC_DEFINE(DCMTK_ENABLE_LFS, DCMTK_LFS64, [Select LFS mode (defined above) that shall be used or don't define it])
    AC_DEFINE(_LARGEFILE64_SOURCE, 1, [Define to enable LFS64 (explicit large file support) if available])
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
#				CACHE-VAR,
#				DESCRIPTION,
#				PROLOGUE, [FUNCTION-BODY])
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
  AH_TEMPLATE([DCMTK_ENABLE_LFS], [Select LFS mode (defined above) that shall be used or don't define it])
  MY_AC_SYS_LARGEFILE_MACRO_VALUE(_FILE_OFFSET_BITS, 64,
    ac_cv_sys_file_offset_bits,
    [Number of bits in a file offset, on hosts where this is settable.],
    [MY_AC_SYS_LARGEFILE_TEST_INCLUDES])
  MY_AC_SYS_LARGEFILE_MACRO_VALUE(_LARGE_FILES, 1,
    ac_cv_sys_large_files,
    [Define for large files, on AIX-style hosts.],
    [MY_AC_SYS_LARGEFILE_TEST_INCLUDES])

  if test "$ac_cv_lfs_support" = yes; then
    AC_DEFINE(DCMTK_ENABLE_LFS, DCMTK_LFS, [Select LFS mode (defined above) that shall be used or don't define it])
  fi

])# MY_AC_SYS_LARGEFILE


#
# AC_STDIO_NAMESPACE
#
# This macro checks with the C++ compiler whether fopen() in <cstdio> is
# in namespace standard or in global namespace.
#
AC_DEFUN([AC_STDIO_NAMESPACE],
[ AH_TEMPLATE([STDIO_NAMESPACE], [Namespace for ANSI C functions in standard C++ headers])
  ac_cv_stdio_namespace_is_std=no
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([
#ifdef USE_STD_CXX_INCLUDES
#include <cstdio>
#else
#include <stdio.h>
#endif
],[FILE *f = ::fopen("name", "r");],
  ac_cv_stdio_namespace_is_std=no, ac_cv_stdio_namespace_is_std=yes)
  AC_LANG_RESTORE

  if test "$ac_cv_stdio_namespace_is_std" = yes; then
    AC_DEFINE(STDIO_NAMESPACE,[std::])
  else
    AC_DEFINE(STDIO_NAMESPACE,[::])
  fi
])


dnl AC_CHECK_CHARP_STRERROR_R checks if the prototype for strerror_r()
dnl specifies a return type of char * instead of int. This is the case
dnl for the GNU version, whereas the XOPEN (XSI-compliant) version returns int.

dnl AC_CHECK_CHARP_STRERROR_R(HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_CHARP_STRERROR_R,
[AC_MSG_CHECKING([ifelse([$1], , [if strerror_r() returns a char *],
[if strerror_r() returns a char * (in $1)])])
AH_TEMPLATE([HAVE_CHARP_STRERROR_R], [Define if your system declares the return type of strerror_r
   as char * instead of int])
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
changequote(, )dnl
  ac_tr_prototype=HAVE_CHARP_STRERROR_R
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_prototype)
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
        AC_DEFINE($2,[1],[Define if $1 is available])
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
        AC_DEFINE($1,[1],[Define if __alignof__ is available])
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
        AC_DEFINE($1,[1],[Define if __attribute__((aligned)) is available])
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
        AC_DEFINE($1,[1],[Define if __attribute__((aligned)) supports templates])
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
        AC_DEFINE($1,[1],[Define if the compiler supports default constructor detection via SFINAE])
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
    AH_TEMPLATE([HAVE_$1_MACRO], [Define if the compiler supports $1.])dnl
    AC_CACHE_CHECK([for $1 macro], [ac_cv_have_$1_macro], [dnl
        AC_TRY_COMPILE([], [const char * func = $1;], [ac_cv_have_$1_macro=yes], [ac_cv_have_$1_macro=no])dnl
    ])
    if test "x$ac_cv_have_$1_macro" = "xyes"; then
        AC_DEFINE([HAVE_$1_MACRO])
    fi
])

dnl
dnl This macro checks if OpenSSL provides the SSL_CTX_get0_param function
dnl
dnl AC_CHECK_SSL_CTX_GET0_PARAM
AC_DEFUN([AC_CHECK_SSL_CTX_GET0_PARAM],
[
    AH_TEMPLATE([HAVE_SSL_CTX_GET0_PARAM], [Define if OpenSSL provides the SSL_CTX_get0_param function.])dnl
    SAVELIBS=$LIBS
    HAVE_SSL_CTX_GET0_PARAM=yes
    LIBS="$LIBS $OPENSSLLIBS"
    AC_MSG_CHECKING([whether OpenSSL provides the SSL_CTX_get0_param function])
    AC_COMPILE_IFELSE(
    [
        AC_LANG_PROGRAM([[#include <openssl/ssl.h>]],[[&SSL_CTX_get0_param;]])
    ],
    [
        AC_MSG_RESULT([yes])
        AC_DEFINE([HAVE_SSL_CTX_GET0_PARAM])
    ],
    [
        AC_MSG_RESULT([no])
    ])
    LIBS=$SAVELIBS
])

dnl
dnl This macro checks if OpenSSL provides the RAND_egd function
dnl
dnl AC_CHECK_RAND_EGD
AC_DEFUN([AC_CHECK_RAND_EGD],
[
    AH_TEMPLATE([HAVE_RAND_EGD], [Define if OpenSSL provides the RAND_egd function.])dnl
    SAVELIBS=$LIBS
    HAVE_RAND_EGD=yes
    LIBS="$LIBS $OPENSSLLIBS"
    AC_MSG_CHECKING([whether OpenSSL provides the RAND_egd function])
    AC_COMPILE_IFELSE(
    [
        AC_LANG_PROGRAM([[#include <openssl/rand.h>]],[[&RAND_egd;]])
    ],
    [
        AC_MSG_RESULT([yes])
        AC_DEFINE([HAVE_RAND_EGD])
    ],
    [
        AC_MSG_RESULT([no])
    ])
    LIBS=$SAVELIBS
])
