/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: global type and constant definitions
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-29 10:57:21 $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctypes.h"

OFLogger DCM_dcmdataGetLogger()
{
    // We don't just use a global variable, because constructors of globals are
    // executed in random order. This guarantees that the OFLogger is constructed
    // before first use.
    static OFLogger DCM_dcmdataLogger = OFLog::getLogger("dcmtk.dcmdata");
    return DCM_dcmdataLogger;
}

/* print flags */
const size_t DCMTypes::PF_shortenLongTagValues = 1 << 0;
const size_t DCMTypes::PF_showTreeStructure    = 1 << 1;
const size_t DCMTypes::PF_doNotMapUIDsToNames  = 1 << 2;
const size_t DCMTypes::PF_convertToMarkup      = 1 << 3;
const size_t DCMTypes::PF_useANSIEscapeCodes   = 1 << 4;

/* writeXML flags */
const size_t DCMTypes::XF_addDocumentType      = 1 << 0;
const size_t DCMTypes::XF_writeBinaryData      = 1 << 1;
const size_t DCMTypes::XF_encodeBase64         = 1 << 2;
const size_t DCMTypes::XF_useDcmtkNamespace    = 1 << 3;
const size_t DCMTypes::XF_embedDocumentType    = 1 << 4;
const size_t DCMTypes::XF_omitDataElementName  = 1 << 5;


/*
 * CVS/RCS Log:
 * $Log: dctypes.cc,v $
 * Revision 1.13  2010-10-29 10:57:21  joergr
 * Added support for colored output to the print() method.
 *
 * Revision 1.12  2010-10-14 13:14:09  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.11  2009-11-04 09:58:10  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.10  2009-01-06 16:27:07  joergr
 * Reworked print() output format for option PF_showTreeStructure.
 *
 * Revision 1.9  2008-04-28 09:22:41  meichel
 * New flag DCMTypes::PF_convertToMarkup causes DcmObject::print to
 *   quote non-ASCII string content as XML markup
 *
 * Revision 1.8  2008-02-26 16:57:25  joergr
 * Added new print flag that disables the mapping of well-known UID numbers to
 * their associated names (e.g. transfer syntax or SOP class).
 *
 * Revision 1.7  2006/05/11 08:45:45  joergr
 * Added new option that allows to omit the element name in the XML output.
 *
 * Revision 1.6  2005/12/08 15:41:41  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.5  2003/04/22 08:19:24  joergr
 * Added new command line option which allows to embed the content of the DTD
 * instead of referencing the DTD file.
 *
 * Revision 1.4  2003/04/01 14:57:20  joergr
 * Added support for XML namespaces.
 *
 * Revision 1.3  2002/12/06 12:21:35  joergr
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
