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
 *  Purpose: This application reads a DICOM image, adds a Modality LUT or
 *    a VOI LUT to the image and writes it back. The LUT has a gamma curve shape.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-04-28 15:45:05 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/dcmmklut.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */
 

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include <math.h>
#include <stdio.h>
#include "dctk.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dcuid.h"    /* for dcmtk version name */

#define OFFIS_CONSOLE_APPLICATION "dcmmklut"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

OFBool opt_verbose = OFFalse;

// ********************************************

E_Condition createLUT16(
  double windowCenter,
  double windowWidth,
  Uint16 numberOfBits,
  double gammaValue,
  DcmUnsignedShort& lutDescriptor,
  DcmUnsignedShort& lutData,
  DcmLongString& lutExplanation)
{
	
  if (opt_verbose)
  {
    cerr << "creating 16 bit LUT with" << endl
         << "  window center  = " << windowCenter << endl
         << "  window width   = " << windowWidth << endl
         << "  number of bits = " << numberOfBits << endl
         << "  gamma          = " << gammaValue << endl;         
  }
  Uint16 descriptor_numEntries=0;
  Uint16 descriptor_firstMapped=0;
  
  if (windowWidth > 65535)
  {
    if (opt_verbose) cerr << "Error: cannot create LUT with more than 65535 entries" << endl;
    return EC_IllegalCall;
  }
  
  if ((numberOfBits<8)||(numberOfBits>16))
  {
    if (opt_verbose) cerr << "Error: cannot create LUT with " << numberOfBits << " bit entries, only 8..16" << endl;
    return EC_IllegalCall;
  }
  
  Sint32 firstValueMapped = (Sint32)(windowCenter - (windowWidth*0.5));
  if (firstValueMapped < 0)
  {
    if (firstValueMapped < -32768)
    {
      if (opt_verbose) cerr << "Error: cannot create LUT - first value mapped is out of range." << endl;
    return EC_IllegalCall;
    }
    Sint16 sfvMapped = (Sint16)firstValueMapped; // cut to 16 bits
    descriptor_firstMapped = (Uint16)sfvMapped;  // cast to unsigned
  } else {
    if (firstValueMapped > 0xFFFF)
    {
      if (opt_verbose) cerr << "Error: cannot create LUT - first value mapped is out of range." << endl;
      return EC_IllegalCall;
    }
    descriptor_firstMapped = (Uint16)firstValueMapped;
  }
  descriptor_numEntries = (Uint16)windowWidth;
  
  // now write LUT descriptor
  E_Condition result = EC_Normal;
  result = lutDescriptor.putUint16(descriptor_numEntries,0);
  if (result==EC_Normal) result = lutDescriptor.putUint16(descriptor_firstMapped,1);  
  if (result==EC_Normal) result = lutDescriptor.putUint16(numberOfBits,2);
  
  // create LUT
  Uint16 maxValue = 0xFFFF >> (16-numberOfBits);
  double step = (double)maxValue / (double)(descriptor_numEntries-1);
  double gammaExp = 1.0/gammaValue;
  double factor = (double)maxValue / pow(maxValue, gammaExp);
  double val;
  Uint16 uval;
  for (Uint16 i=0; i<descriptor_numEntries; i++)
  {
    val = factor * pow(i*step, gammaExp);
    uval = (Uint16)val;
    if (result==EC_Normal) result = lutData.putUint16(uval,i);
  }
  
  // create LUT explanation
  char buf[100];
  if (firstValueMapped < 0)
  {
    sprintf(buf, "LUT with gamma %3.1f, descriptor %u/%ld/%u", gammaValue, 
      descriptor_numEntries, firstValueMapped, numberOfBits );
  } else {
    sprintf(buf, "LUT with gamma %3.1f, descriptor %u/%u/%u", gammaValue, 
      descriptor_numEntries, descriptor_firstMapped,numberOfBits );
  }
  if (result==EC_Normal) result = lutExplanation.putString(buf);
  return result;
}

