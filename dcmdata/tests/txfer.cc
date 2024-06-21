/*
 *
 *  Copyright (C) 2024, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
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
 *  Purpose: test program for class DcmXfer
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcuid.h"


OFTEST(dcmdata_xferLookup_1)
{
    /* look up the enum */
    DcmXfer xfer(EXS_LittleEndianExplicit);
    OFCHECK(xfer.isValid());
    OFCHECK_EQUAL(OFString(xfer.getXferID()), OFString(UID_LittleEndianExplicitTransferSyntax));
    OFCHECK_EQUAL(OFString(xfer.getXferName()), "Little Endian Explicit");
    OFCHECK_EQUAL(OFString(xfer.getXferKeyword()), "ExplicitVRLittleEndian");
    OFCHECK(xfer.isLittleEndian());
    OFCHECK(xfer.isExplicitVR());
    OFCHECK(xfer.isStandard());
    OFCHECK(xfer.usesNativeFormat());
    OFCHECK(!xfer.isDatasetCompressed());
    OFCHECK(xfer.isPixelDataUncompressed());
    OFCHECK(!xfer.isPixelDataFragmentable());
    OFCHECK(!xfer.isLosslessCompressed());
}


OFTEST(dcmdata_xferLookup_2)
{
    /* look up the name */
    DcmXfer xfer("JPIP HTJ2K Referenced Deflate");
    OFCHECK(xfer.isValid());
    OFCHECK_EQUAL(xfer.getXfer(), EXS_JPIPHTJ2KReferencedDeflate);
    OFCHECK_EQUAL(OFString(xfer.getXferID()), OFString(UID_JPIPHTJ2KReferencedDeflateTransferSyntax));
    OFCHECK_EQUAL(OFString(xfer.getXferKeyword()), "JPIPHTJ2KReferencedDeflate");
    OFCHECK(xfer.isLittleEndian());
    OFCHECK(xfer.isExplicitVR());
    OFCHECK(xfer.isStandard());
    OFCHECK(xfer.isDatasetCompressed());
    OFCHECK(!xfer.usesEncapsulatedFormat());
    OFCHECK(!xfer.isPixelDataCompressed());
    OFCHECK(xfer.usesReferencedPixelData());
    OFCHECK(xfer.isLosslessCompressed());
}


OFTEST(dcmdata_xferLookup_3)
{
    /* look up the UID */
    DcmXfer xfer(UID_FragmentableMPEG2MainProfileHighLevelTransferSyntax);
    OFCHECK(xfer.isValid());
    OFCHECK_EQUAL(xfer.getXfer(), EXS_FragmentableMPEG2MainProfileHighLevel);
    OFCHECK_EQUAL(OFString(xfer.getXferName()), "Fragmentable MPEG2 Main Profile / High Level");
    OFCHECK_EQUAL(OFString(xfer.getXferKeyword()), "MPEG2MPHLF");
    OFCHECK(xfer.isLittleEndian());
    OFCHECK(xfer.isExplicitVR());
    OFCHECK(xfer.isStandard());
    OFCHECK(!xfer.isDatasetCompressed());
    OFCHECK(xfer.usesEncapsulatedFormat());
    OFCHECK(xfer.isPixelDataCompressed());
    OFCHECK(xfer.isPixelDataLossyCompressed());
    OFCHECK(xfer.isPixelDataFragmentable());
    OFCHECK(!xfer.isLosslessCompressed());
}


OFTEST(dcmdata_xferLookup_4)
{
    /* look up unsuccessful */
    DcmXfer xfer("some invalid name");
    OFCHECK(!xfer.isValid());
}
