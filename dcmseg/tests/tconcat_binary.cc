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
 *  Module:  dcmseg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Test for creating, writing and reading binary Segmentation objects
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmfg/concatenationcreator.h"
#include "dcmtk/dcmfg/concatenationloader.h"
#include "dcmtk/dcmseg/segdoc.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdict.h"

const OFString ATLAS_FILE          = "/home/michael/data/dcm/seg_atlas.dcm";
const size_t NUM_FRAMES_PER_CONCAT = 24;

OFTEST(dcmseg_concat_binary)
{
    if (!OFStandard::fileExists(ATLAS_FILE))
    {
        DCMSEG_DEBUG("Will not run dcmseg_concat_binary test, input file " << ATLAS_FILE << " missing");
        return;
    }
    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK(dcmDataDict.isDictionaryLoaded());
        return;
    }

    // Loading
    DcmSegmentation* seg = NULL;
    OFCondition result   = DcmSegmentation::loadFile(ATLAS_FILE, seg);
    if (result.good())
    {
        ConcatenationCreator cc;
        result = cc.setCfgFramesPerInstance(NUM_FRAMES_PER_CONCAT);
        if (result.good())
        {
            seg->setCheckFGOnWrite(OFFalse);
            result = seg->writeConcatenation(cc);
            if (result.good())
            {
                size_t n = 0;
                do
                {
                    OFStringStream ss;
                    ss << "/tmp/concat_" << n;
                    result = cc.writeNextInstance(ss.str().c_str());
                    n++;
                } while (result.good());
                if (result != FG_EC_ConcatenationComplete)
                {
                    DCMSEG_ERROR("Could not write concatenation instance: " << result.text());
                    OFCHECK(result.good());
                }
            }
            else
            {
                DCMSEG_ERROR("Could not write concatenation: " << result.text());
                OFCHECK(result.good());
            }
        }
        else
        {
            DCMSEG_ERROR("Could not configure concatenation number of frames to " << NUM_FRAMES_PER_CONCAT << ": "
                                                                                   << result.text());
            OFCHECK(result.good());
        }
    }
    delete seg;
    seg = NULL;

    if (result == FG_EC_ConcatenationComplete)
    {
        ConcatenationLoader cl;
        result = cl.scan("/tmp", "concat*", OFFalse);
        if (result.good())
        {
            if (cl.getInfo().size() == 1)
            {
                OFVector<DcmIODTypes::Frame*> frames;
                DcmFileFormat dcmff;
                result = DcmSegmentation::loadConcatenation(cl, cl.getInfo().begin()->first, seg);
                if (result.good())
                {
                    seg->setCheckFGOnWrite(OFFalse);
                    result = seg->saveFile("/tmp/atlas_copy.dcm", EXS_LittleEndianExplicit);
                    if (result.bad())
                    {
                        DCMSEG_ERROR("Unable to write re-assembled Concatenation Source instance: " << result.text());
                        OFCHECK(result.good());
                    }
                }
                delete seg;
            }
            else
            {
                DCMSEG_ERROR("Unable to load concatenation: Expected 1 but found "
                             << cl.getInfo().size() << " concatenations in directory /tmp");
                OFCHECK(cl.getInfo().size() > 1);
            }
        }
    }
}
