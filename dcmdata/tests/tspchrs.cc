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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for class DcmSpecificCharacterSet
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2011-10-26 16:13:03 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcspchrs.h"
#include "dcmtk/dcmdata/dctypes.h"


OFTEST(dcmdata_specificCharacterSet_1)
{
    DcmSpecificCharacterSet converter;
    if (converter.isConversionLibraryAvailable())
    {
        // try to select all character sets without code extensions
        OFCHECK(converter.selectCharacterSet("").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 6  ").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 100").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 101").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 109").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 110").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 144").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 127").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 126").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 138").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 148").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 13 ").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 166").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 192").good());
        OFCHECK(converter.selectCharacterSet("  GB18030 ").good());
        // the following should fail
        OFCHECK(converter.selectCharacterSet("DCMTK").bad());
        OFCHECK(converter.selectCharacterSet(" \\ ").bad());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 100").bad());
        OFCHECK(converter.selectCharacterSet("ISO 2022 IR 100\\ISO 2022 IR 126").bad());
    } else {
        // in case there is no libiconv, report a warning but do not fail
        DCMDATA_WARN("Cannot test DcmSpecificCharacterSet since the underlying character set conversion library is not available");
    }
}

OFTEST(dcmdata_specificCharacterSet_2)
{
    DcmSpecificCharacterSet converter;
    if (converter.isConversionLibraryAvailable())
    {
        OFString resultStr;
        // check whether string conversion from Latin-1 to UTF-8 works
        OFCHECK(converter.selectCharacterSet("ISO_IR 100").good());
        OFCHECK(converter.convertToUTF8String("J\366rg", resultStr).good());
        OFCHECK_EQUAL(resultStr, "J\303\266rg");
        OFCHECK(converter.convertToUTF8String("J\351r\364me", resultStr).good());
        OFCHECK_EQUAL(resultStr, "J\303\251r\303\264me");
        // check whether string conversion from UTF-8 to UTF-8 works :-)
        OFCHECK(converter.selectCharacterSet("ISO_IR 192").good());
        OFCHECK(converter.convertToUTF8String("J\303\266rg", resultStr).good());
        OFCHECK_EQUAL(resultStr, "J\303\266rg");
        OFCHECK(converter.convertToUTF8String("J\303\251r\303\264me", resultStr).good());
        OFCHECK_EQUAL(resultStr, "J\303\251r\303\264me");
    } else {
        // in case there is no libiconv, report a warning but do not fail
        DCMDATA_WARN("Cannot test DcmSpecificCharacterSet since the underlying character set conversion library is not available");
    }
}


/*
 *
 * CVS/RCS Log:
 * $Log: tspchrs.cc,v $
 * Revision 1.1  2011-10-26 16:13:03  joergr
 * Added helper class for converting between different DICOM character sets.
 * This initial version only supports the conversion to UTF-8 (Unicode) and only
 * from DICOM characters sets without code extension techniques (i.e. ISO 2022).
 *
 *
 *
 */
