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
 *  Update Date:      $Date: 1998-12-22 17:57:02 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/dcmmkcrv.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
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
#include "cmdlnarg.h"
#include "ofcmdln.h"
#include "dctk.h"


#define MAX_POINTS 65535

// ********************************************


static void
printHeader()
{
    cerr << "dcmmkcrv: Add Curve to DICOM Image" << endl;
}


static void
printUsage(const OFCommandLine &cmd)
{
    OFString str;
    cmd.getOptionString(str);
    printHeader();
    cerr << "usage: dcmmklut [options] dcmimg-in curvedata-in dcmimg-out" << endl;
    cerr << "options are:" << endl << endl;
    cerr << str << endl;
    exit(0);
}

static void
printError(const OFString &str)
{
    printHeader();
    cerr << "error: " << str << endl;
    exit(1);
}

static void
checkValue(OFCommandLine &cmd,
           const OFCommandLine::E_ValueStatus status)
{
    OFString str;
    if (status != OFCommandLine::VS_Normal)
    {
        cmd.getStatusString(status, str);
        printError(str);
    }
}

// ********************************************

int main(int argc, char *argv[])
{
    OFCommandLine cmd;
    OFString str;
    
    const char *opt_inName = NULL;                     /* in file name */
    const char *opt_outName = NULL;                    /* out file name */
    const char *opt_curveName = NULL;                  /* curve file name */
    long opt_data_vr = 4;
    long opt_group   = 0;
    OFBool opt_poly = OFTrue;
    const char *opt_label = NULL;
    const char *opt_description = NULL;
    const char *opt_axis_x = NULL;
    const char *opt_axis_y = NULL;
    long opt_curve_vr = 0;
    
    SetDebugLevel(( 0 ));
  
    prepareCmdLineArgs(argc, argv, "dcmmklut");
      
    cmd.addGroup("options:");
     cmd.addOption("--help", "print this help screen");

    cmd.addGroup("Curve options:");
     cmd.addOption("--data-vr",     "-v", 1,  "[n]umber : integer 0..4",
                                              "select curve data VR: 0=US, 1=SS, 2=FL, 3=FD, 4=SL (default)" );
     cmd.addOption("--group",       "-g", 1,  "[n]umber : integer 0..15",
                                              "select repeating group: 0=0x5000, 1=0x5002 etc.");
     cmd.addOption("--roi",         "+r",     "create as ROI curve");
     cmd.addOption("--poly",        "-r",     "create as POLY curve (default)");                                         
     cmd.addOption("--label",       "-l", 1,  "s: string",
                                              "set Curve Label to s (default: absent)");
     cmd.addOption("--description", "-d", 1,  "s: string",
                                              "set Curve Description to s (default: absent)");
     cmd.addOption("--axis",        "-a", 2,  "x: string, y: string",
                                              "set Axis Units to x\\y (default: absent");
     cmd.addOption("--curve-vr",    "-c", 1,  "[n]umber: integer 0..2",
                                              "select VR with which the Curve Data element is written\n"
                                              "0=VR according to --data-vr (default), 1=OB, 2=OW");                                                                                                 
     
    switch (cmd.parseLine(argc, argv))    
    {
        case OFCommandLine::PS_NoArguments:
            printUsage(cmd);
            break;
        case OFCommandLine::PS_UnknownOption:
            cmd.getStatusString(OFCommandLine::PS_UnknownOption, str);
            printError(str);
            break;
        case OFCommandLine::PS_MissingValue:
            cmd.getStatusString(OFCommandLine::PS_MissingValue, str);
            printError(str);
            break;
        case OFCommandLine::PS_Normal:
            if ((cmd.getArgCount() == 1) && cmd.findOption("--help"))
                printUsage(cmd);
            else if (cmd.getParamCount() < 3)
                printError("Missing filename");
            else if (cmd.getParamCount() > 3)
                printError("Too many filenames");
            else 
            {
                cmd.getParam(1, opt_inName);
                cmd.getParam(2, opt_curveName);
                cmd.getParam(3, opt_outName);

                if (cmd.findOption("--data-vr"))
                    checkValue(cmd, cmd.getValue(opt_data_vr,0,4));
                if (cmd.findOption("--group"))
                    checkValue(cmd, cmd.getValue(opt_group,0,15));
                if (cmd.findOption("--roi")) opt_poly = OFFalse;
                if (cmd.findOption("--poly")) opt_poly = OFTrue;                
                if (cmd.findOption("--label"))
                    checkValue(cmd, cmd.getValue(opt_label));
                if (cmd.findOption("--description"))
                    checkValue(cmd, cmd.getValue(opt_description));
                if (cmd.findOption("--axis"))
                {
                    checkValue(cmd, cmd.getValue(opt_axis_x));
                    checkValue(cmd, cmd.getValue(opt_axis_y));
                }
                if (cmd.findOption("--curve-vr"))
                    checkValue(cmd, cmd.getValue(opt_curve_vr,0,2));
            }
    }

    DcmFileStream inf(opt_inName, DCM_ReadMode);
    if ( inf.Fail() ) {
	cerr << "cannot open file: " << opt_inName << endl;
        return 1;
    }

    DcmFileFormat *fileformat = new DcmFileFormat;
    E_Condition error = EC_Normal;
    if (!fileformat)
    {
      cerr << "memory exhausted\n";
      return 1;
    }

    fileformat->transferInit();
    error = fileformat -> read(inf);
    fileformat->transferEnd();

    if (error != EC_Normal) 
    {
	cerr << "Error: "  
	     << dcmErrorConditionToString(error)
	     << ": reading file: " <<  opt_inName << endl;
	return 1;
    }
    DcmDataset *dataset = fileformat->getDataset();
    
    /* read curve data */    

    ifstream curvefile(opt_curveName);
    if (!curvefile)
    {
      cerr << "cannot open curve file: " << opt_curveName << endl;
      return 1;
    }
    
    double *array = new double[MAX_POINTS*2];
    if (array==NULL)
    {
      cerr << "out of memory" << endl;
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
      	cerr << "warning: too many curve points, can only handle " << MAX_POINTS << "." << endl;
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
      cerr << "out of memory" << endl;
      return 1;
    }
    
    curveDimensions->setGTag(0x5000+2*opt_group);
    curveDimensions->putUint16(2,0);
    dataset->insert(curveDimensions, OFTrue);

    numberOfPoints->setGTag(0x5000+2*opt_group);
    numberOfPoints->putUint16((Uint16)(idx/2),0);
    dataset->insert(numberOfPoints, OFTrue);
    
    typeOfData->setGTag(0x5000+2*opt_group);
    if (opt_poly) typeOfData->putString("POLY"); else typeOfData->putString("ROI");
    dataset->insert(typeOfData, OFTrue);

    dataValueRepresentation->setGTag(0x5000+2*opt_group);
    dataValueRepresentation->putUint16((Uint16)opt_data_vr,0);
    dataset->insert(dataValueRepresentation, OFTrue);
    
    if (opt_description)
    {
      curveDescription->setGTag(0x5000+2*opt_group);
      curveDescription->putString(opt_description);
      dataset->insert(curveDescription, OFTrue);  
    }

    if (opt_label)
    {
      curveLabel->setGTag(0x5000+2*opt_group);
      curveLabel->putString(opt_label);
      dataset->insert(curveLabel, OFTrue);  
    }
    
    if (opt_axis_x && opt_axis_y)
    {
      OFString aString(opt_axis_x);
      aString += "\\";
      aString += opt_axis_y;
      axisUnits->setGTag(0x5000+2*opt_group);
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
        rawData = new Uint16[idx];
        if (rawData==NULL) { cerr << "out of memory" << endl; return 1; }
        byteLength = sizeof(Uint16)*idx;
        for (i=0; i<idx; i++) ((Uint16 *)rawData)[i] = (Uint16)(array[i]);
        align = sizeof(Uint16);
        break;
      case 1: // SS
        rawData = new Sint16[idx];
        if (rawData==NULL) { cerr << "out of memory" << endl; return 1; }
        byteLength = sizeof(Sint16)*idx;
        for (i=0; i<idx; i++) ((Sint16 *)rawData)[i] = (Sint16)(array[i]);
        align = sizeof(Sint16);
        break;
      case 2: // FL
        rawData = new Float32[idx];
        if (rawData==NULL) { cerr << "out of memory" << endl; return 1; }
        byteLength = sizeof(Float32)*idx;
        for (i=0; i<idx; i++) ((Float32 *)rawData)[i] = (Float32)(array[i]);
        align = sizeof(Float32);
        break;
      case 3: // FD
        rawData = new Float64[idx];
        if (rawData==NULL) { cerr << "out of memory" << endl; return 1; }
        byteLength = sizeof(Float64)*idx;
        for (i=0; i<idx; i++) ((Float64 *)rawData)[i] = (Float64)(array[i]);
        align = sizeof(Float64);
        break;
      case 4: // SL
        rawData = new Sint32[idx];
        if (rawData==NULL) { cerr << "out of memory" << endl; return 1; }
        byteLength = sizeof(Sint32)*idx;
        for (i=0; i<idx; i++) ((Sint32 *)rawData)[i] = (Sint32)(array[i]);
        align = sizeof(Sint32);
        break;
      default:
        cerr << "unknown data VR, bailing out" << endl;
        return 1;
    }
    
    DcmElement *element = NULL;
    switch (opt_curve_vr)
    {
      case 0: // explicit VR
        switch (opt_data_vr)
        {
          case 0: // US            
            element = new DcmUnsignedShort(DcmTag(DCM_CurveData, EVR_US));
            if (element==NULL) { cerr << "out of memory" << endl; return 1; }
            element->putUint16Array((Uint16 *)rawData, byteLength/sizeof(Uint16));
            break;
          case 1: // SS
            element = new DcmSignedShort(DcmTag(DCM_CurveData, EVR_SS));
            if (element==NULL) { cerr << "out of memory" << endl; return 1; }
            element->putSint16Array((Sint16 *)rawData, byteLength/sizeof(Sint16));
            break;
          case 2: // FL
            element = new DcmFloatingPointSingle(DcmTag(DCM_CurveData, EVR_FL));
            if (element==NULL) { cerr << "out of memory" << endl; return 1; }
            element->putFloat32Array((Float32 *)rawData, byteLength/sizeof(Float32));
            break;
          case 3: // FD
            element = new DcmFloatingPointDouble(DcmTag(DCM_CurveData, EVR_FD));
            if (element==NULL) { cerr << "out of memory" << endl; return 1; }
            element->putFloat64Array((Float64 *)rawData, byteLength/sizeof(Float64));
            break;
          case 4: // SL
            element = new DcmSignedLong(DcmTag(DCM_CurveData, EVR_SL));
            if (element==NULL) { cerr << "out of memory" << endl; return 1; }
            element->putSint32Array((Sint32 *)rawData, byteLength/sizeof(Sint32));
            break;
          default:
            cerr << "unknown data VR, bailing out" << endl;
            return 1;
        }
        element->setGTag(0x5000+2*opt_group);
        dataset->insert(element, OFTrue);
        break;
      case 1: // OB
        // create little endian byte order
        swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, rawData, byteLength, align);
        element = new DcmOtherByteOtherWord(DCM_CurveData);
        if (element==NULL) { cerr << "out of memory" << endl; return 1; }
        element->setGTag(0x5000+2*opt_group);
        element->setVR(EVR_OB);
        element->putUint8Array((Uint8 *)rawData, byteLength);
        dataset->insert(element, OFTrue);
        break;
      case 2: // OW
        // create little endian byte order
        if (align != sizeof(Uint16))
        {
          swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, rawData, byteLength, align);
          swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, rawData, byteLength, sizeof(Uint16));
        }
        element = new DcmOtherByteOtherWord(DCM_CurveData);
        if (element==NULL) { cerr << "out of memory" << endl; return 1; }
        element->setGTag(0x5000+2*opt_group);
        element->setVR(EVR_OW);
        element->putUint16Array((Uint16 *)rawData, byteLength/sizeof(Uint16));
        dataset->insert(element, OFTrue);
        break;
      default:
        cerr << "unsupported VR, bailing out" << endl;
        return 1;
    }
    /* write back */
    
    DcmFileStream outf( opt_outName, DCM_WriteMode );
    if ( outf.Fail() ) {
	cerr << "cannot create file: " << opt_outName << endl;
	return 1;
    }
    
    fileformat->transferInit();
    error = fileformat->write(outf, dataset->getOriginalXfer());
    fileformat->transferEnd();

    if (error != EC_Normal) 
    {
	cerr << "Error: "  
	     << dcmErrorConditionToString(error)
	     << ": writing file: " <<  opt_outName << endl;
	return 1;
    }


    return 0;

}


/*
** CVS/RCS Log:
** $Log: dcmmkcrv.cc,v $
** Revision 1.1  1998-12-22 17:57:02  meichel
** Implemented Presentation State interface for overlays,
**   VOI LUTs, VOI windows, curves. Added test program that
**   allows to add curve data to DICOM images.
**
**
**
*/
