/*
 *
 *  Copyright (C) 2003-2010, OFFIS e.V.
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Portable macros for new-style typecasts
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:50 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef OFCAST_H
#define OFCAST_H

#include "dcmtk/config/osconfig.h"

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
 * Revision 1.3  2010-10-14 13:15:50  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.2  2005/12/08 16:05:47  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2003/07/09 12:26:02  meichel
 * Added new header file ofcast.h which defines portable macros
 *   for new-style typecast operators
 *
 *
 */