E_Condition createLUT8(
  double windowCenter,
  double windowWidth,
  double gammaValue,
  DcmUnsignedShort& lutDescriptor,
  DcmUnsignedShort& lutData,
  DcmLongString& lutExplanation)
{
  if (opt_verbose)
  {
    cerr << "creating 8 bit LUT with" << endl
         << "  window center  = " << windowCenter << endl
         << "  window width   = " << windowWidth << endl
         << "  number of bits = 8" << endl
         << "  gamma          = " << gammaValue << endl;         
  }

  Uint16 descriptor_numEntries=0;
  Uint16 descriptor_firstMapped=0;
  
  if (windowWidth > 65535)
  {
    if (opt_verbose) cerr << "Error: cannot create LUT with more than 65535 entries" << endl;
    return EC_IllegalCall;
  }
    
  Sint32 firstValueMapped = (Sint32)(windowCenter - (windowWidth*0.5));
  if (firstValueMapped < 0)
  {
    if (firstValueMapped < -32768)
    {
      if (opt_verbose) cerr << "Error: cannot create LUT - first value mapped is out of range." << endl;
      return EC_IllegalCall;
    }
    Sint16 sfvMapped = (Sint16)firstValueMapped; // cut to 16 bits
    descriptor_firstMapped = (Uint16)sfvMapped;  // cast to unsigned
  } else {
    if (firstValueMapped > 0xFFFF)
    {
      if (opt_verbose) cerr << "Error: cannot create LUT - first value mapped is out of range." << endl;
      return EC_IllegalCall;
    }
    descriptor_firstMapped = (Uint16)firstValueMapped;
  }
  descriptor_numEntries = (Uint16)windowWidth;
  
  // now write LUT descriptor
  E_Condition result = EC_Normal;
  result = lutDescriptor.putUint16(descriptor_numEntries,0);
  if (result==EC_Normal) result = lutDescriptor.putUint16(descriptor_firstMapped,1);  
  if (result==EC_Normal) result = lutDescriptor.putUint16(8,2);
  
  // create LUT
  double step = 255.0 / (double)(descriptor_numEntries-1);
  double gammaExp = 1.0/gammaValue;
  double factor = 255.0 / pow(255.0, gammaExp);
  double val;
  Uint8 *array= new Uint8[descriptor_numEntries+2];

  if (!array)
  {
    if (opt_verbose) cerr << "Error: cannot allocate temporary storage for LUT." << endl;
    return EC_IllegalCall;
  }
  for (Uint16 i=0; i<descriptor_numEntries; i++)
  {
    val = factor * pow(i*step, gammaExp);
    array[i] = (Uint8)val;
  }
  array[descriptor_numEntries]=0; //create padding value
  // the array is now in little endian byte order. Swap to local byte order if neccessary.
  if (result==EC_Normal) result = swapIfNecessary(gLocalByteOrder, 
    EBO_LittleEndian, array, descriptor_numEntries+1, sizeof(Uint16));
  
  if (result==EC_Normal) result = lutData.putUint16Array((Uint16 *)array,(descriptor_numEntries+1)/2);
  
  // create LUT explanation
  char buf[100];
  if (firstValueMapped < 0)
  {
    sprintf(buf, "LUT with gamma %3.1f, descriptor %u/%ld/%u, 8bit", gammaValue, 
      descriptor_numEntries, firstValueMapped,8);
  } else {
    sprintf(buf, "LUT with gamma %3.1f, descriptor %u/%u/%u, 8bit", gammaValue, 
      descriptor_numEntries, descriptor_firstMapped,8);
  }
  if (result==EC_Normal) result = lutExplanation.putString(buf);
  return result;
}

// ********************************************

#define SHORTCOL 3
#define LONGCOL 12

