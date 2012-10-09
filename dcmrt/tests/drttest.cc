/*
 *
 *  Copyright (c) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Test read and write methods of DICOM RT classes
 *
 *  Generated manually
 *  File created on 2008-12-08
 *  Last modified on 2012-10-09 by Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmrt/drtdose.h"
#include "dcmtk/dcmrt/drtimage.h"
#include "dcmtk/dcmrt/drtplan.h"
#include "dcmtk/dcmrt/drtstrct.h"
#include "dcmtk/dcmrt/drttreat.h"

#include "dcmtk/dcmdata/dcfilefo.h"

#define TEST_COPY_CONSTRUCTOR
#define TEST_ASSIGNMENT_OPERATOR

#define RTDOSE_FILENAME   "/share/dicom/images/dkfz-rt/Phantom/RD1.3.6.1.4.1.2452.6.2742071499.1301053063.852537260.1034784.dcm"
#define RTIMAGE_FILENAME  "/share/dicom/images/dkfz-rt/Phantom/RI1.3.6.1.4.1.2452.6.2440400960.1328333180.1418898622.324303.dcm"
#define RTPLAN_FILENAME   "/share/dicom/images/dkfz-rt/Phantom/RP1.3.6.1.4.1.2452.6.998701572.1323706291.2356805512.4049491.dcm"
#define RTSTRUCT_FILENAME "/share/dicom/images/dkfz-rt/Phantom/RS1.3.6.1.4.1.2452.6.2951369053.1166013784.715527822.1679960.dcm"
#define RTTREAT_FILENAME  "/share/dicom/images/dkfz-rt/rt_set_20090707/RT_RECORD/SER01/IMG004"


int main()
{
    OFCondition status;
    DcmFileFormat fileformat;
    OFLog::configure(OFLogger::INFO_LOG_LEVEL);
    DCMRT_INFO("DCMRT Test Program" << OFendl << "------------------" << OFendl);

    /* test RT dose */
    DRTDoseIOD rtdose;
    status = fileformat.loadFile(RTDOSE_FILENAME);
    if (status.good())
        status = rtdose.read(*fileformat.getDataset());
    DCMRT_INFO("Read RT Dose: " << status.text());

    fileformat.clear();
    status = rtdose.write(*fileformat.getDataset());
    DCMRT_INFO("Save RT Dose: " << status.text());
    if (status.good())
        status = fileformat.saveFile("rtdose.dcm");
    DCMRT_INFO("Write RT Dose: " << status.text() << OFendl);

#ifdef TEST_COPY_CONSTRUCTOR
    DRTDoseIOD rtdose2(rtdose);
    fileformat.clear();
    status = rtdose2.write(*fileformat.getDataset());
    DCMRT_INFO("Write copy of RT Dose: " << status.text());
    if (status.good())
        status = fileformat.saveFile("rtdose2.dcm");
    DCMRT_INFO("Save copy of RT Dose: " << status.text() << OFendl);
#endif

    /* test RT image */
    DRTImageIOD rtimage;
    status = fileformat.loadFile(RTIMAGE_FILENAME);
    if (status.good())
        status = rtimage.read(*fileformat.getDataset());
    DCMRT_INFO("Read RT Image: " << status.text());
    fileformat.clear();
    status = rtimage.write(*fileformat.getDataset());
    DCMRT_INFO("Write RT Image: " << status.text());
    if (status.good())
        status = fileformat.saveFile("rtimage.dcm");
    DCMRT_INFO("Save RT Image: " << status.text() << OFendl);

    /* test RT plan */
    DRTPlanIOD rtplan;
    status = fileformat.loadFile(RTPLAN_FILENAME);
    if (status.good())
        status = rtplan.read(*fileformat.getDataset());
    DCMRT_INFO("Read RT Plan: " << status.text());
    fileformat.clear();
    status = rtplan.write(*fileformat.getDataset());
    DCMRT_INFO("Write RT Plan: " << status.text());
    if (status.good())
        status = fileformat.saveFile("rtplan.dcm");
    DCMRT_INFO("Save RT Plan: " << status.text() << OFendl);

#ifdef TEST_ASSIGNMENT_OPERATOR
    DRTPlanIOD rtplan2 = rtplan;
    fileformat.clear();
    status = rtplan2.write(*fileformat.getDataset());
    DCMRT_INFO("Write copy of RT Plan: " << status.text());
    if (status.good())
        status = fileformat.saveFile("rtplan2.dcm");
    DCMRT_INFO("Save copy of RT Plan: " << status.text() << OFendl);
#endif

    /* test RT structure set */
    DRTStructureSetIOD rtstruct;
    status = fileformat.loadFile(RTSTRUCT_FILENAME);
    if (status.good())
        status = rtstruct.read(*fileformat.getDataset());
    DCMRT_INFO("Read RT Structure Set: " << status.text());
    fileformat.clear();
    status = rtstruct.write(*fileformat.getDataset());
    DCMRT_INFO("Write RT Structure Set: " << status.text());
    if (status.good())
        status = fileformat.saveFile("rtstruct.dcm");
    DCMRT_INFO("Save RT Structure Set: " << status.text() << OFendl);

    /* test RT treatment summary record */
    DRTTreatmentSummaryRecordIOD rttreat;
    status = fileformat.loadFile(RTTREAT_FILENAME);
    if (status.good())
        status = rttreat.read(*fileformat.getDataset());
    DCMRT_INFO("Read RT Treatment Summary Record: " << status.text());
    fileformat.clear();
    rttreat.getRequestAttributesSequence().clear();
    DCMRT_INFO("Fix invalid sequence elements: " << status.text());
    status = rttreat.write(*fileformat.getDataset());
    DCMRT_INFO("Write RT Treatment Summary Record: " << status.text());
    if (status.good())
        status = fileformat.saveFile("rttreat.dcm");
    DCMRT_INFO("Save RT Treatment Summary Record: " << status.text() << OFendl);

    return 0;
}
