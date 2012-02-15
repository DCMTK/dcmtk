/*
 *
 *  Copyright (C) 2001-2012, OFFIS e.V.
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
 *  Purpose: class OFCondition and helper classes
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2012-02-15 14:50:46 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcond.h"


/* ---------- global condition constants ---------- */

makeOFConditionConst(EC_Normal,                      0, 0, OF_ok,      "Normal");
makeOFConditionConst(EC_IllegalParameter,            0, 1, OF_error,   "Illegal parameter");
makeOFConditionConst(EC_MemoryExhausted,             0, 2, OF_failure, "Virtual Memory exhausted");
makeOFConditionConst(EC_NoEncodingLibrary,           0, 3, OF_error,   "No character encoding library available");
makeOFConditionConst(EC_NoEncodingSelected,          0, 4, OF_error,   "No character encoding selected");
// codes 5..8 are used for dynamically created error messages (see below constants)
makeOFConditionConst(EC_CouldNotCreateTemporaryFile, 0, 9, OF_error,   "Could not create temporary file");
makeOFConditionConst(EC_InvalidFilename,             0, 10, OF_error,  "Invalid filename");


// NOTE:
// error codes 1024..2047 are reserved for inter-process communication
// errors defined elsewhere in module ofstd

const unsigned short EC_CODE_CannotOpenEncoding     = 5;
const unsigned short EC_CODE_CannotCloseEncoding    = 6;
const unsigned short EC_CODE_CannotConvertEncoding  = 7;
const unsigned short EC_CODE_CannotControlConverter = 8;


/*
 *
 * CVS/RCS Log:
 * $Log: ofcond.cc,v $
 * Revision 1.11  2012-02-15 14:50:46  uli
 * Removed dependency on static initialization order from OFCondition.
 * All static condition objects are now created via makeOFConditionConst()
 * in a way that doesn't need a constructor to run. This should only break
 * code which defines its own condition objects, all other changes are
 * backwards compatible.
 *
 * Revision 1.10  2011-11-28 11:15:46  joergr
 * Added general condition constant for invalid filenames.
 *
 * Revision 1.9  2011-11-16 13:50:37  uli
 * Added a new class for managing temporary files.
 *
 * Revision 1.8  2011-10-28 09:32:48  joergr
 * Restructured code of OFCharacterEncoding in order to allow particular classes
 * to access more low-level functions, e.g. for opening multiple conversion
 * descriptors at the same time. This will be needed for ISO 2022 support.
 *
 * Revision 1.7  2011-10-24 13:03:10  joergr
 * Changed name of status code constant for "illegal character encoding".
 *
 * Revision 1.6  2011-10-21 09:14:59  joergr
 * Added class for managing and converting between different character encodings
 * based on the libiconv toolkit.
 *
 * Revision 1.5  2010-10-14 13:14:53  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.4  2005/12/08 15:48:51  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2002/07/10 11:52:54  meichel
 * Added comment about reserved range of error codes
 *
 * Revision 1.2  2001/09/25 17:07:27  meichel
 * Disabled implicit conversion to bool, added default constructor
 *   to class OFCondition.
 *
 * Revision 1.1  2001/08/23 16:08:41  meichel
 * Initial release of class OFCondition, a generic approach for condition codes
 *
 *
 */
