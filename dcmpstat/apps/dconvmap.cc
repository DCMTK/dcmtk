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
 *  Purpose: This application reads a 264 byte MAP file and
 *    converts it into a Presentation LUT Sequence that is written to file.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-10-01 14:53:58 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/Attic/dconvmap.cc,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
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
#include "displint.h" /* for cubic spline interpolation */

#define OFFIS_CONSOLE_APPLICATION "dconvmap"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

static OFBool opt_verbose = OFFalse;


// ********************************************


E_Condition interpolateValues(
  const unsigned short *srcData,
  unsigned long srcCount,
  unsigned short *destData,
  unsigned long destCount)
{
  E_Condition result = EC_IllegalCall;
  if ((srcData != NULL) && (srcCount > 0) && (destCount > 0))
  {
    if (destData != NULL)
    {
      unsigned short *srcIndex = new unsigned short[srcCount];
      if (srcIndex != NULL)
      {
        register unsigned long i;
        register double gradient = ((srcCount < destCount) && (srcCount > 1)) ? (double)(destCount - 1) / (double)(srcCount - 1) : 1;
        for (i = 0; i < srcCount - 1; i++)
          srcIndex[i] = (unsigned short)((double)i * gradient);
        srcIndex[i] = (srcCount < destCount) ? (unsigned short)(destCount - 1) : (unsigned short)(srcCount - 1);
        double *spline = new double[srcCount];
        if (spline != NULL)
        {
          if (DiCubicSpline<unsigned short, unsigned short>::Function(srcIndex, srcData, srcCount, spline))
          {
            unsigned short *destIndex = new unsigned short[destCount];
            if (destIndex != NULL)
            {
              gradient = ((destCount < srcCount) && (destCount > 1)) ? (double)(srcCount - 1) / (double)(destCount - 1) : 1;
              for (i = 0; i < destCount - 1; i++)
                destIndex[i] = (unsigned short)((double)i * gradient);
              destIndex[i] = (destCount < srcCount) ? (unsigned short)(srcCount - 1) : (unsigned short)(destCount - 1);
              if (DiCubicSpline<unsigned short, unsigned short>::Interpolation(srcIndex, srcData, spline, srcCount, destIndex, destData, destCount))
                result = EC_Normal;
            } else result = EC_MemoryExhausted;
            delete[] destIndex;
          }
        } else result = EC_MemoryExhausted;
        delete[] spline;
      } else result = EC_MemoryExhausted;
      delete[] srcIndex;
    } else result = EC_MemoryExhausted;
  }
  return result;
}


