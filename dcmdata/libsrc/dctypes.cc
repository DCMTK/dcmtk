/*
 *
 *  Copyright (C) 2002, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: global type and constant definitions
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-12-06 12:21:35 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dctypes.cc,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "dctypes.h"


/* print flags */
const size_t DCMTypes::PF_shortenLongTagValues = 1;
const size_t DCMTypes::PF_showTreeStructure    = 2;
const size_t DCMTypes::PF_lastEntry            = 4;

/* writeXML flags */
const size_t DCMTypes::XF_addDocumentType = 1;
const size_t DCMTypes::XF_writeBinaryData = 2;
const size_t DCMTypes::XF_encodeBase64    = 4;


/*
 * CVS/RCS Log:
 * $Log: dctypes.cc,v $
 * Revision 1.3  2002-12-06 12:21:35  joergr
 * Enhanced "print()" function by re-working the implementation and replacing
 * the boolean "showFullData" parameter by a more general integer flag.
 *
 * Revision 1.2  2002/05/14 08:22:04  joergr
 * Added support for Base64 (MIME) encoded binary data.
 *
 * Revision 1.1  2002/04/25 10:13:12  joergr
 * Added support for XML output of DICOM objects.
 *
 *
 */
