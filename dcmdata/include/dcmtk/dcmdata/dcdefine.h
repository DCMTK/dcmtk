/*
 *
 *  Copyright (C) 2011, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Contains preprocessor definitions
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2011-12-14 09:04:11 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCDEFINE_H
#define DCDEFINE_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofdefine.h"


#ifdef dcmdata_EXPORTS
#define DCMTK_DCMDATA_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMDATA_EXPORT DCMTK_DECL_IMPORT
#endif


#endif // DCDEFINE_H


/*
 * CVS/RCS Log:
 * $Log: dcdefine.h,v $
 * Revision 1.10  2011-12-14 09:04:11  uli
 * Make it possible to accurately build dcmdata and libi2d as DLLs.
 *
 */
