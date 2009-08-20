
#ifndef CHARLS_CONFIG
#define CHARLS_CONFIG

#include "dcmtk/config/osconfig.h"

#ifdef _DEBUG
#include <assert.h>
#define ASSERT(t) assert(t)
#else
#  ifndef ASSERT
#    define ASSERT(t) {}
#  endif
#endif

#if defined(_WIN32)
#ifndef CHARLS_IMEXPORT
#  define CHARLS_IMEXPORT __declspec(dllexport)
#endif
// default signed int types (32 or 64 bit)


#ifndef VOID
#  ifdef  _WIN64
typedef int LONG;
#  else
typedef int LONG;
#  endif
#endif

#else /* not defined(_WIN32) */

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

// default signed int types (32 or 64 bit)
typedef intptr_t LONG;
#endif



enum constants
{
  LONG_BITCOUNT = sizeof(LONG)*8
};

typedef unsigned char BYTE;
typedef unsigned short USHORT;



#undef  NEAR

#ifndef inlinehint
#  ifdef _MSC_VER
#    ifdef _DEBUG
#      define inlinehint
#    else
#      define inlinehint __forceinline
#    endif
#  elif defined(__GNUC__) && (__GNUC__ > 3 || __GNUC__ == 3 && __GNUC_MINOR__ > 0)
#    define inlinehint inline
#  else
#    define inlinehint inline
#  endif
#endif




#if defined(i386) || defined(__i386__) || defined(_M_IX86) || defined(__amd64__) || defined(_M_X64)
#define ARCH_HAS_UNALIGNED_MEM_ACCESS /* TODO define this symbol for more architectures */
#define USE_X86_ASSEMBLY
#endif

#ifdef _MSC_VER
#include <intrin.h>

inline unsigned int byteswap(unsigned int x)
{
	return _byteswap_ulong(x);
}

inline unsigned long long byteswap(unsigned long long x)
{
	return _byteswap_uint64(x);
}

#elif defined(USE_X86_ASSEMBLY)
// We can use x86 inline assembly!
inline size_t byteswap(size_t x)
{
	asm("bswap %0" : "=r" (x) : "0" (x));
	return x;
}
#endif


#endif

