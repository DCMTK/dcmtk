#ifndef OSCONFIG_H
#define OSCONFIG_H

/*
** Define enclosures for include files with C linkage (mostly system headers)
*/
#ifdef __cplusplus
#define BEGIN_EXTERN_C extern "C" {
#define END_EXTERN_C }
#else
#define BEGIN_EXTERN_C 
#define END_EXTERN_C
#endif


/*
** This head includes an OS/Compiler specific configuration header.
** Add entries for specific non-unix OS/Compiler environments.
** Under unix the default <config.h> should be used.
**
*/

#ifdef HAVE_CONFIG_H
/*
** Some sort of Unix platform supported by the GNU Configure utility.
** Just include the automatically generated <config.h> 
*/
#include <config.h>

#elif __MWERKS__
/*
** Metrowerks C++ compiler for 68k and PowerPC Apple Macintosh
*/
#include "cfmwerks.h"

#elif defined(_WIN32)
/*
** Visual C++ in a Windows 32 bit environment (WindowsNT/95)
*/
#include "cfwin32.h"

#else
/*
** Don't know what sort of machine this is 
*/
#endif

#endif /* !OSCONFIG_H*/

