/*
 *
 *  Copyright (C) 2002-2016, OFFIS e.V.
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
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctypes.h"

OFLogger DCM_dcmdataLogger = OFLog::getLogger("dcmtk.dcmdata");

/* print flags */
const size_t DCMTypes::PF_shortenLongTagValues  = 1 << 0;
const size_t DCMTypes::PF_showTreeStructure     = 1 << 1;
const size_t DCMTypes::PF_doNotMapUIDsToNames   = 1 << 2;
const size_t DCMTypes::PF_convertToMarkup       = 1 << 3;
const size_t DCMTypes::PF_convertToOctalNumbers = 1 << 4;
const size_t DCMTypes::PF_useANSIEscapeCodes    = 1 << 5;

/* write XML flags */
const size_t DCMTypes::XF_addDocumentType       = 1 << 0;
const size_t DCMTypes::XF_writeBinaryData       = 1 << 1;
const size_t DCMTypes::XF_encodeBase64          = 1 << 2;
const size_t DCMTypes::XF_useXMLNamespace       = 1 << 3;
const size_t DCMTypes::XF_embedDocumentType     = 1 << 4;
const size_t DCMTypes::XF_omitDataElementName   = 1 << 5;
const size_t DCMTypes::XF_convertNonASCII       = 1 << 6;
const size_t DCMTypes::XF_useNativeModel        = 1 << 7;

/* character set conversion flags */
const size_t DCMTypes::CF_transliterate         = 1 << 0;
const size_t DCMTypes::CF_discardIllegal        = 1 << 1;
