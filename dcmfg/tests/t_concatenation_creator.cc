/*
 *
 *  Copyright (C) 2019-2024, OFFIS e.V.
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
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Tests for ConcatenationCreator class
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmfg/concatenationcreator.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/oftempf.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcfilefo.h"

// NEMA CT Test files are available at:
// ftp://medical.nema.org/medical/dicom/Multiframe/CT/nemamfct.images.tar.bz2
static OFString NEMA_ENHANCED_CT_DIR = "/home/michael/Downloads/NEMA/DISCIMG/IMAGES/";
static OFString CT1                  = NEMA_ENHANCED_CT_DIR + PATH_SEPARATOR + "CT0001";

OFTEST(dcmfg_concatenation_creator)
{
    if (!OFStandard::fileExists(CT1))
        return;
    DcmFileFormat dcmff;
    OFCondition result = dcmff.loadFile(CT1);
    ConcatenationCreator cc;
    result = cc.setCfgInput(dcmff.getDataset(), OFFalse /* do not take ownership */);
    OFCHECK(result.good());
    if (result.good())
    {
        size_t numInstances = cc.getNumInstances();
        OFCHECK(numInstances == 3);
        for (size_t i = 0; result.good() && (i < numInstances); i++)
        {
            DcmFileFormat destff;
            result = cc.writeNextInstance(*(destff.getDataset()));
            OFCHECK(result.good());
            if (result.good())
            {
                OFTempFile tf(O_RDWR, "", "", ".dcm");
                result = destff.saveFile(tf.getFilename(), EXS_LittleEndianExplicit);
                OFCHECK(result.good());
            }
        }
    }
}

OFTEST(dcmfg_concatenation_creator_fail)
{
    if (!OFStandard::fileExists(CT1))
        return;
    DcmFileFormat dcmff;
    OFCondition result = dcmff.loadFile(CT1);
    ConcatenationCreator cc;
    cc.setCfgFramesPerInstance(55);
    result = cc.setCfgInput(dcmff.getDataset(), OFFalse /* do not take ownership */);
    OFCHECK(result.good());
    OFTempFile tf(O_RDWR, "", "", ".dcm");
    result = cc.writeNextInstance(tf.getFilename());
    OFCHECK(result == FG_EC_NotEnoughFrames);
}