E_Condition createLUT(
  unsigned char *map,
  Uint16 numberOfBits,
  unsigned long numberOfEntries,
  DcmEVR lutVR,
  DcmItem& item)
{
  if (map==NULL) return EC_IllegalCall;
  if (numberOfBits < 10) return EC_IllegalCall;
  if (numberOfBits > 16) return EC_IllegalCall;
  if (numberOfEntries < 1) return EC_IllegalCall;
  if (numberOfEntries > 65536) return EC_IllegalCall;

  Uint16 entries = (numberOfEntries == 65536) ? 0 : (Uint16)numberOfEntries;

  E_Condition result = EC_Normal;
  DcmElement *descriptor = new DcmUnsignedShort(DcmTag(DCM_LUTDescriptor, EVR_US));
  if (descriptor)
  {
    if (EC_Normal==result) result = descriptor->putUint16(entries,0);
    if (EC_Normal==result) result = descriptor->putUint16(0,1);
    if (EC_Normal==result) result = descriptor->putUint16(numberOfBits,2);
    if (EC_Normal==result) result = item.insert(descriptor);
  } else result = EC_MemoryExhausted;

  if (result == EC_Normal)
  {
    unsigned short *mapData = new unsigned short[256];
    unsigned short *lutData = mapData;
    if (mapData != NULL)
    {
      int shift = 16-numberOfBits;
      unsigned char c;
      for (int i=0; i<256; i++)
      {
        c = *map++;
        mapData[i] = (c | (c<<8)) >> shift;
      }
      if (numberOfEntries != 256)
      {
        lutData = new unsigned short[numberOfEntries];
        result = interpolateValues(mapData, 256, lutData, numberOfEntries);
      }
    } else result = EC_MemoryExhausted;

    if (result == EC_Normal)
    {
      // write LUT Data as OW, US, or SS
      DcmElement *lutdata = NULL;
      switch (lutVR)
      {
        case EVR_US:
          lutdata = new DcmUnsignedShort(DcmTag(DCM_LUTData, EVR_US));
          if (lutdata)
          {
            if (EC_Normal==result) result = lutdata->putUint16Array(lutData, numberOfEntries);
            if (EC_Normal==result) result = item.insert(lutdata);
          } else result = EC_MemoryExhausted;
          break;
        case EVR_OW:
          lutdata = new DcmOtherByteOtherWord(DcmTag(DCM_LUTData, EVR_OW));
          if (lutdata)
          {
            if (EC_Normal==result) result = lutdata->putUint16Array(lutData, numberOfEntries);
            if (EC_Normal==result) result = item.insert(lutdata);
          } else result = EC_MemoryExhausted;
          break;
        default:
          cerr << "Error: unsupported VR for LUT Data" << endl;
          result = EC_IllegalCall;
          break;
      }

      if (result == EC_Normal)
      {
        DcmElement *explanation = new DcmLongString(DCM_LUTExplanation);
        if (explanation)
        {
          char buf[100];
          sprintf(buf, "Presentation LUT with descriptor %lu/0/%u", numberOfEntries, numberOfBits);
          if (result==EC_Normal) result = explanation->putString(buf);
          if (EC_Normal==result) result = item.insert(explanation);
        } else result = EC_MemoryExhausted;
      }
    }

    if (lutData != mapData) delete[] lutData;
    delete[] mapData;
  }

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
    OFCmdUnsignedInt bits=12;
    OFCmdUnsignedInt entries=256;
    DcmEVR lutVR = EVR_OW;

    SetDebugLevel(( 0 ));

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Convert MAP file to Presentation LUT", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("mapfile-in",     "MAP file");
    cmd.addParam("dcmimg-out",     "DICOM output filename");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                      "-h",        "print this help text and exit");
     cmd.addOption("--verbose",                   "-v",        "verbose mode, print processing details");
     cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");
    cmd.addGroup("LUT creation options:");
      cmd.addSubGroup("LUT content:");
       cmd.addOption("--bits",        "-b", 1, "[n]umber : integer",
                                               "create LUT with n bit values (10..16, default: 12)");
       cmd.addOption("--entries",     "-e", 1, "[n]umber : integer",
                                               "create LUT with n entries (1..65536, default: 256)");
      cmd.addSubGroup("LUT data VR:");
       cmd.addOption("--data-ow",     "+Dw",    "write LUT Data as OW (default)");
       cmd.addOption("--data-us",     "+Du",    "write LUT Data as US");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      cmd.getParam(1, opt_inName);
      cmd.getParam(2, opt_outName);

      if (cmd.findOption("--verbose")) opt_verbose=OFTrue;
      if (cmd.findOption("--debug")) SetDebugLevel(3);
      if (cmd.findOption("--bits")) app.checkValue(cmd.getValue(bits,(OFCmdUnsignedInt)10,(OFCmdUnsignedInt)16));
      if (cmd.findOption("--entries")) app.checkValue(cmd.getValue(entries,(OFCmdUnsignedInt)1,(OFCmdUnsignedInt)65536));

      cmd.beginOptionBlock();
      if (cmd.findOption("--data-us")) lutVR = EVR_US;
      if (cmd.findOption("--data-ow")) lutVR = EVR_OW;
      cmd.endOptionBlock();
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded()) {
    cerr << "Warning: no data dictionary loaded, "
         << "check environment variable: "
         << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }

    unsigned char buffer[1000];
    FILE *inf;
    if (NULL == (inf = fopen(opt_inName, "rb")))
    {
    cerr << "cannot open file: " << opt_inName << endl;
        return 1;
    }

    if (264 !=  fread(buffer, 1, 264, inf))
    {
    cerr << "read error in file: " << opt_inName << endl;
        return 1;
    }

    if (0 !=  fread(buffer+264, 1, 1, inf))
    {
    cerr << "file too large, not a map file: " << opt_inName << endl;
        return 1;
    }

    fclose(inf);
    if ((buffer[0] != 0x8a)||(buffer[1] != 0x3f)||(buffer[2] != 0x0)||(buffer[3] != 0x0))
    {
    cerr << "magic word wrong, not a map file: " << opt_inName << endl;
        return 1;
    }

    /* create LUT */
    E_Condition result = EC_Normal;

    /* create Item with LUT */
    DcmItem *ditem = new DcmItem();
    DcmSequenceOfItems *dseq = new DcmSequenceOfItems(DCM_PresentationLUTSequence);
    DcmFileFormat fileformat;
    DcmDataset *dataset = fileformat.getDataset();
    if (ditem)
    {
      result = createLUT(buffer+8, (Uint16)bits, entries, lutVR, *ditem);
      if (EC_Normal != result)
      {
        cerr << "could not create LUT, bailing out." << endl;
        return 1;
      }
    }
    if (dseq && ditem)
    {
      dseq->insert(ditem);
      dataset->insert(dseq, OFTrue);
    } else result = EC_MemoryExhausted;

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

    fileformat.transferInit();
    result = fileformat.write(outf, EXS_LittleEndianExplicit);
    fileformat.transferEnd();

    if (result != EC_Normal)
    {
      cerr << "Error: "
           << dcmErrorConditionToString(result)
           << ": writing file: " <<  opt_outName << endl;
      return 1;
    }
    return 0;
}


/*
** CVS/RCS Log:
** $Log: dconvmap.cc,v $
** Revision 1.3  1999-10-01 14:53:58  joergr
** Fixed type conversion problems reported by MSVC5.
**
** Revision 1.2  1999/10/01 13:31:46  joergr
** Added new command line option specifying the number of LUT entries to
** MAP file conversion tool.
**
** Revision 1.1  1999/09/08 16:42:51  meichel
** Added sample application that converts PhotoImpact MAP files
**   to DICOM Presentation LUTs.
**
**
*/
