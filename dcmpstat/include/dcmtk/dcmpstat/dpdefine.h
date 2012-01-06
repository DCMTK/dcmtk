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
 *  Module:  dcmpstat
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Contains preprocessor definitions
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2012-01-06 10:05:49 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DPDEFINE_H
#define DPDEFINE_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofdefine.h"


#ifdef dcmpstat_EXPORTS
#define DCMTK_DCMPSTAT_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMPSTAT_EXPORT DCMTK_DECL_IMPORT
#endif


#endif


/*
 * CVS/RCS Log:
 * $Log: dpdefine.h,v $
 * Revision 1.1  2012-01-06 10:05:49  uli
 * Make it possible to build dcmpstat as a DLL.
 *
 */
