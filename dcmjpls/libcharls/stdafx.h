// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef STDAFX
#define STDAFX

#if defined(_WIN32)
#define CHARLS_IMEXPORT __declspec(dllexport)
#if defined(_MSC_VER)
#pragma warning (disable:4100)
#pragma warning (disable:4512)
#endif
#endif

#include "util.h"


#endif
