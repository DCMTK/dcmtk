/*
** Author: Andreas Barth	Created: 19.06.97
**				
** Kuratorium OFFIS e.V.
**
** Module: oftypes.h
**
** Purpose:
**	Defines some C++ standard types that are not consistently 
**      supported by all C++ Compilers
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-02 11:51:16 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/oftypes.h,v $
** CVS/RCS Revision:	$Revision: 1.1 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef OFTYPES_H
#define OFTYPES_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

// Definition of type OFBool

#ifdef HAVE_CXX_BOOL
#define OFBool bool
#define OFTrue true
#define OFFalse false
#else
typedef int OFBool;
#ifndef OFTrue 
#define OFTrue (1)
#endif

#ifndef OFFalse 
#define OFFalse (0)
#endif
#endif
#endif

/*
** CVS/RCS Log:
** $Log: oftypes.h,v $
** Revision 1.1  1997-07-02 11:51:16  andreas
** - Preliminary release of the OFFIS Standard Library.
**   In the future this library shall contain a subset of the
**   ANSI C++ Library (Version 3) that works on a lot of different
**   compilers. Additionally this library shall include classes and
**   functions that are often used. All classes and functions begin
**   with OF... This library is independent of the DICOM development and
**   shall contain no DICOM specific stuff.
**
**
*/

