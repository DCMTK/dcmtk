
dnl 
dnl Filename: aclocal.m4
dnl
dnl Purpose: additional M4 macros for GNU autoconf
dnl
dnl Authors: Andreas Barth, Marco Eichelberg
dnl
dnl Last Update:  $Author: meichel $
dnl Revision:     $Revision: 1.13 $
dnl Status:       $State: Exp $
dnl
dnl $Id: aclocal.m4,v 1.13 2000-12-19 12:15:45 meichel Exp $
dnl
dnl $Log: aclocal.m4,v $
dnl Revision 1.13  2000-12-19 12:15:45  meichel
dnl Updated configure for the FreeBSD Posix implementation which requires
dnl   a special gcc option -pthread to cause linking with libc_r instead of libc.
dnl
dnl Revision 1.12  2000/09/08 14:20:11  meichel
dnl Added new options to configure
dnl
dnl Revision 1.11  2000/09/05 12:19:32  joergr
dnl Added new test checking for the presence of type ssize_t.
dnl
dnl Revision 1.10  2000/03/10 11:55:43  meichel
dnl Added special configure test for <netinet/in.h> and <netinet/tcp.h>,
dnl   needed for IRIX 6.
dnl
dnl Revision 1.9  2000/02/24 13:51:52  meichel
dnl Added new check that distinguishes NeXT's libtool from GNU libtool (which
dnl   has a totally different purpose). Required because the old configure scheme
dnl   failed if GNU libtool was found in the search path.
dnl
dnl Revision 1.8  1999/04/28 16:49:47  meichel
dnl Added test whether the compiler supports the new explicit template
dnl   specialization syntax, e.g. template<> int a_class<int>::a_method()
dnl
dnl Revision 1.7  1997/09/11 15:53:17  hewett
dnl Enhanced the configure macro AC_CHECK_PROTOTYPE to check the
dnl include files passed as aruments before searching for a
dnl prototype.  This makes the configure.in file considerably
dnl simpler.  The include files passed as aruments to the
dnl AC_CHECL_PROTOTYPE macro must have already been tested for
dnl using the AC_CHECK_HEADERS macro.  If not, the include files
dnl are assumed not to exist.
dnl
dnl Revision 1.6  1997/07/03 09:38:17  meichel
dnl Corrected bug in configure module - all tests trying to link or run
dnl   a test program could fail (and, therefore, report an incorrect result)
dnl   if libg++.a was not found in the default link path.
dnl
dnl Revision 1.5  1997/07/02 11:53:02  andreas
dnl - Preliminary release of the OFFIS Standard Library.
dnl   In the future this library shall contain a subset of the
dnl   ANSI C++ Library (Version 3) that works on a lot of different
dnl   compilers. Additionally this library shall include classes and
dnl   functions that are often used. All classes and functions begin
dnl   with OF... This library is independent of the DICOM development and
dnl   shall contain no DICOM specific stuff.
dnl
dnl Revision 1.4  1996/12/03 15:28:19  meichel
dnl Added support for HP-UX 9.05 systems using GCC 2.7.2.1
dnl
dnl Revision 1.3  1996/03/28 11:05:22  meichel
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


dnl AC_CHECK_INTP_SELECT checks if the prototype for select()
dnl   specifies arguments 2-4 to be int* instead of struct fd_set *.
dnl   This is true for HP UX 9.x and causes C++ code to break.

dnl Note:
dnl   Since GNU autoheader does not support this macro, you must create
dnl   an entry in your acconfig.h.
dnl Examples:
dnl   in configure.in: 
dnl     AC_CHECK_INTP_SELECT(sys/time.h sys/types.h)
dnl   in acconfig.h:
dnl     #undef HAVE_INTP_SELECT

dnl AC_CHECK_INTP_SELECT(HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_INTP_SELECT,
[AC_MSG_CHECKING([ifelse([$1], , [if select() needs int* parameters], 
[if select() needs int* parameters (in $1)])])
ifelse([$1], , [ac_includes=""
],
[ac_includes=""
for ac_header in $1
do
  ac_includes="$ac_includes
#include<$ac_header>"
done])
AC_CACHE_VAL(ac_cv_prototype_intp_select,
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
]),
[
  int i;
  struct fd_set fds;
  i = select(1, &fds, &fds, &fds, NULL);
],
eval "ac_cv_prototype_intp_select=no", 
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
]),
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

dnl AC_CHECK_CLASS_TEMPLATE checks if the C++-Compiler is capable of
dnl   using class templates in the easiest form i. e. all methods are
dnl   inline, no template methods and no typedefs in the class
dnl Note:
dnl   Since GNU autoheader does not support this macro, you must
dnl   create the entry 
dnl     #undef HAVE_CLASS_TEMPLATE
dnl   in your acconfig.h 
dnl Examples:
dnl   in configure.in: 
dnl     AC_CHECK_CLASS_TEMPLATE
dnl   in acconfig.h:
dnl     #undef HAVE_CLASS_TEMPLATE

