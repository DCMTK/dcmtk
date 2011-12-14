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
 *  Update Date:      $Date: 2011-12-14 09:04:16 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef I2DEFINE_H
#define I2DEFINE_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofdefine.h"


#ifdef i2d_EXPORTS
#define DCMTK_I2D_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_I2D_EXPORT DCMTK_DECL_IMPORT
#endif


#endif // DCDEFINE_H


/*
 * CVS/RCS Log:
 * $Log: i2define.h,v $
 * Revision 1.1  2011-12-14 09:04:16  uli
 * Make it possible to accurately build dcmdata and libi2d as DLLs.
 *
 */
