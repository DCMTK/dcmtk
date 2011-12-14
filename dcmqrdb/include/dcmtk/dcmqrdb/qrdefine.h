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
 *  Module:  dcmqrdb
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Contains preprocessor definitions
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2011-12-14 12:58:35 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef QRDEFINE_H
#define QRDEFINE_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofdefine.h"


#ifdef dcmqrdb_EXPORTS
#define DCMTK_DCMQRDB_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMQRDB_EXPORT DCMTK_DECL_IMPORT
#endif


#endif


/*
 * CVS/RCS Log:
 * $Log: qrdefine.h,v $
 * Revision 1.1  2011-12-14 12:58:35  uli
 * Make it possible to build dcmqrdb as a DLL.
 *
 */
