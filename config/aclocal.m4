
dnl 
dnl Filename: aclocal.m4
dnl
dnl Purpose: additional M4 macros for GNU autoconf
dnl
dnl Authors: Andreas Barth, Marco Eichelberg
dnl
dnl Last Update:  $Author: meichel $
dnl Revision:     $Revision: 1.19 $
dnl Status:       $State: Exp $
dnl
dnl $Id: aclocal.m4,v 1.19 2002-05-15 14:13:11 meichel Exp $
dnl
dnl $Log: aclocal.m4,v $
dnl Revision 1.19  2002-05-15 14:13:11  meichel
dnl Added configure test for readdir_r conforming to Posix 1.c draft 6
dnl
dnl Revision 1.18  2002/04/16 14:06:18  joergr
dnl Added configurable support for C++ ANSI standard includes (e.g. streams).
dnl Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
dnl contribution.
dnl
dnl Revision 1.17  2001/12/18 09:51:57  meichel
dnl Modified configure test for "const" support of the C compiler
dnl   in order to avoid a macro recursion error on Sun CC 2.0.1
dnl
dnl Revision 1.16  2001/11/02 12:03:42  meichel
dnl Added new configure tests for std::_Ios_Openmode and ios::nocreate,
dnl   required for gcc 3.0.x.
dnl
dnl Revision 1.15  2001/08/23 16:29:11  meichel
dnl Added configure tests required by dcmjpeg module
dnl
dnl Revision 1.14  2000/12/20 09:54:29  meichel
dnl Fixed remaining problems with configure on FreeBSD.
dnl
dnl Revision 1.13  2000/12/19 12:15:45  meichel
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

dnl AC_CHECK_DECLARATION checks if a certain type is declared in the include files given as argument 2 or 3.
dnl   Files given as argument 2 are included extern "C" in C++ mode,
dnl   files given as argument 3 are included "as is".
dnl Note:
dnl   Since GNU autoheader does not support this macro, you must create entries
dnl   in your acconfig.h for each function which is tested.
dnl Examples:
dnl   in configure.in: 
dnl     AC_CHECK_DECLARATION(struct sembuf, sys/types.h sys/ipc.h sys/sem.h)
dnl   in acconfig.h:
dnl     #undef HAVE_DECLARATION_STRUCT_SEMBUF
dnl
dnl AC_CHECK_DECLARATION(FUNCTION, C-HEADER-FILE..., C++-HEADER-FILE..., ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND])
AC_DEFUN(AC_CHECK_DECLARATION,
[
AC_MSG_CHECKING([ifelse([$2 $3], , [predefined type $1], [ifelse([$2], , [declaration for $1 (in $3)], [ifelse([$3], , [declaration for $1 (in $2)], [declaration for $1 (in $2 $3)])])])])
ifelse([$2], , [ac_includes=""
],
[ac_includes=""
for ac_header in $2
do
  ac_includes="$ac_includes
#include<$ac_header>"
done
])
ifelse([$3], , [ac_cpp_includes=""
],
[ac_cpp_includes=""
for ac_header in $3
do
  ac_cpp_includes="$ac_cpp_includes
#include<$ac_header>"
done
])
[ac_cv_declaration=`echo 'ac_cv_declaration_$1' | tr ' :' '__'`
]
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
[$ac_cpp_includes
]
[$1 dummy;
]
, ,eval "$ac_cv_declaration=yes", eval "$ac_cv_declaration=no")])dnl
if eval "test \"\$$ac_cv_declaration\" = yes"; then
  AC_MSG_RESULT(yes)
changequote(, )dnl
  ac_tr_declaration=HAVE_DECLARATION_`echo $1 | tr ' :[a-z]' '__[A-Z]'`
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_declaration)
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

dnl AC_CHECK_STD_NAMESPACE checks if the C++-Compiler supports the
dnl   standard name space.
dnl Note:
dnl   Since GNU autoheader does not support this macro, you must
dnl   create the entry 
dnl     #undef HAVE_STD_NAMESPACE
dnl   in your acconfig.h 
dnl Examples:
dnl   in configure.in: 
dnl     AC_CHECK_STD_NAMESPACE
dnl   in acconfig.h:
dnl     #undef HAVE_STD_NAMESPACE

dnl AC_CHECK_STD_NAMESPACE
AC_DEFUN(AC_CHECK_STD_NAMESPACE,
[AC_MSG_CHECKING([for C++ standard namespace])
AC_CACHE_VAL(ac_cv_check_std_namespace,
[AC_TRY_COMPILE_AND_LINK([
#include "iostream"
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
], , 
SAVE_CXXFLAGS="$CXXFLAGS"
SAVE_CFLAGS="$CFLAGS"
CXXFLAGS="-pthread $CXXFLAGS"
CFLAGS="-pthread $CFLAGS"
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
[AC_CACHE_CHECK([for inline], ac_cv_my_c_inline,
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
dnl but defines C_CONST instead of redefining "inline" directly.

AC_DEFUN(AC_MY_C_CONST,
[dnl This message is consistent in form with the other checking messages,
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
[AC_CACHE_CHECK(whether char is unsigned, ac_cv_my_c_char_unsigned,
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
[AC_CACHE_CHECK(whether right shift is unsigned, ac_cv_my_c_rightshift_unsigned,
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
dnl Note:
dnl   Since GNU autoheader does not support this macro, you must create entries
dnl   in your acconfig.h manually
dnl Examples:
dnl   in configure.in: 
dnl     AC_CHECK_IOS_NOCREATE
dnl   in acconfig.h:
dnl     #undef HAVE_IOS_NOCREATE
dnl
dnl AC_CHECK_IOS_NOCREATE(IOS-Name, header [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
AC_DEFUN(AC_CHECK_IOS_NOCREATE,
[
AC_MSG_CHECKING([declaration of ios::nocreate (in $2)])
ac_cv_declaration=ac_cv_declaration_ios_nocreate
AC_CACHE_VAL($ac_cv_declaration,
[AC_TRY_COMPILE([
#include <$2>
], [ifstream file("name", $1::nocreate)] ,eval "$ac_cv_declaration=yes", eval "$ac_cv_declaration=no")])dnl
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
dnl Note:
dnl   Since GNU autoheader does not support this macro, you must create entries
dnl   in your acconfig.h manually
dnl Examples:
dnl   in configure.in: 
dnl     AC_CHECK_OLD_READDIR_R
dnl   in acconfig.h:
dnl     #undef HAVE_OLD_READDIR_R
dnl
dnl  The test macro AC_HEADER_DIRENT must be run before this test!
dnl
dnl AC_CHECK_OLD_READDIR_R([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
AC_DEFUN(AC_CHECK_OLD_READDIR_R,
[
AC_MSG_CHECKING([if declaration of readdir_r conforms to Posix 1.c draft 6])
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


