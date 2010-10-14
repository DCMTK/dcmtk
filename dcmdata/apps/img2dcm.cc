/*
 *
 *  Copyright (C) 2007-2010, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Implements utility for converting standard image formats to DICOM
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:30 $
 *  CVS/RCS Revision: $Revision: 1.20 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/libi2d/i2d.h"
#include "dcmtk/dcmdata/libi2d/i2djpgs.h"
#include "dcmtk/dcmdata/libi2d/i2dbmps.h"
#include "dcmtk/dcmdata/libi2d/i2dplsc.h"
#include "dcmtk/dcmdata/libi2d/i2dplvlp.h"
#include "dcmtk/dcmdata/libi2d/i2dplnsc.h"

#define OFFIS_CONSOLE_APPLICATION "img2dcm"
static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v" OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define SHORTCOL 4
#define LONGCOL 21

static OFLogger img2dcmLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static OFCondition evaluateFromFileOptions(OFCommandLine& cmd,
                                           Image2Dcm& converter)
{
  OFCondition cond;
  // Parse command line options dealing with DICOM file import
  if ( cmd.findOption("--dataset-from") )
  {
    OFString tempStr;
    OFCommandLine::E_ValueStatus valStatus;
    valStatus = cmd.getValue(tempStr);
    if (valStatus != OFCommandLine::VS_Normal)
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to read value of --dataset-from option");
    converter.setTemplateFile(tempStr);
  }

  if (cmd.findOption("--study-from"))
  {
    OFString tempStr;
    OFCommandLine::E_ValueStatus valStatus;
    valStatus = cmd.getValue(tempStr);
    if (valStatus != OFCommandLine::VS_Normal)
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to read value of --study-from option");
    converter.setStudyFrom(tempStr);
  }

  if (cmd.findOption("--series-from"))
  {
    OFString tempStr;
    OFCommandLine::E_ValueStatus valStatus;
    valStatus = cmd.getValue(tempStr);
    if (valStatus != OFCommandLine::VS_Normal)
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to read value of --series-from option");
    converter.setSeriesFrom(tempStr);
  }

  if (cmd.findOption("--instance-inc"))
    converter.setIncrementInstanceNumber(OFTrue);

  // Return success
  return EC_Normal;
}


static void addCmdLineOptions(OFCommandLine& cmd)
{
  cmd.addParam("imgfile-in",  "image input filename");
  cmd.addParam("dcmfile-out", "DICOM output filename");

  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
    cmd.addOption("--help",                  "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive);
    cmd.addOption("--version",                          "print version information and exit", OFCommandLine::AF_Exclusive);
    OFLog::addOptions(cmd);

  cmd.addGroup("input options:", LONGCOL, SHORTCOL + 2);
    cmd.addSubGroup("general:");
      cmd.addOption("--input-format",        "-i",   1, "[i]nput file format: string", "supported formats: JPEG (default), BMP");
      cmd.addOption("--dataset-from",        "-df",  1, "[f]ilename: string",
                                                        "use dataset from DICOM file f");

      cmd.addOption("--study-from",          "-stf", 1, "[f]ilename: string",
                                                        "read patient/study from DICOM file f");
      cmd.addOption("--series-from",         "-sef", 1, "[f]ilename: string",
                                                        "read patient/study/series from DICOM file f");
      cmd.addOption("--instance-inc",        "-ii",     "increase instance number read from DICOM file");
    cmd.addSubGroup("JPEG format:");
      cmd.addOption("--disable-progr",       "-dp",     "disable support for progressive JPEG");
      cmd.addOption("--disable-ext",         "-de",     "disable support for extended sequential JPEG");
      cmd.addOption("--insist-on-jfif",      "-jf",     "insist on JFIF header");
      cmd.addOption("--keep-appn",           "-ka",     "keep APPn sections (except JFIF)");

  cmd.addGroup("processing options:", LONGCOL, SHORTCOL + 2);
    cmd.addSubGroup("attribute checking:");
      cmd.addOption("--do-checks",                      "enable attribute validity checking (default)");
      cmd.addOption("--no-checks",                      "disable attribute validity checking");
      cmd.addOption("--insert-type2",        "+i2",     "insert missing type 2 attributes (default)\n(only with --do-checks)");
      cmd.addOption("--no-type2-insert",     "-i2",     "do not insert missing type 2 attributes \n(only with --do-checks)");
      cmd.addOption("--invent-type1",        "+i1",     "invent missing type 1 attributes (default)\n(only with --do-checks)");
      cmd.addOption("--no-type1-invent",     "-i1",     "do not invent missing type 1 attributes\n(only with --do-checks)");
    cmd.addSubGroup("character set:");
      cmd.addOption("--latin1",              "+l1",     "set latin-1 as standard character set (default)");
      cmd.addOption("--no-latin1",           "-l1",     "keep 7-bit ASCII as standard character set");
    cmd.addSubGroup("other processing options:");
      cmd.addOption("--key",                 "-k",   1, "[k]ey: gggg,eeee=\"str\", path or dict. name=\"str\"",
                                                        "add further attribute");

  cmd.addGroup("output options:");
    cmd.addSubGroup("target SOP class:");
      cmd.addOption("--sec-capture",         "-sc",     "write Secondary Capture SOP class (default)");
      cmd.addOption("--new-sc",              "-nsc",    "write new Secondary Capture SOP classes");
      cmd.addOption("--vl-photo",            "-vlp",    "write Visible Light Photographic SOP class");

    cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",          "+F",      "write file format (default)");
      cmd.addOption("--write-dataset",       "-F",      "write data set without file meta information");
    cmd.addSubGroup("group length encoding:");
      cmd.addOption("--group-length-recalc", "+g=",     "recalculate group lengths if present (default)");
      cmd.addOption("--group-length-create", "+g",      "always write with group length elements");
      cmd.addOption("--group-length-remove", "-g",      "always write without group length elements");
    cmd.addSubGroup("length encoding in sequences and items:");
      cmd.addOption("--length-explicit",     "+e",      "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",    "-e",      "write with undefined lengths");
    cmd.addSubGroup("data set trailing padding (not with --write-dataset):");
      cmd.addOption("--padding-off",         "-p",      "no padding (implicit if --write-dataset)");
      cmd.addOption("--padding-create",      "+p",   2, "[f]ile-pad [i]tem-pad: integer",
                                                        "align file on multiple of f bytes\nand items on multiple of i bytes");
}


static OFCondition startConversion(OFCommandLine& cmd,
                                   int argc,
                                   char *argv[])
{
  // Parse command line and exclusive options
  prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Convert standard image formats into DICOM format", rcsid);
  if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
  {
    /* check exclusive options first */
    if (cmd.hasExclusiveOption())
    {
      if (cmd.findOption("--version"))
      {
        app.printHeader(OFTrue /*print host identifier*/);
        exit(0);
      }
    }
  }

  /* print resource identifier */
  OFLOG_DEBUG(img2dcmLogger, rcsid << OFendl);

  // Main class for controlling conversion
  Image2Dcm i2d;
  // Output plugin to use (ie. SOP class to write)
  I2DOutputPlug *outPlug = NULL;
  // Input plugin to use (ie. file format to read)
  I2DImgSource *inputPlug = NULL;
  // Group length encoding mode for output DICOM file
  E_GrpLenEncoding grpLengthEnc = EGL_recalcGL;
  // Item and Sequence encoding mode for output DICOM file
  E_EncodingType lengthEnc = EET_ExplicitLength;
  // Padding mode for output DICOM file
  E_PaddingEncoding padEnc = EPD_noChange;
  // File pad length for output DICOM file
  OFCmdUnsignedInt filepad = 0;
  // Item pad length for output DICOM file
  OFCmdUnsignedInt itempad = 0;
  // Write only pure dataset, i.e. without meta header
  E_FileWriteMode writeMode = EWM_fileformat;
  // Override keys are applied at the very end of the conversion "pipeline"
  OFList<OFString> overrideKeys;
  // The transfersytanx proposed to be written by output plugin
  E_TransferSyntax writeXfer;

  // Parse rest of command line options
  OFLog::configureFromCommandLine(cmd, app);

  OFString pixDataFile, outputFile, tempStr;
  cmd.getParam(1, tempStr);

  if (tempStr.length() == 0)
  {
    OFLOG_ERROR(img2dcmLogger, "No image input filename specified");
    return EC_IllegalCall;
  }
  else
    pixDataFile = tempStr;

  cmd.getParam(2, tempStr);
  if (tempStr.length() == 0)
  {
    OFLOG_ERROR(img2dcmLogger, "No DICOM output filename specified");
    return EC_IllegalCall;
  }
  else
    outputFile = tempStr;

  if (cmd.findOption("--input-format"))
  {
    app.checkValue(cmd.getValue(tempStr));
    if (tempStr == "JPEG")
    {
      inputPlug = new I2DJpegSource();
    }
    else if (tempStr == "BMP")
    {
      inputPlug = new I2DBmpSource();
    }
    else
    {
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "No plugin for selected input format available");
    }
    if (!inputPlug)
    {
      return EC_MemoryExhausted;
    }
  }
  else // default is JPEG
  {
    inputPlug = new I2DJpegSource();
  }
  OFLOG_INFO(img2dcmLogger, OFFIS_CONSOLE_APPLICATION ": Instantiated input plugin: " << inputPlug->inputFormat());

 // Find out which plugin to use
  cmd.beginOptionBlock();
  if (cmd.findOption("--sec-capture"))
    outPlug = new I2DOutputPlugSC();
  if (cmd.findOption("--vl-photo"))
  {
    outPlug = new I2DOutputPlugVLP();
  }
  if (cmd.findOption("--new-sc"))
    outPlug = new I2DOutputPlugNewSC();
  cmd.endOptionBlock();
  if (!outPlug) // default is the old Secondary Capture object
    outPlug = new I2DOutputPlugSC();
  if (outPlug == NULL) return EC_MemoryExhausted;
  OFLOG_INFO(img2dcmLogger, OFFIS_CONSOLE_APPLICATION ": Instantiatiated output plugin: " << outPlug->ident());

  cmd.beginOptionBlock();
  if (cmd.findOption("--write-file"))    writeMode = EWM_fileformat;
  if (cmd.findOption("--write-dataset")) writeMode = EWM_dataset;
  cmd.endOptionBlock();

  cmd.beginOptionBlock();
  if (cmd.findOption("--group-length-recalc")) grpLengthEnc = EGL_recalcGL;
  if (cmd.findOption("--group-length-create")) grpLengthEnc = EGL_withGL;
  if (cmd.findOption("--group-length-remove")) grpLengthEnc = EGL_withoutGL;
  cmd.endOptionBlock();

  cmd.beginOptionBlock();
  if (cmd.findOption("--length-explicit"))  lengthEnc = EET_ExplicitLength;
  if (cmd.findOption("--length-undefined")) lengthEnc = EET_UndefinedLength;
  cmd.endOptionBlock();

  cmd.beginOptionBlock();
  if (cmd.findOption("--padding-off"))
  {
    filepad = 0;
    itempad = 0;
  }
  else if (cmd.findOption("--padding-create"))
  {
    OFCmdUnsignedInt opt_filepad; OFCmdUnsignedInt opt_itempad;
    app.checkValue(cmd.getValueAndCheckMin(opt_filepad, 0));
    app.checkValue(cmd.getValueAndCheckMin(opt_itempad, 0));
    itempad = opt_itempad;
    filepad = opt_filepad;
  }
  cmd.endOptionBlock();

  // create override attribute dataset (copied from findscu code)
  if (cmd.findOption("--key", 0, OFCommandLine::FOM_FirstFromLeft))
  {
    const char *ovKey = NULL;
    do {
      app.checkValue(cmd.getValue(ovKey));
      overrideKeys.push_back(ovKey);
    } while (cmd.findOption("--key", 0, OFCommandLine::FOM_NextFromLeft));
  }
  i2d.setOverrideKeys(overrideKeys);

  // Test for ISO Latin 1 option
  OFBool insertLatin1 = OFTrue;
  cmd.beginOptionBlock();
  if (cmd.findOption("--latin1"))
    insertLatin1 = OFTrue;
  if (cmd.findOption("--no-latin1"))
    insertLatin1 = OFFalse;
  cmd.endOptionBlock();
  i2d.setISOLatin1(insertLatin1);

  // evaluate validity checking options
  OFBool insertType2 = OFTrue;
  OFBool inventType1 = OFTrue;
  OFBool doChecks = OFTrue;
  cmd.beginOptionBlock();
  if (cmd.findOption("--no-checks"))
    doChecks = OFFalse;
  if (cmd.findOption("--do-checks"))
    doChecks = OFTrue;
  cmd.endOptionBlock();

  cmd.beginOptionBlock();
  if (cmd.findOption("--insert-type2"))
    insertType2 = OFTrue;
  if (cmd.findOption("--no-type2-insert"))
    insertType2 = OFFalse;
  cmd.endOptionBlock();

  cmd.beginOptionBlock();
  if (cmd.findOption("--invent-type1"))
    inventType1 = OFTrue;
  if (cmd.findOption("--no-type1-invent"))
    inventType1 = OFFalse;
  cmd.endOptionBlock();
  i2d.setValidityChecking(doChecks, insertType2, inventType1);
  outPlug->setValidityChecking(doChecks, insertType2, inventType1);

  // evaluate --xxx-from options and transfer syntax options
  OFCondition cond;
  cond = evaluateFromFileOptions(cmd, i2d);
  if (cond.bad())
  {
    delete outPlug; outPlug = NULL;
    delete inputPlug; inputPlug = NULL;
    return cond;
  }

  if (inputPlug->inputFormat() == "JPEG")
  {
    I2DJpegSource *jpgSource = OFstatic_cast(I2DJpegSource*, inputPlug);
    if (!jpgSource)
    {
       delete outPlug; outPlug = NULL;
       delete inputPlug; inputPlug = NULL;
       return EC_MemoryExhausted;
    }
    if ( cmd.findOption("--disable-progr") )
      jpgSource->setProgrSupport(OFFalse);
    if ( cmd.findOption("--disable-ext") )
      jpgSource->setExtSeqSupport(OFFalse);
    if ( cmd.findOption("--insist-on-jfif") )
      jpgSource->setInsistOnJFIF(OFTrue);
    if ( cmd.findOption("--keep-appn") )
      jpgSource->setKeepAPPn(OFTrue);
  }
  inputPlug->setImageFile(pixDataFile);

  /* make sure data dictionary is loaded */
  if (!dcmDataDict.isDictionaryLoaded())
  {
    OFLOG_WARN(img2dcmLogger, "no data dictionary loaded, check environment variable: "
      << DCM_DICT_ENVIRONMENT_VARIABLE);
  }

  DcmDataset *resultObject = NULL;
  OFLOG_INFO(img2dcmLogger, OFFIS_CONSOLE_APPLICATION ": Starting image conversion");
  cond = i2d.convert(inputPlug, outPlug, resultObject, writeXfer);

  // Save
  if (cond.good())
  {
    OFLOG_INFO(img2dcmLogger, OFFIS_CONSOLE_APPLICATION ": Saving output DICOM to file " << outputFile);
    DcmFileFormat dcmff(resultObject);
    cond = dcmff.saveFile(outputFile.c_str(), writeXfer, lengthEnc, grpLengthEnc, padEnc, filepad, itempad, writeMode);
  }

  // Cleanup and return
  delete outPlug; outPlug = NULL;
  delete inputPlug; inputPlug = NULL;
  delete resultObject; resultObject = NULL;

  return cond;
}


