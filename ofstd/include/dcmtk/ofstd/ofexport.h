/*
 *
 *  Copyright (C) 2012, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  ofstd
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: common defines for import and exporting symbols
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2012-02-15 11:37:42 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef OFEXPORT_H
#define OFEXPORT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */


#ifdef DCMTK_SHARED
#ifdef _WIN32
/* Defines needed for building DLLs on windows */
#define DCMTK_DECL_EXPORT __declspec(dllexport)
#define DCMTK_DECL_IMPORT __declspec(dllimport)
#elif defined(HAVE_HIDDEN_VISIBILITY)
/* GCC hides everything when given -fvisibility=hidden. The symbols which
 * should be visible have to get a default visibility again.
 */
#define DCMTK_DECL_EXPORT __attribute__ ((visibility("default")))
#endif
#endif

#ifndef DCMTK_DECL_EXPORT
#define DCMTK_DECL_EXPORT
#endif

#ifndef DCMTK_DECL_IMPORT
#define DCMTK_DECL_IMPORT
#endif


#endif

/*
 * CVS/RCS Log:
 * $Log: ofexport.h,v $
 * Revision 1.2  2012-02-15 11:37:42  uli
 * Don't use C++ comments in file included from C.
 *
 * Revision 1.1  2012-02-01 09:49:18  uli
 * Moved some definitions to a file that can be included from C.
 *
 */