dnl AC_CHECK_CLASS_TEMPLATE
AC_DEFUN(AC_CHECK_CLASS_TEMPLATE,
[AC_MSG_CHECKING([for C++ class template])
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
dnl Note:
dnl   Since GNU autoheader does not support this macro, you must
dnl   create the entry 
dnl     #undef HAVE_FUNCTION_TEMPLATE
dnl   in your acconfig.h 
dnl Examples:
dnl   in configure.in: 
dnl     AC_CHECK_FUNCTION_TEMPLATE
dnl   in acconfig.h:
dnl     #undef HAVE_FUNCTION_TEMPLATE

dnl AC_CHECK_FUNCTION_TEMPLATE
AC_DEFUN(AC_CHECK_FUNCTION_TEMPLATE,
[AC_MSG_CHECKING([for C++ function template])
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
dnl Note:
dnl   Since GNU autoheader does not support this macro, you must
dnl   create the entry 
dnl     #undef HAVE_STATIC_TEMPLATE_METHOD
dnl   in your acconfig.h 
dnl Examples:
dnl   in configure.in: 
dnl     AC_CHECK_STATIC_TEMPLATE_METHOD
dnl   in acconfig.h:
dnl     #undef HAVE_STATIC_TEMPLATE_METHOD

dnl AC_CHECK_STATIC_TEMPLATE_METHOD
AC_DEFUN(AC_CHECK_STATIC_TEMPLATE_METHOD,
[AC_MSG_CHECKING([for C++ static methods in class templates])
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
dnl Note:
dnl   Since GNU autoheader does not support this macro, you must
dnl   create the entry 
dnl     #undef HAVE_EXPLICIT_TEMPLATE_SPECIALIZATION
dnl   in your acconfig.h 
dnl Examples:
dnl   in configure.in: 
dnl     AC_CHECK_EXPLICIT_TEMPLATE_SPECIALIZATION
dnl   in acconfig.h:
dnl     #undef HAVE_EXPLICIT_TEMPLATE_SPECIALIZATION

dnl AC_CHECK_EXPLICIT_TEMPLATE_SPECIALIZATION
AC_DEFUN(AC_CHECK_EXPLICIT_TEMPLATE_SPECIALIZATION,
[AC_MSG_CHECKING([for C++ explicit template specialization syntax])
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

dnl AC_TYPE_SSIZE_T
dnl checks for the presence of type ssize_t
AC_DEFUN(AC_TYPE_SSIZE_T,
[AC_CHECK_TYPE(ssize_t, long)])


dnl AC_CHECK_INTP_ACCEPT checks if the prototype for accept()
dnl   specifies arguments 2-4 to be int* instead of size_t *.

dnl Note:
dnl   Since GNU autoheader does not support this macro, you must create
dnl   an entry in your acconfig.h.
dnl Examples:
dnl   in configure.in: 
dnl     AC_CHECK_INTP_ACCEPT(sys/types.h sys/socket.h)
dnl   in acconfig.h:
dnl     #undef AC_CHECK_INTP_ACCEPT

dnl AC_CHECK_INTP_ACCEPT(HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_INTP_ACCEPT,
[AC_MSG_CHECKING([ifelse([$1], , [if accept() needs int* parameters], 
[if accept() needs int* parameters (in $1)])])
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
ifelse(AC_LANG, CPLUSPLUS, [#ifdef __cplusplus
extern "C" {
#endif
])
[$ac_includes
]
ifelse(AC_LANG, CPLUSPLUS, [#ifdef __cplusplus
}
#endif
]),
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
ifelse(AC_LANG, CPLUSPLUS, [#ifdef __cplusplus
extern "C" {
#endif
])
[$ac_includes
]
ifelse(AC_LANG, CPLUSPLUS, [#ifdef __cplusplus
}
#endif
]),
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

dnl AC_CHECK_PTHREAD_OPTION()
AC_DEFUN(AC_CHECK_PTHREAD_OPTION,
[AC_MSG_CHECKING(whether compiler requires -pthread option for posix threads)
REQUIRES_PTHREAD_OPTION="no"
AC_TRY_LINK(
ifelse(AC_LANG, CPLUSPLUS, [#ifdef __cplusplus
extern "C" {
#endif
])
#include <pthread.h>
ifelse(AC_LANG, CPLUSPLUS, [#ifdef __cplusplus
}
#endif
]),
[
  (void) pthread_create(NULL, NULL, NULL, NULL);
], eval "ac_requires_pthread=no", 
AC_TRY_LINK(
ifelse(AC_LANG, CPLUSPLUS, [#ifdef __cplusplus
extern "C" {
#endif
])
#include <pthread.h>
ifelse(AC_LANG, CPLUSPLUS, [#ifdef __cplusplus
}
#endif
]),
[
  (void) pthread_create(NULL, NULL, NULL, NULL);
], 
REQUIRES_PTHREAD_OPTION="yes"
, ) )

if test $REQUIRES_PTHREAD_OPTION = yes ; then
  AC_MSG_RESULT([yes])
  CXXFLAGS="-pthread $CXXFLAGS"
  CFLAGS="-pthread $CFLAGS"
else
  AC_MSG_RESULT([no])
fi
])