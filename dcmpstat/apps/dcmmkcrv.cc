/*
 *
 *  Copyright (C) 1998-2017, OFFIS e.V.
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
 *  Module:  dcmpstat
 *
 *  Authors: Marco Eichelberg
 *
 *  Purpose: This application reads a DICOM image, adds a Curve and writes it back.
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmmkcrv"

static OFLogger dcmmkcrvLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define MAX_POINTS 65535

// ********************************************

#define SHORTCOL 3
#define LONGCOL 13

int main(int argc, char *argv[])
{
    const char *opt_inName = NULL;                     /* in file name */
    const char *opt_outName = NULL;                    /* out file name */
    const char *opt_curveName = NULL;                  /* curve file name */
    OFCmdUnsignedInt opt_data_vr = 4;
    OFCmdUnsignedInt opt_group   = 0;
    OFBool opt_poly = OFTrue;
    const char *opt_label = NULL;
    const char *opt_description = NULL;
    const char *opt_axis_x = NULL;
    const char *opt_axis_y = NULL;
    OFCmdUnsignedInt opt_curve_vr = 0;

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Add 2D curve data to image", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in", "DICOM input image file");
    cmd.addParam("curvedata-in", "curve data input file (text)");
    cmd.addParam("dcmfile-out", "DICOM output filename");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",           "-h",    "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",                 "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);

    cmd.addGroup("curve creation options:");
      cmd.addSubGroup("curve type:");
        cmd.addOption("--poly",        "-r",    "create as POLY curve (default)");
        cmd.addOption("--roi",         "+r",    "create as ROI curve");

      cmd.addSubGroup("curve value representation:");
        cmd.addOption("--data-vr",     "+v", 1, "[n]umber: integer 0..4 (default: 4)",
                                                "select curve data VR: 0=US, 1=SS, 2=FL, 3=FD, 4=SL");
        cmd.addOption("--curve-vr",    "-c", 1, "[n]umber: integer 0..2 (default: 0)",
                                                "select VR with which the Curve Data element is written\n"
                                                "0=VR according to --data-vr, 1=OB, 2=OW");
      cmd.addSubGroup("repeating group:");
        cmd.addOption("--group",       "-g", 1, "[n]umber: integer 0..15 (default: 0)",
                                                "select repeating group: 0=0x5000, 1=0x5002 etc.");
      cmd.addSubGroup("curve description:");
        cmd.addOption("--label",       "-l", 1, "s: string",
                                                "set Curve Label to s (default: absent)");
        cmd.addOption("--description", "+d", 1, "s: string",
                                                "set Curve Description to s (default: absent)");
        cmd.addOption("--axis",        "-a", 2, "x: string, y: string",
                                                "set Axis Units to x\\y (default: absent)");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv))
    {
      /* check exclusive options first */
      if (cmd.hasExclusiveOption())
      {
        if (cmd.findOption("--version"))
        {
            app.printHeader(OFTrue /*print host identifier*/);
            COUT << OFendl << "External libraries used:";
#ifdef WITH_ZLIB
            COUT << OFendl << "- ZLIB, Version " << zlibVersion() << OFendl;
#else
            COUT << " none" << OFendl;
#endif
            return 0;
         }
      }

      /* command line parameters */
      cmd.getParam(1, opt_inName);
      cmd.getParam(2, opt_curveName);
      cmd.getParam(3, opt_outName);

      OFLog::configureFromCommandLine(cmd, app);

      cmd.beginOptionBlock();
      if (cmd.findOption("--roi")) opt_poly = OFFalse;
      if (cmd.findOption("--poly")) opt_poly = OFTrue;
      cmd.endOptionBlock();


      if (cmd.findOption("--data-vr")) app.checkValue(cmd.getValueAndCheckMinMax(opt_data_vr, 0, 4));
      if (cmd.findOption("--curve-vr")) app.checkValue(cmd.getValueAndCheckMinMax(opt_curve_vr, 0, 2));
      if (cmd.findOption("--group")) app.checkValue(cmd.getValueAndCheckMinMax(opt_group, 0, 15));
      if (cmd.findOption("--label")) app.checkValue(cmd.getValue(opt_label));
      if (cmd.findOption("--description")) app.checkValue(cmd.getValue(opt_description));
      if (cmd.findOption("--axis"))
      {
          app.checkValue(cmd.getValue(opt_axis_x));
          app.checkValue(cmd.getValue(opt_axis_y));
      }
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmmkcrvLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded()) {
        OFLOG_WARN(dcmmkcrvLogger, "no data dictionary loaded, "
            << "check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    DcmFileFormat *fileformat = new DcmFileFormat;
    if (!fileformat)
    {
      OFLOG_FATAL(dcmmkcrvLogger, "memory exhausted");
      return 1;
    }

    OFCondition error = fileformat->loadFile(opt_inName);
    if (! error.good())
    {
      OFLOG_FATAL(dcmmkcrvLogger, error.text() << ": reading file: " <<  opt_inName);
      return 1;
    }

    DcmDataset *dataset = fileformat->getDataset();

    /* read curve data */

    STD_NAMESPACE ifstream curvefile(opt_curveName);
    if (!curvefile)
    {
      OFLOG_FATAL(dcmmkcrvLogger, "cannot open curve file: " << opt_curveName);
      return 1;
    }

    double *array = new double[MAX_POINTS*2];
    if (array==NULL)
    {
      OFLOG_FATAL(dcmmkcrvLogger, "out of memory");
      return 1;
    }

    size_t idx=0;
    double d;
    OFBool done = OFFalse;
    while ((curvefile.good())&&(!done))
    {
      curvefile >> d;
      if (idx == MAX_POINTS*2)
      {
        OFLOG_WARN(dcmmkcrvLogger, "too many curve points, can only handle " << MAX_POINTS << ".");
        done = OFTrue;
      } else {
        if (curvefile.good()) array[idx++] = d;
      }
    }

    /* create curve description data */
    DcmUnsignedShort *curveDimensions = new DcmUnsignedShort(DCM_RETIRED_CurveDimensions);
    DcmUnsignedShort *numberOfPoints = new DcmUnsignedShort(DCM_RETIRED_NumberOfPoints);
    DcmCodeString    *typeOfData = new DcmCodeString(DCM_RETIRED_TypeOfData);
    DcmUnsignedShort *dataValueRepresentation = new DcmUnsignedShort(DCM_RETIRED_DataValueRepresentation);
    DcmLongString    *curveDescription = new DcmLongString(DCM_RETIRED_CurveDescription);
    DcmShortString   *axisUnits = new DcmShortString(DCM_RETIRED_AxisUnits);
    DcmLongString    *curveLabel = new DcmLongString(DCM_RETIRED_CurveLabel);

    if ((!curveDimensions)||(!numberOfPoints)||(!typeOfData)||(!dataValueRepresentation)
       ||(!curveDescription)||(!axisUnits)||(!curveLabel))
    {
      OFLOG_FATAL(dcmmkcrvLogger, "out of memory");
      delete [] array;
      return 1;
    }

    curveDimensions->setGTag(OFstatic_cast(Uint16,0x5000+2*opt_group));
    curveDimensions->putUint16(2,0);
    dataset->insert(curveDimensions, OFTrue);

    numberOfPoints->setGTag(OFstatic_cast(Uint16,0x5000+2*opt_group));
    numberOfPoints->putUint16(OFstatic_cast(Uint16,idx/2),0);
    dataset->insert(numberOfPoints, OFTrue);

    typeOfData->setGTag(OFstatic_cast(Uint16,0x5000+2*opt_group));
    if (opt_poly) typeOfData->putString("POLY"); else typeOfData->putString("ROI");
    dataset->insert(typeOfData, OFTrue);

    dataValueRepresentation->setGTag(OFstatic_cast(Uint16,0x5000+2*opt_group));
    dataValueRepresentation->putUint16(OFstatic_cast(Uint16,opt_data_vr),0);
    dataset->insert(dataValueRepresentation, OFTrue);

    if (opt_description)
    {
      curveDescription->setGTag(OFstatic_cast(Uint16,0x5000+2*opt_group));
      curveDescription->putString(opt_description);
      dataset->insert(curveDescription, OFTrue);
    }

    if (opt_label)
    {
      curveLabel->setGTag(OFstatic_cast(Uint16,0x5000+2*opt_group));
      curveLabel->putString(opt_label);
      dataset->insert(curveLabel, OFTrue);
    }

    if (opt_axis_x && opt_axis_y)
    {
      OFString aString(opt_axis_x);
      aString += "\\";
      aString += opt_axis_y;
      axisUnits->setGTag(OFstatic_cast(Uint16,0x5000+2*opt_group));
      axisUnits->putString(aString.c_str());
      dataset->insert(axisUnits, OFTrue);
    }

    /* now create the curve itself */
    void *rawData;
    size_t i;
    size_t byteLength = 0;
    size_t align=0;

    switch (opt_data_vr)
    {
      case 0: // US
        OFLOG_INFO(dcmmkcrvLogger, "creating curve, VR=US, points=" << idx/2);
        rawData = new Uint16[idx];
        if (rawData==NULL) { OFLOG_FATAL(dcmmkcrvLogger, "out of memory"); delete [] array; return 1; }
        byteLength = sizeof(Uint16)*idx;
        for (i=0; i<idx; i++) OFstatic_cast(Uint16 *,rawData)[i] = OFstatic_cast(Uint16,array[i]);
        align = sizeof(Uint16);
        break;
      case 1: // SS
        OFLOG_INFO(dcmmkcrvLogger, "creating curve, VR=SS, points=" << idx/2);
        rawData = new Sint16[idx];
        if (rawData==NULL) { OFLOG_FATAL(dcmmkcrvLogger, "out of memory"); delete [] array; return 1; }
        byteLength = sizeof(Sint16)*idx;
        for (i=0; i<idx; i++) OFstatic_cast(Sint16 *,rawData)[i] = OFstatic_cast(Sint16,array[i]);
        align = sizeof(Sint16);
        break;
      case 2: // FL
        OFLOG_INFO(dcmmkcrvLogger, "creating curve, VR=FL, points=" << idx/2);
        rawData = new Float32[idx];
        if (rawData==NULL) { OFLOG_FATAL(dcmmkcrvLogger, "out of memory"); delete [] array; return 1; }
        byteLength = sizeof(Float32)*idx;
        for (i=0; i<idx; i++) OFstatic_cast(Float32 *,rawData)[i] = OFstatic_cast(Float32,array[i]);
        align = sizeof(Float32);
        break;
      case 3: // FD
        OFLOG_INFO(dcmmkcrvLogger, "creating curve, VR=FD, points=" << idx/2);
        rawData = new Float64[idx];
        if (rawData==NULL) { OFLOG_FATAL(dcmmkcrvLogger, "out of memory"); delete [] array; return 1; }
        byteLength = sizeof(Float64)*idx;
        for (i=0; i<idx; i++) OFstatic_cast(Float64 *,rawData)[i] = OFstatic_cast(Float64,array[i]);
        align = sizeof(Float64);
        break;
      case 4: // SL
        OFLOG_INFO(dcmmkcrvLogger, "creating curve, VR=SL, points=" << idx/2);
        rawData = new Sint32[idx];
        if (rawData==NULL) { OFLOG_FATAL(dcmmkcrvLogger, "out of memory"); delete [] array; return 1; }
        byteLength = sizeof(Sint32)*idx;
        for (i=0; i<idx; i++) OFstatic_cast(Sint32 *,rawData)[i] = OFstatic_cast(Sint32,array[i]);
        align = sizeof(Sint32);
        break;
      default:
        OFLOG_FATAL(dcmmkcrvLogger, "unknown data VR, bailing out");
        delete [] array;
        return 1;
    }
    delete [] array;

    DcmElement *element = NULL;
    switch (opt_curve_vr)
    {
      case 0: // explicit VR
        switch (opt_data_vr)
        {
          case 0: // US
            OFLOG_INFO(dcmmkcrvLogger, "encoding curve data element as VR=US");
            element = new DcmUnsignedShort(DcmTag(DCM_RETIRED_CurveData, EVR_US));
            if (element==NULL) { OFLOG_FATAL(dcmmkcrvLogger, "out of memory"); return 1; }
            element->putUint16Array(OFstatic_cast(Uint16 *,rawData), OFstatic_cast(Uint32, byteLength/sizeof(Uint16)));
            break;
          case 1: // SS
            OFLOG_INFO(dcmmkcrvLogger, "encoding curve data element as VR=SS");
            element = new DcmSignedShort(DcmTag(DCM_RETIRED_CurveData, EVR_SS));
            if (element==NULL) { OFLOG_FATAL(dcmmkcrvLogger, "out of memory"); return 1; }
            element->putSint16Array(OFstatic_cast(Sint16 *,rawData), OFstatic_cast(Uint32, byteLength/sizeof(Sint16)));
            break;
          case 2: // FL
            OFLOG_INFO(dcmmkcrvLogger, "encoding curve data element as VR=FL");
            element = new DcmFloatingPointSingle(DcmTag(DCM_RETIRED_CurveData, EVR_FL));
            if (element==NULL) { OFLOG_FATAL(dcmmkcrvLogger, "out of memory"); return 1; }
            element->putFloat32Array(OFstatic_cast(Float32 *,rawData), OFstatic_cast(Uint32, byteLength/sizeof(Float32)));
            break;
          case 3: // FD
            OFLOG_INFO(dcmmkcrvLogger, "encoding curve data element as VR=FD");
            element = new DcmFloatingPointDouble(DcmTag(DCM_RETIRED_CurveData, EVR_FD));
            if (element==NULL) { OFLOG_FATAL(dcmmkcrvLogger, "out of memory"); return 1; }
            element->putFloat64Array(OFstatic_cast(Float64 *,rawData), OFstatic_cast(Uint32, byteLength/sizeof(Float64)));
            break;
          case 4: // SL
            OFLOG_INFO(dcmmkcrvLogger, "encoding curve data element as VR=SL");
            element = new DcmSignedLong(DcmTag(DCM_RETIRED_CurveData, EVR_SL));
            if (element==NULL) { OFLOG_FATAL(dcmmkcrvLogger, "out of memory"); return 1; }
            element->putSint32Array(OFstatic_cast(Sint32 *,rawData), OFstatic_cast(Uint32, byteLength/sizeof(Sint32)));
            break;
          default:
            OFLOG_FATAL(dcmmkcrvLogger, "unknown data VR, bailing out");
            return 1;
        }
        element->setGTag(OFstatic_cast(Uint16,0x5000+2*opt_group));
        dataset->insert(element, OFTrue);
        break;
      case 1: // OB
        // create little endian byte order
        OFLOG_INFO(dcmmkcrvLogger, "encoding curve data element as VR=OB");
        swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, rawData, OFstatic_cast(Uint32, byteLength), align);
        element = new DcmOtherByteOtherWord(DCM_RETIRED_CurveData);
        if (element==NULL) { OFLOG_FATAL(dcmmkcrvLogger, "out of memory"); return 1; }
        element->setGTag(OFstatic_cast(Uint16,0x5000+2*opt_group));
        element->setVR(EVR_OB);
        element->putUint8Array(OFstatic_cast(Uint8 *,rawData), OFstatic_cast(Uint32, byteLength));
        dataset->insert(element, OFTrue);
        break;
      case 2: // OW
        // create little endian byte order
        OFLOG_INFO(dcmmkcrvLogger, "encoding curve data element as VR=OW");
        if (align != sizeof(Uint16))
        {
          swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, rawData, OFstatic_cast(Uint32, byteLength), align);
          swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, rawData, OFstatic_cast(Uint32, byteLength), sizeof(Uint16));
        }
        element = new DcmOtherByteOtherWord(DCM_RETIRED_CurveData);
        if (element==NULL) { OFLOG_FATAL(dcmmkcrvLogger, "out of memory"); return 1; }
        element->setGTag(OFstatic_cast(Uint16,0x5000+2*opt_group));
        element->setVR(EVR_OW);
        element->putUint16Array(OFstatic_cast(Uint16 *,rawData), OFstatic_cast(Uint32, byteLength/sizeof(Uint16)));
        dataset->insert(element, OFTrue);
        break;
      default:
        OFLOG_FATAL(dcmmkcrvLogger, "unsupported VR, bailing out");
        return 1;
    }
    /* write back */

    error = fileformat->saveFile(opt_outName, dataset->getOriginalXfer());
    if (error != EC_Normal)
    {
      OFLOG_FATAL(dcmmkcrvLogger, error.text()
          << ": writing file: " <<  opt_outName);
          return 1;
    }

    return 0;

}
