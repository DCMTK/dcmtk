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
 *  Update Date:      $Date: 1999-07-28 11:21:07 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/dcmmklut.cc,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
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

E_Condition createLUT(
  double windowCenter,
  double windowWidth,
  Uint16 numberOfBits,
  double gammaValue,
  OFBool byteAlign,
  DcmEVR lutVR,
  DcmItem& item)
{	
  E_Condition result = EC_Normal;

  // create LUT descriptor
  unsigned long descriptor_numEntries = (unsigned long) windowWidth;
  long          descriptor_firstMapped = (long)(windowCenter - (windowWidth*0.5));
  Uint16        descriptor_numBits = numberOfBits;
  Uint16        descriptor_numEntries16 = 0;
  
  if (opt_verbose)
  {
    if (byteAlign) cerr << "creating 8 bit LUT with" << endl;
    else           cerr << "creating 16 bit LUT with" << endl;
    cerr << "  window center  = " << windowCenter << endl
         << "  window width   = " << windowWidth << endl
         << "  number of bits = " << numberOfBits << endl
         << "  gamma          = " << gammaValue << endl
         << "  descriptor VR  = ";
    if (descriptor_firstMapped < 0) cerr << "SS" << endl; else cerr << "US" << endl;
    cerr << "  lut data VR    = ";
    switch (lutVR)
    {
      case EVR_US:
        cerr << "US" << endl;
        break;
      case EVR_OW:
        cerr << "OW" << endl;
        break;
      case EVR_SS:
        cerr << "SS" << endl;
        break;
      default:
        cerr << "unknown" << endl;
        break;
    }    
  }
  if (descriptor_numEntries==0) cerr << "Warning: creating LUT without LUT data" << endl;
  
  if (descriptor_numEntries > 65536)
  {
    cerr << "Error: cannot create LUT with more than 65536 entries" << endl;
    return EC_IllegalCall;
  }
  if (descriptor_numEntries < 65536) descriptor_numEntries16 = (Uint16)descriptor_numEntries;
  
  if ((descriptor_numBits<8)||(descriptor_numBits>16))
  {
    cerr << "Error: cannot create LUT with " << numberOfBits << " bit entries, only 8..16" << endl;
    return EC_IllegalCall;
  }
  
  DcmElement *descriptor = NULL;
  if (descriptor_firstMapped < 0)
  {
    // LUT Descriptor is SS
    if (descriptor_firstMapped < -32768)
    {
      cerr << "Error: cannot create LUT - first value mapped < -32768" << endl;
      return EC_IllegalCall;
    }
    descriptor = new DcmSignedShort(DcmTag(DCM_LUTDescriptor, EVR_SS));
    if (descriptor)
    {  
      if (EC_Normal==result) result = descriptor->putSint16((Sint16)descriptor_numEntries16,0);
      if (EC_Normal==result) result = descriptor->putSint16((Sint16)descriptor_firstMapped,1);
      if (EC_Normal==result) result = descriptor->putSint16((Sint16)descriptor_numBits,2);
      if (EC_Normal==result) result = item.insert(descriptor);
    } else return EC_MemoryExhausted;
  } else {
    // LUT Descriptor is US
    if (descriptor_firstMapped > 0xFFFF)
    {
      cerr << "Error: cannot create LUT - first value mapped > 65535" << endl;
      return EC_IllegalCall;
    }
    descriptor = new DcmUnsignedShort(DcmTag(DCM_LUTDescriptor, EVR_US));
    if (descriptor)
    {
      if (EC_Normal==result) result = descriptor->putUint16(descriptor_numEntries16,0);
      if (EC_Normal==result) result = descriptor->putUint16((Uint16)descriptor_firstMapped,1);
      if (EC_Normal==result) result = descriptor->putUint16(descriptor_numBits,2);
      if (EC_Normal==result) result = item.insert(descriptor);
    } else return EC_MemoryExhausted;
  }

  // create LUT
  Uint16 maxValue = 0xFFFF >> (16-numberOfBits);
  double step = (double)maxValue / ((double)descriptor_numEntries-1.0);
  double gammaExp = 1.0/gammaValue;
  double factor = (double)maxValue / pow(maxValue, gammaExp);
  double val;
  unsigned long i = 0;
  unsigned long wordsToWrite = 0;
  Uint16 *array = new Uint16[65536]; // max size for LUT
  if (array==NULL) return EC_MemoryExhausted;

  if (byteAlign)
  {
    Uint8 *array8 = (Uint8 *)array;
    for (i=0; i<descriptor_numEntries; i++)
    {
      val = factor * pow(i*step, gammaExp);
      array8[i] = (Uint8)val;
    }
    array8[descriptor_numEntries]=0; //create padding value
    
    // the array is now in little endian byte order. Swap to local byte order if neccessary.
    if (result==EC_Normal) result = swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, array8, descriptor_numEntries+1, sizeof(Uint16));
    wordsToWrite = (descriptor_numEntries+1)/2;
  } else {
    for (i=0; i<descriptor_numEntries; i++)
    {
      val = factor * pow(i*step, gammaExp);
      array[i]= (Uint16)val;
    }
    wordsToWrite = descriptor_numEntries;
  }

  if ((wordsToWrite > 32767)&&(lutVR != EVR_OW))
  {
    cerr << "Warning: LUT data >= 64K, writing as OW" << endl;
    lutVR = EVR_OW;
  }

  // write LUT Data as OW, US, or SS
  DcmElement *lutdata = NULL;
  switch (lutVR)
  {
    case EVR_US:
      lutdata = new DcmUnsignedShort(DcmTag(DCM_LUTData, EVR_US));
      if (lutdata)
      {
        if (EC_Normal==result) result = lutdata->putUint16Array(array, wordsToWrite);
        if (EC_Normal==result) result = item.insert(lutdata);
      } else return EC_MemoryExhausted;
      break;
    case EVR_OW:
      lutdata = new DcmOtherByteOtherWord(DcmTag(DCM_LUTData, EVR_OW));
      if (lutdata)
      {
        if (EC_Normal==result) result = lutdata->putUint16Array(array, wordsToWrite);
        if (EC_Normal==result) result = item.insert(lutdata);
      } else return EC_MemoryExhausted;
      break;
    case EVR_SS:
      lutdata = new DcmSignedShort(DcmTag(DCM_LUTData, EVR_SS));
      if (lutdata)
      {
        if (EC_Normal==result) result = lutdata->putSint16Array((Sint16 *)array, wordsToWrite);
        if (EC_Normal==result) result = item.insert(lutdata);
      } else return EC_MemoryExhausted;
      break;
    default:
      cerr << "Error: unsupported VR for LUT Data" << endl;
      return EC_IllegalCall;
      break;
  }
  delete[] array;
  
  // create LUT explanation
  DcmElement *explanation = new DcmLongString(DCM_LUTExplanation);
  if (explanation)
  {
    char buf[100];
    if (descriptor_firstMapped < 0)
    {
      sprintf(buf, "LUT with gamma %3.1f, descriptor %u/%ld/%u", gammaValue, 
        descriptor_numEntries16, descriptor_firstMapped, numberOfBits);
    } else {
      sprintf(buf, "LUT with gamma %3.1f, descriptor %u/%ld/%u", gammaValue, 
        descriptor_numEntries16, descriptor_firstMapped, numberOfBits);
    }
    if (result==EC_Normal) result = explanation->putString(buf);
    if (EC_Normal==result) result = item.insert(explanation);
  } else return EC_MemoryExhausted;
  
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
    DcmEVR lutVR = EVR_OW;
            
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
      cmd.addSubGroup("LUT data VR:");
       cmd.addOption("--data-ow",     "+Dw",    "write LUT Data as OW (default)");
       cmd.addOption("--data-us",     "+Du",    "write LUT Data as US");
       cmd.addOption("--data-ss",     "+Ds",    "write LUT Data as SS");

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
      if (cmd.findOption("--byte-align"))
      {
      	 byteAlign = OFTrue;
      	 bits = 8;
      }
      if (cmd.findOption("--bits")) app.checkValue(cmd.getValue(bits,8,16));

      cmd.beginOptionBlock();
      if (cmd.findOption("--data-ow")) lutVR = EVR_OW;
      if (cmd.findOption("--data-us")) lutVR = EVR_US;
      if (cmd.findOption("--data-ss")) lutVR = EVR_SS;
      cmd.endOptionBlock();
    }

    if (createMLUT && (bits != 8) && (bits != 16))
    {
	cerr << "error: --modality cannot be used with --bits other than 8 or 16" << endl;
	return 1;
    }
    if ((bits != 8) && byteAlign)
    {
	cerr << "error: --byte-align cannot be used with --bits other than 8" << endl;
	return 1;
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

    DcmLongString modalityLUTType(DCM_ModalityLUTType);
    modalityLUTType.putString("US"); // unspecified Modality LUT
    
    E_Condition result = EC_Normal;

    /* create Item with LUT */
    DcmItem *ditem = new DcmItem();
    if (ditem)
    {
      if (byteAlign)
      {
        result = createLUT(windowCenter, windowWidth, 8, gammaValue, OFTrue, lutVR, *ditem);
      } else {
        result = createLUT(windowCenter, windowWidth, (Uint16)bits, gammaValue, OFFalse, lutVR, *ditem);
      }
      if (EC_Normal != result) 
      {
        cerr << "could not create LUT, bailing out." << endl;
        return 1;
      }
      if (createMLUT)
      {
        DcmElement *delem = new DcmLongString(modalityLUTType);
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
** Revision 1.5  1999-07-28 11:21:07  meichel
** New options in dcmmklut:
** - creation of LUTs with 65536 entries
** - creation of LUT data with VR=OW, US or SS
** - creation of LUT descriptor with VR=US or SS
**
** Revision 1.4  1999/05/04 15:27:22  meichel
** Minor code purifications to keep gcc on OSF1 quiet.
**
** Revision 1.3  1999/05/03 14:16:37  joergr
** Minor code purifications to keep Sun CC 2.0.1 quiet.
**
** Revision 1.2  1999/04/28 15:45:05  meichel
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
*/
