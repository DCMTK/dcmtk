/*
 *
 *  Copyright (C) 2003, OFFIS
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Portable macros for new-style typecasts
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-07-09 12:26:02 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofcast.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef OFCAST_H
#define OFCAST_H

#include "osconfig.h"

#ifdef HAVE_CONST_CAST
#define OFconst_cast(x,y) (const_cast< x >(y))
#else
#define OFconst_cast(x,y) ((x)(y))
#endif

#ifdef HAVE_DYNAMIC_CAST
#define OFdynamic_cast(x,y) (dynamic_cast< x >(y))
#else
#define OFdynamic_cast(x,y) ((x)(y))
#endif

#ifdef HAVE_REINTERPRET_CAST
#define OFreinterpret_cast(x,y) (reinterpret_cast< x >(y))
#else
#define OFreinterpret_cast(x,y) ((x)(y))
#endif

#ifdef HAVE_STATIC_CAST
#define OFstatic_cast(x,y) (static_cast< x >(y))
#else
#define OFstatic_cast(x,y) ((x)(y))
#endif

#endif

/*
 * CVS/RCS Log:
 * $Log: ofcast.h,v $
 * Revision 1.1  2003-07-09 12:26:02  meichel
 * Added new header file ofcast.h which defines portable macros
 *   for new-style typecast operators
 *
 *
 */
