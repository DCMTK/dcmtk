/*
 *
 *  Copyright (C) 1997-2000, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  ofstd
 *
 *  Author:  Andreas Barth
 *
 *  Purpose:
 *      Defines some C++ standard types that are not consistently 
 *      supported by all C++ Compilers
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-10-10 12:01:22 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/oftypes.h,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
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

/** the boolean type used throughout the DCMTK project. Mapped to the 
 *  built-in type "bool" if the current C++ compiler supports it. Mapped 
 *  to int for old-fashioned compilers which do not yet support bool.
 */
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
 * CVS/RCS Log:
 * $Log: oftypes.h,v $
 * Revision 1.4  2000-10-10 12:01:22  meichel
 * Created/updated doc++ comments
 *
 * Revision 1.3  2000/03/08 16:36:03  meichel
 * Updated copyright header.
 *
 * Revision 1.2  1998/11/27 12:42:53  joergr
 * Added copyright message to source files and changed CVS header.
 *
 * Revision 1.1  1997/07/02 11:51:16  andreas
 * - Preliminary release of the OFFIS Standard Library.
 *   In the future this library shall contain a subset of the
 *   ANSI C++ Library (Version 3) that works on a lot of different
 *   compilers. Additionally this library shall include classes and
 *   functions that are often used. All classes and functions begin
 *   with OF... This library is independent of the DICOM development and
 *   shall contain no DICOM specific stuff.
 *
 */

