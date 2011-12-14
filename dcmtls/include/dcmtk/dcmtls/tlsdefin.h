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
 *  Module:  dcmimgle
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Contains preprocessor definitions
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2011-12-14 11:45:15 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef TLSDEFIN_H
#define TLSDEFIN_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofdefine.h"


#ifdef dcmtls_EXPORTS
#define DCMTK_DCMTLS_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMTLS_EXPORT DCMTK_DECL_IMPORT
#endif


#endif


/*
 * CVS/RCS Log:
 * $Log: tlsdefin.h,v $
 * Revision 1.1  2011-12-14 11:45:15  uli
 * Make it possible to perfectly build dcmnet and dcmtls a DLLs.
 *
 * Revision 1.1  2011-12-14 09:50:21  uli
 * Make it possible to properly build dcmimgle as a DLL.
 *
 *
 */