int main(int argc, char *argv[])
{
    OFString str;   
    const char *opt_inName = NULL;                     /* in file name */
    const char *opt_outName = NULL;                    /* out file name */
    OFCmdFloat gammaValue=1.0;
    OFCmdFloat windowCenter=0.0;
    OFCmdFloat windowWidth=0.0;
    OFCmdUnsignedInt bits=16;
    OFBool byteAlign = OFFalse;
    OFBool replaceMode = OFTrue;
    OFBool createMLUT = OFFalse;
        
    SetDebugLevel(( 0 ));

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Add modality or VOI LUT to image", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
  
    cmd.addParam("window-center",  "window center equivalent of LUT to be created");
    cmd.addParam("window-width",   "window width equivalent of LUT to be created");
    cmd.addParam("dcmimg-in",      "DICOM input image file");
    cmd.addParam("dcmimg-out",     "DICOM output filename");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                      "-h",        "print this help text and exit");
     cmd.addOption("--verbose",                   "-v",        "verbose mode, print processing details");
     cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");
    cmd.addGroup("LUT creation options:");
      cmd.addSubGroup("LUT type:");
       cmd.addOption("--modality",    "+Tm",    "create as Modality LUT");
       cmd.addOption("--voi",         "+Tv",    "create as VOI LUT (default)");
      cmd.addSubGroup("LUT placement:");
       cmd.addOption("--add",         "+Pa",    "add to existing transform\n(default for +Tv, only with +Tv)");
       cmd.addOption("--replace",     "+Pr",    "replace existing transform (default for +Tm)");
      cmd.addSubGroup("LUT content:");
       cmd.addOption("--gamma",       "-g", 1, "gamma: float",
                                               "use gamma value (default: 1.0)");
       cmd.addOption("--bits",        "-b", 1, "[n]umber : integer",
                                               "create LUT with n bit values (8..16, default: 16)");
       cmd.addOption("--byte-align",  "-a",    "create byte-aligned LUT (implies -b 8)");

    /* evaluate command line */                           
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      app.checkParam(cmd.getParam(1, windowCenter));
      app.checkParam(cmd.getParam(2, windowWidth,0.0));
      cmd.getParam(3, opt_inName);
      cmd.getParam(4, opt_outName);

      if (cmd.findOption("--verbose")) opt_verbose=OFTrue;
      if (cmd.findOption("--debug")) SetDebugLevel(3);
      cmd.beginOptionBlock();
      if (cmd.findOption("--modality")) createMLUT = OFTrue;
      if (cmd.findOption("--voi")) createMLUT = OFFalse;                
      cmd.endOptionBlock();
      if (createMLUT) replaceMode=OFTrue; else replaceMode=OFFalse;                

      cmd.beginOptionBlock();
      if (cmd.findOption("--add"))
      {
      	app.checkConflict("--add","--modality", createMLUT);
        replaceMode = OFFalse;
      }
      if (cmd.findOption("--replace")) replaceMode = OFTrue;
      cmd.endOptionBlock();

      if (cmd.findOption("--gamma")) app.checkValue(cmd.getValue(gammaValue));
      if (cmd.findOption("--bits")) app.checkValue(cmd.getValue(bits,8,16));
      if (cmd.findOption("--byte-align")) byteAlign = OFTrue;
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded()) {
	cerr << "Warning: no data dictionary loaded, "
	     << "check environment variable: "
	     << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
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
    
    /* create LUT */
    DcmDataset *dataset = fileformat -> getDataset();

    DcmUnsignedShort lutDescriptor(DCM_LUTDescriptor);
    DcmUnsignedShort lutData(DCM_LUTData);
    DcmLongString lutExplanation(DCM_LUTExplanation);
    DcmLongString modalityLUTType(DCM_ModalityLUTType);
    modalityLUTType.putString("US"); // unspecified Modality LUT
    
    E_Condition result = EC_Normal;
    if (byteAlign)
    {
      result = createLUT8(windowCenter, windowWidth, gammaValue,
        lutDescriptor, lutData, lutExplanation);
    } else {
      result = createLUT16(windowCenter, windowWidth, bits, gammaValue,
        lutDescriptor, lutData, lutExplanation);
    }
    if (EC_Normal != result) 
    {
      cerr << "could not create LUT, bailing out." << endl;
    }
    
    /* create Item with LUT */
    DcmItem *ditem = new DcmItem();
    if (ditem)
    {
    	DcmElement *delem;
        delem = new DcmUnsignedShort(lutDescriptor);
        if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
        delem = new DcmUnsignedShort(lutData);
        if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
        delem = new DcmLongString(lutExplanation);
        if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
        if (createMLUT)
        {
          delem = new DcmLongString(modalityLUTType);
          if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
        }
    } else result = EC_MemoryExhausted;

    DcmSequenceOfItems *dseq = NULL;
    if (EC_Normal==result)
    {
      if (createMLUT)
      {
        dseq = new DcmSequenceOfItems(DCM_ModalityLUTSequence);
        if (dseq)
        {
          dataset->insert(dseq, OFTrue);
          dseq->insert(ditem);
        } else result = EC_MemoryExhausted;
        delete dataset->remove(DCM_RescaleIntercept);
        delete dataset->remove(DCM_RescaleSlope);
        delete dataset->remove(DCM_RescaleType);
      } else {
        if (!replaceMode)
        {
          // search existing sequence
          DcmStack stack;
          if (EC_Normal == dataset->search(DCM_VOILUTSequence, stack, ESM_fromHere, OFFalse)) 
            dseq=(DcmSequenceOfItems *)stack.top();
        }
        if (dseq==NULL)
        {
          dseq = new DcmSequenceOfItems(DCM_VOILUTSequence);
          if (dseq) dataset->insert(dseq, OFTrue); else result = EC_MemoryExhausted;
        }
        if (dseq) dseq->insert(ditem);
        if (replaceMode)
        {
          delete dataset->remove(DCM_WindowCenter);
          delete dataset->remove(DCM_WindowWidth);
          delete dataset->remove(DCM_WindowCenterWidthExplanation);
        }
      }
    }
    
    if (result != EC_Normal)
    {
      cerr << "error while adding LUT to image dataset. Bailing out."<< endl;
      return 1;
    }
    
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
** $Log: dcmmklut.cc,v $
** Revision 1.2  1999-04-28 15:45:05  meichel
** Cleaned up module dcmpstat apps, adapted to new command line class
**   and added short documentation.
**
** Revision 1.1  1998/12/14 16:05:48  meichel
** Added sample application that creates Modality and VOI LUTs
**   with gamma curve characteristics.
**
** Revision 1.1  1998/11/27 14:50:19  meichel
** Initial Release.
**
**
**
*/
