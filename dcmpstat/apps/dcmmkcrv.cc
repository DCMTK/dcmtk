/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Module:  dcmpstat
 *
 *  Authors: Marco Eichelberg
 *
 *  Purpose: This application reads a DICOM image, adds a Curve and writes it back.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-03 14:13:24 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/dcmmkcrv.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */
 

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include <fstream.h>
#include "dctk.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dcuid.h"    /* for dcmtk version name */

#define OFFIS_CONSOLE_APPLICATION "dcmmkcrv"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define MAX_POINTS 65535

OFBool opt_verbose = OFFalse;

// ********************************************

#define SHORTCOL 3
#define LONGCOL 13

int main(int argc, char *argv[])
{
    OFString str;   
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
    
    SetDebugLevel(( 0 ));

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Add 2D curve data to image", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmimg-in",      "DICOM input image file");
    cmd.addParam("curvedata-in",   "curve data input file (text)");
    cmd.addParam("dcmimg-out",     "DICOM output filename");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                      "-h",        "print this help text and exit");
     cmd.addOption("--verbose",                   "-v",        "verbose mode, print processing details");
     cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");
    cmd.addGroup("curve creation options:");
      cmd.addSubGroup("curve type:");
        cmd.addOption("--poly",        "-r",     "create as POLY curve (default)");                                         
        cmd.addOption("--roi",         "+r",     "create as ROI curve");

      cmd.addSubGroup("curve value representation:");
        cmd.addOption("--data-vr",     "+v", 1,  "[n]umber: integer 0..4 (default: 4)",
                                                 "select curve data VR: 0=US, 1=SS, 2=FL, 3=FD, 4=SL");
        cmd.addOption("--curve-vr",    "-c", 1,  "[n]umber: integer 0..2 (default: 0)",
                                                 "select VR with which the Curve Data element is written\n"
                                                 "0=VR according to --data-vr, 1=OB, 2=OW");
      cmd.addSubGroup("repeating group:");
        cmd.addOption("--group",       "-g", 1,  "[n]umber: integer 0..15 (default: 0)",
                                                 "select repeating group: 0=0x5000, 1=0x5002 etc.");
      cmd.addSubGroup("curve description:");
        cmd.addOption("--label",       "-l", 1,  "s: string",
                                                 "set Curve Label to s (default: absent)");
        cmd.addOption("--description", "+d", 1,  "s: string",
                                                 "set Curve Description to s (default: absent)");
        cmd.addOption("--axis",        "-a", 2,  "x: string, y: string",
                                                 "set Axis Units to x\\y (default: absent)");

    /* evaluate command line */                           
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      cmd.getParam(1, opt_inName);
      cmd.getParam(2, opt_curveName);
      cmd.getParam(3, opt_outName);

      if (cmd.findOption("--verbose")) opt_verbose=OFTrue;
      if (cmd.findOption("--debug")) SetDebugLevel(3);
      cmd.beginOptionBlock();
      if (cmd.findOption("--roi")) opt_poly = OFFalse;
      if (cmd.findOption("--poly")) opt_poly = OFTrue;                
      cmd.endOptionBlock();


      if (cmd.findOption("--data-vr")) app.checkValue(cmd.getValue(opt_data_vr,0,(OFCmdUnsignedInt)4));
      if (cmd.findOption("--curve-vr")) app.checkValue(cmd.getValue(opt_curve_vr,0,(OFCmdUnsignedInt)2));
      if (cmd.findOption("--group")) app.checkValue(cmd.getValue(opt_group,0,(OFCmdUnsignedInt)15));
      if (cmd.findOption("--label")) app.checkValue(cmd.getValue(opt_label));
      if (cmd.findOption("--description")) app.checkValue(cmd.getValue(opt_description));
      if (cmd.findOption("--axis"))
      {
          app.checkValue(cmd.getValue(opt_axis_x));
          app.checkValue(cmd.getValue(opt_axis_y));
      }
    }
  
    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded()) {
	CERR << "Warning: no data dictionary loaded, "
	     << "check environment variable: "
	     << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }

    DcmFileStream inf(opt_inName, DCM_ReadMode);
    if ( inf.Fail() ) {
	CERR << "cannot open file: " << opt_inName << endl;
        return 1;
    }

    DcmFileFormat *fileformat = new DcmFileFormat;
    E_Condition error = EC_Normal;
    if (!fileformat)
    {
      CERR << "memory exhausted\n";
      return 1;
    }

    fileformat->transferInit();
    error = fileformat -> read(inf);
    fileformat->transferEnd();

    if (error != EC_Normal) 
    {
	CERR << "Error: "  
	     << dcmErrorConditionToString(error)
	     << ": reading file: " <<  opt_inName << endl;
	return 1;
    }
    DcmDataset *dataset = fileformat->getDataset();
    
    /* read curve data */    

    ifstream curvefile(opt_curveName);
    if (!curvefile)
    {
      CERR << "cannot open curve file: " << opt_curveName << endl;
      return 1;
    }
    
    double *array = new double[MAX_POINTS*2];
    if (array==NULL)
    {
      CERR << "out of memory" << endl;
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
      	CERR << "warning: too many curve points, can only handle " << MAX_POINTS << "." << endl;
        done = OFTrue;
      } else {
      	if (curvefile.good()) array[idx++] = d;
      }
    }
    
    /* create curve description data */
    DcmUnsignedShort *curveDimensions = new DcmUnsignedShort(DCM_CurveDimensions);
    DcmUnsignedShort *numberOfPoints = new DcmUnsignedShort(DCM_NumberOfPoints);
    DcmCodeString    *typeOfData = new DcmCodeString(DCM_TypeOfData);
    DcmUnsignedShort *dataValueRepresentation = new DcmUnsignedShort(DCM_DataValueRepresentation);
    DcmLongString    *curveDescription = new DcmLongString(DCM_CurveDescription);
    DcmShortString   *axisUnits = new DcmShortString(DCM_AxisUnits);
    DcmLongString    *curveLabel = new DcmLongString(DCM_CurveLabel);
    
    if ((!curveDimensions)||(!numberOfPoints)||(!typeOfData)||(!dataValueRepresentation)
       ||(!curveDescription)||(!axisUnits)||(!curveLabel))
    {
      CERR << "out of memory" << endl;
      return 1;
    }
    
    curveDimensions->setGTag((Uint16)(0x5000+2*opt_group));
    curveDimensions->putUint16(2,0);
    dataset->insert(curveDimensions, OFTrue);

    numberOfPoints->setGTag((Uint16)(0x5000+2*opt_group));
    numberOfPoints->putUint16((Uint16)(idx/2),0);
    dataset->insert(numberOfPoints, OFTrue);
    
    typeOfData->setGTag((Uint16)(0x5000+2*opt_group));
    if (opt_poly) typeOfData->putString("POLY"); else typeOfData->putString("ROI");
    dataset->insert(typeOfData, OFTrue);

    dataValueRepresentation->setGTag((Uint16)(0x5000+2*opt_group));
    dataValueRepresentation->putUint16((Uint16)opt_data_vr,0);
    dataset->insert(dataValueRepresentation, OFTrue);
    
    if (opt_description)
    {
      curveDescription->setGTag((Uint16)(0x5000+2*opt_group));
      curveDescription->putString(opt_description);
      dataset->insert(curveDescription, OFTrue);  
    }

    if (opt_label)
    {
      curveLabel->setGTag((Uint16)(0x5000+2*opt_group));
      curveLabel->putString(opt_label);
      dataset->insert(curveLabel, OFTrue);  
    }
    
    if (opt_axis_x && opt_axis_y)
    {
      OFString aString(opt_axis_x);
      aString += "\\";
      aString += opt_axis_y;
      axisUnits->setGTag((Uint16)(0x5000+2*opt_group));
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
        if (opt_verbose) CERR << "creating curve, VR=US, points=" << idx/2 << endl;
        rawData = new Uint16[idx];
        if (rawData==NULL) { CERR << "out of memory" << endl; return 1; }
        byteLength = sizeof(Uint16)*idx;
        for (i=0; i<idx; i++) ((Uint16 *)rawData)[i] = (Uint16)(array[i]);
        align = sizeof(Uint16);
        break;
      case 1: // SS
        if (opt_verbose) CERR << "creating curve, VR=SS, points=" << idx/2 << endl;
        rawData = new Sint16[idx];
        if (rawData==NULL) { CERR << "out of memory" << endl; return 1; }
        byteLength = sizeof(Sint16)*idx;
        for (i=0; i<idx; i++) ((Sint16 *)rawData)[i] = (Sint16)(array[i]);
        align = sizeof(Sint16);
        break;
      case 2: // FL
        if (opt_verbose) CERR << "creating curve, VR=FL, points=" << idx/2 << endl;
        rawData = new Float32[idx];
        if (rawData==NULL) { CERR << "out of memory" << endl; return 1; }
        byteLength = sizeof(Float32)*idx;
        for (i=0; i<idx; i++) ((Float32 *)rawData)[i] = (Float32)(array[i]);
        align = sizeof(Float32);
        break;
      case 3: // FD
        if (opt_verbose) CERR << "creating curve, VR=FD, points=" << idx/2 << endl;
        rawData = new Float64[idx];
        if (rawData==NULL) { CERR << "out of memory" << endl; return 1; }
        byteLength = sizeof(Float64)*idx;
        for (i=0; i<idx; i++) ((Float64 *)rawData)[i] = (Float64)(array[i]);
        align = sizeof(Float64);
        break;
      case 4: // SL
        if (opt_verbose) CERR << "creating curve, VR=SL, points=" << idx/2 << endl;
        rawData = new Sint32[idx];
        if (rawData==NULL) { CERR << "out of memory" << endl; return 1; }
        byteLength = sizeof(Sint32)*idx;
        for (i=0; i<idx; i++) ((Sint32 *)rawData)[i] = (Sint32)(array[i]);
        align = sizeof(Sint32);
        break;
      default:
        CERR << "unknown data VR, bailing out" << endl;
        return 1;
    }
    
    DcmElement *element = NULL;
    switch (opt_curve_vr)
    {
      case 0: // explicit VR
        switch (opt_data_vr)
        {
          case 0: // US            
            if (opt_verbose) CERR << "encoding curve data element as VR=US" << endl;
            element = new DcmUnsignedShort(DcmTag(DCM_CurveData, EVR_US));
            if (element==NULL) { CERR << "out of memory" << endl; return 1; }
            element->putUint16Array((Uint16 *)rawData, byteLength/sizeof(Uint16));
            break;
          case 1: // SS
            if (opt_verbose) CERR << "encoding curve data element as VR=SS" << endl;
            element = new DcmSignedShort(DcmTag(DCM_CurveData, EVR_SS));
            if (element==NULL) { CERR << "out of memory" << endl; return 1; }
            element->putSint16Array((Sint16 *)rawData, byteLength/sizeof(Sint16));
            break;
          case 2: // FL
            if (opt_verbose) CERR << "encoding curve data element as VR=FL" << endl;
            element = new DcmFloatingPointSingle(DcmTag(DCM_CurveData, EVR_FL));
            if (element==NULL) { CERR << "out of memory" << endl; return 1; }
            element->putFloat32Array((Float32 *)rawData, byteLength/sizeof(Float32));
            break;
          case 3: // FD
            if (opt_verbose) CERR << "encoding curve data element as VR=FD" << endl;
            element = new DcmFloatingPointDouble(DcmTag(DCM_CurveData, EVR_FD));
            if (element==NULL) { CERR << "out of memory" << endl; return 1; }
            element->putFloat64Array((Float64 *)rawData, byteLength/sizeof(Float64));
            break;
          case 4: // SL
            if (opt_verbose) CERR << "encoding curve data element as VR=SL" << endl;
            element = new DcmSignedLong(DcmTag(DCM_CurveData, EVR_SL));
            if (element==NULL) { CERR << "out of memory" << endl; return 1; }
            element->putSint32Array((Sint32 *)rawData, byteLength/sizeof(Sint32));
            break;
          default:
            CERR << "unknown data VR, bailing out" << endl;
            return 1;
        }
        element->setGTag((Uint16)(0x5000+2*opt_group));
        dataset->insert(element, OFTrue);
        break;
      case 1: // OB
        // create little endian byte order
        if (opt_verbose) CERR << "encoding curve data element as VR=OB" << endl;
        swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, rawData, byteLength, align);
        element = new DcmOtherByteOtherWord(DCM_CurveData);
        if (element==NULL) { CERR << "out of memory" << endl; return 1; }
        element->setGTag((Uint16)(0x5000+2*opt_group));
        element->setVR(EVR_OB);
        element->putUint8Array((Uint8 *)rawData, byteLength);
        dataset->insert(element, OFTrue);
        break;
      case 2: // OW
        // create little endian byte order
        if (opt_verbose) CERR << "encoding curve data element as VR=OW" << endl;
        if (align != sizeof(Uint16))
        {
          swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, rawData, byteLength, align);
          swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, rawData, byteLength, sizeof(Uint16));
        }
        element = new DcmOtherByteOtherWord(DCM_CurveData);
        if (element==NULL) { CERR << "out of memory" << endl; return 1; }
        element->setGTag((Uint16)(0x5000+2*opt_group));
        element->setVR(EVR_OW);
        element->putUint16Array((Uint16 *)rawData, byteLength/sizeof(Uint16));
        dataset->insert(element, OFTrue);
        break;
      default:
        CERR << "unsupported VR, bailing out" << endl;
        return 1;
    }
    /* write back */
    
    DcmFileStream outf( opt_outName, DCM_WriteMode );
    if ( outf.Fail() ) {
	CERR << "cannot create file: " << opt_outName << endl;
	return 1;
    }
    
    fileformat->transferInit();
    error = fileformat->write(outf, dataset->getOriginalXfer());
    fileformat->transferEnd();

    if (error != EC_Normal) 
    {
	CERR << "Error: "  
	     << dcmErrorConditionToString(error)
	     << ": writing file: " <<  opt_outName << endl;
	return 1;
    }


    return 0;

}


/*
** CVS/RCS Log:
** $Log: dcmmkcrv.cc,v $
** Revision 1.4  2000-03-03 14:13:24  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.3  1999/05/03 14:16:36  joergr
** Minor code purifications to keep Sun CC 2.0.1 quiet.
**
** Revision 1.2  1999/04/28 15:45:05  meichel
** Cleaned up module dcmpstat apps, adapted to new command line class
**   and added short documentation.
**
** Revision 1.1  1998/12/22 17:57:02  meichel
** Implemented Presentation State interface for overlays,
**   VOI LUTs, VOI windows, curves. Added test program that
**   allows to add curve data to DICOM images.
**
*/