int main(int argc, char *argv[])
{

  // variables for command line
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Convert image file to DICOM", rcsid);
  OFCommandLine cmd;

  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
  addCmdLineOptions(cmd);

  OFCondition cond = startConversion(cmd, argc, argv);
  if (cond.bad())
  {
    OFLOG_FATAL(img2dcmLogger, "Error converting file: " << cond.text());
    return 1;
  }

  return 0;
}



/*
 * CVS/RCS Log:
 * $Log: img2dcm.cc,v $
 * Revision 1.20  2010-10-14 13:13:30  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.19  2010-10-06 09:14:23  joergr
 * Added text on the fact that the --key option also supports attribute paths.
 * Introduced meaningful sub groups for the processing command line options.
 *
 * Revision 1.18  2009-11-13 13:20:23  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.17  2009-11-04 09:58:06  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.16  2009-09-30 08:05:25  uli
 * Stop including dctk.h in libi2d's header files.
 *
 * Revision 1.15  2009-08-21 09:25:13  joergr
 * Added parameter 'writeMode' to save/write methods which allows for specifying
 * whether to write a dataset or fileformat as well as whether to update the
 * file meta information or to create a new file meta information header.
 *
 * Revision 1.14  2009-07-16 14:26:25  onken
 * Added img2dcm input plugin for the BMP graphics format (at the moment only
 * support for 24 Bit RGB).
 *
 * Revision 1.13  2009-07-10 13:16:10  onken
 * Added path functionality for --key option and lets the code make use
 * of the DcmPath classes.
 *
 * Revision 1.12  2009-04-24 12:20:42  joergr
 * Fixed minor inconsistencies regarding layout/formatting in syntax usage.
 *
 * Revision 1.11  2009-04-21 14:02:49  joergr
 * Fixed minor inconsistencies in manpage / syntax usage.
 *
 * Revision 1.10  2009-03-31 10:47:41  onken
 * Added NULL pointer check.
 *
 * Revision 1.9  2008-10-29 18:03:33  joergr
 * Fixed minor inconsistencies.
 *
 * Revision 1.8  2008-09-25 14:35:34  joergr
 * Moved checking on presence of the data dictionary.
 *
 * Revision 1.7  2008-09-25 11:19:48  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.6  2008-01-16 16:32:14  onken
 * Fixed some empty or doubled log messages in libi2d files.
 *
 * Revision 1.5  2008-01-16 15:19:41  onken
 * Moved library "i2dlib" from /dcmdata/libsrc/i2dlib to /dcmdata/libi2d
 *
 * Revision 1.4  2008-01-14 16:51:11  joergr
 * Fixed minor inconsistencies.
 *
 * Revision 1.3  2008-01-11 14:16:04  onken
 * Added various options to i2dlib. Changed logging to use a configurable
 * logstream. Added output plugin for the new Multiframe Secondary Capture SOP
 * Classes. Added mode for JPEG plugin to copy exsiting APPn markers (except
 * JFIF). Changed img2dcm default behaviour to invent type1/type2 attributes (no
 * need for templates any more). Added some bug fixes.
 *
 * Revision 1.1  2007/11/08 16:00:34  onken
 * Initial checkin of img2dcm application and corresponding library i2dlib.
 *
 *
 */
