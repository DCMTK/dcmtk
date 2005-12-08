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
** Under unix the default <cfunix.h> should be used.
**
*/

#ifdef HAVE_CONFIG_H
/*
** Some sort of Unix platform supported by the GNU Configure utility.
** Just include the automatically generated <cfunix.h>
*/
#include "cfunix.h"

#elif defined(_WIN32)
/*
** Visual C++ in a Windows 32 bit environment (Windows 9x/Me/NT/2000/XP)
*/
#include "dcmtk/config/cfwin32.h"

#else
/*
** Don't know what sort of machine this is
*/
#endif

#endif /* !OSCONFIG_H*/
