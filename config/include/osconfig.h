#ifndef OSCONFIG_H
#define OSCONFIG_H

/*
** This head includes an OS/Compiler specific configuration header.
** Add entries for specific non-unix OS/Compiler environments.
** Under unix the default <config.h> should be used.
**
*/

#ifdef __MWERKS__
/*
** Metrowerks C++ compiler for 68k and PowerPC Apple Macintosh
*/
#include "cfmwerks.h"

#elif _MSC_VER
/* NOTE: the symbol _MSC_VER may be incorrect.  Not tested. */
/*
** Visual C++ under Windows 95
*/
/* include config header file */

#else /* !_MSC_VER */

/*
** Some Unix platform supported by the GNU Configure utility
** Just include the automatically generated <config.h> 
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#endif /* !_MSC_VER */

#endif /* !OSCONFIG_H*/

