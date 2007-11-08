/*
 *
 *  Copyright (C) 2001-2007, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Implements utility for converting standard image formats to DICOM
 *
 *  Last Update:      $$
 *  Update Date:      $$
 *  Source File:      $$
 *  CVS/RCS Revision: $$
 *  Status:           $$
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/i2dlib/i2d.h"
#include "dcmtk/dcmdata/i2dlib/i2djpgs.h"
#include "dcmtk/dcmdata/i2dlib/i2dplsc.h"
#include "dcmtk/dcmdata/i2dlib/i2dplvlp.h"

#define OFFIS_CONSOLE_APPLICATION "img2dcm"
static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v" OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";
#define SHORTCOL 4
#define LONGCOL 21


/** static helper function that adds a given "override key" using image2dcm's command line syntax
 *  to the given dataset of override keys. The name "override" indicates that these keys have
 *  higher precedence than identical keys in the image dataset that might possibly read from
 *  a DICOM file or set by other command line parameters.
 *  @param overrideKeys the attribute is added to this dataset, if successful
 *    The overrideKeys object is created on the heap and a pointer passed back to the caller
 *    if the caller passes a reference to a NULL pointer.
 *  @param app console application object, only used for error output.
 *  @param s override key
 */
static void addOverrideKey(DcmDataset * & overrideKeys,
                           OFConsoleApplication& app,
                           const char* s)
{
  unsigned int g = 0xffff;
  unsigned int e = 0xffff;
  int n = 0;
  char val[1024];
  OFString dicName, valStr;
  OFString msg;
  char msg2[200];
  val[0] = '\0';

  // try to parse group and element number
  n = sscanf(s, "%x,%x=%s", &g, &e, val);
  OFString toParse = s;
  size_t eqPos = toParse.find('=');
  if (n < 2)  // if at least no tag could be parsed
  {
    // if value is given, extract it (and extrect dictname)
    if (eqPos != OFString_npos)
    {
      dicName = toParse.substr(0,eqPos).c_str();
      valStr = toParse.substr(eqPos+1,toParse.length());
    }
    else // no value given, just dictionary name
      dicName = s; // only dictionary name given (without value)
    // try to lookup in dictionary
    DcmTagKey key(0xffff,0xffff);
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dicent = globalDataDict.findEntry(dicName.c_str());
    dcmDataDict.unlock();
    if (dicent!=NULL) {
      // found dictionary name, copy group and element number
      key = dicent->getKey();
      g = key.getGroup();
      e = key.getElement();
    }
    else {
      // not found in dictionary
      msg = "bad key format or dictionary name not found in dictionary: ";
      msg += dicName;
      app.printError(msg.c_str());
    }
  } // tag could be parsed, copy value if it exists
  else
  {
    if (eqPos != OFString_npos)
      valStr = toParse.substr(eqPos+1,toParse.length());
  }
  DcmTag tag(g,e);
  if (tag.error() != EC_Normal) {
      sprintf(msg2, "unknown tag: (%04x,%04x)", g, e);
      app.printError(msg2);
  }
  DcmElement *elem = newDicomElement(tag);
  if (elem == NULL) {
      sprintf(msg2, "cannot create element for tag: (%04x,%04x)", g, e);
      app.printError(msg2);
  }
  if (valStr.length() > 0) {
      if (elem->putString(valStr.c_str()).bad())
      {
          sprintf(msg2, "cannot put tag value: (%04x,%04x)=\"", g, e);
          msg = msg2;
          msg += valStr;
          msg += "\"";
          app.printError(msg.c_str());
      }
  }

  if (overrideKeys == NULL) overrideKeys = new DcmDataset;
  if (overrideKeys->insert(elem, OFTrue).bad()) {
      sprintf(msg2, "cannot insert tag: (%04x,%04x)", g, e);
      app.printError(msg2);
  }
}


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

  if ( cmd.findOption("--study-from") )
  {
    OFString tempStr;
    OFCommandLine::E_ValueStatus valStatus;
    valStatus = cmd.getValue(tempStr);
    if (valStatus != OFCommandLine::VS_Normal)
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to read value of --study-from option");
    converter.setStudyFrom(tempStr);
  }

  else if ( cmd.findOption("--series-from") )
  {
    OFString tempStr;
    OFCommandLine::E_ValueStatus valStatus;
    valStatus = cmd.getValue(tempStr);
    if (valStatus != OFCommandLine::VS_Normal)
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to read value of --series-from option");
    converter.setSeriesFrom(tempStr);
  }

  if ( cmd.findOption("--instance-inc") )
    converter.setIncrementInstanceNumber(OFTrue);

    // Return success
  return EC_Normal;
}


static void addCmdLineOptions(OFCommandLine& cmd)
{
  cmd.addParam("imgfile-in",   "Image input filename");
  cmd.addParam("dcmfile-out",  "DICOM output filename");

  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
    cmd.addOption("--help",                   "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
    cmd.addOption("--version",                          "print version information and exit", OFCommandLine::AF_Exclusive);
    cmd.addOption("--verbose",                "-v",     "verbose mode, print processing details");
    cmd.addOption("--debug",                  "-d",     "debug mode, print debug information");

  cmd.addGroup("input options:", LONGCOL, SHORTCOL + 2);
    cmd.addSubGroup("general input options:");
      cmd.addOption("--input-format",            "-i",1,  "[i]nput file format: string","supported formats: JPEG (default)");
      cmd.addOption("--dataset-from", "-df", 1,  "[f]ilename : string",
                                                   "use dataset from DICOM file f");

      cmd.addOption("--study-from",   "-stf", 1, "[f]ilename : string",
                                                   "read patient/study from DICOM file f");
      cmd.addOption("--series-from",  "-sef", 1, "[f]ilename : string",
                                                   "read patient/study/series from DICOM file f");
      cmd.addOption("--instance-inc", "-ii",     "incr. instance number read from DICOM file");
    cmd.addSubGroup("JPEG input options:");
      cmd.addOption("--disable-progr", "-dp",     "disable support for progressive JPEG");
      cmd.addOption("--disable-ext",   "-de",     "disable support for extended sequential JPEG");

  cmd.addGroup("processing options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--no-checks",      "Disable attribute validity checking");
     cmd.addOption("--key",               "-k",   1, "key: gggg,eeee=\"str\" or dictionary name=\"str\"",
                                                       "add further attribute");

  cmd.addGroup("output options:");
    cmd.addSubGroup("target SOP class:");
      cmd.addOption("--vl-photo",            "-vlp",   "write Visible Light Photo SOP class (default)");
      cmd.addOption("--sec-capture",         "-sc",    "write Secondary Capture SOP class (depr.)");
    cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",          "+F",     "write file format (default)");
      cmd.addOption("--write-dataset",       "-F",     "write data set without file meta information");
    cmd.addSubGroup("group length encoding:");
      cmd.addOption("--group-length-recalc", "+g=",    "recalculate group lengths if present (default)");
      cmd.addOption("--group-length-create", "+g",     "always write with group length elements");
      cmd.addOption("--group-length-remove", "-g",     "always write without group length elements");
    cmd.addSubGroup("length encoding in sequences and items:");
      cmd.addOption("--length-explicit",     "+e",     "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",    "-e",     "write with undefined lengths");
    cmd.addSubGroup("data set trailing padding (not with --write-dataset):");
      cmd.addOption("--padding-off",         "-p",     "no padding (implicit if --write-dataset)");
      cmd.addOption("--padding-create",      "+p",  2, "[f]ile-pad [i]tem-pad: integer",
                                                         "align file on multiple of f bytes\nand items on multiple of i bytes");
}


static OFCondition startConversion(OFCommandLine& cmd,
                                   int argc,
                                   char *argv[])
{
  // Parse command line and exclusive options
  prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Convert image file to DICOM", rcsid);
  if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
  {
    /* check exclusive options first */

    if (cmd.hasExclusiveOption())
    {
      if (cmd.findOption("--version"))
      {
          app.printHeader(OFTrue /*print host identifier*/);          // uses ofConsole.lockCerr()
          exit(0);
       }
    }
  }

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
  OFBool writeOnlyDataset = OFFalse;
  // Override keys are applied at the very end of the conversion "pipeline"
  DcmDataset *overrideKeys;
  // The transfersytanx proposed to be written by output plugin
  E_TransferSyntax writeXfer;

  // Parse rest of command line options
  OFBool dMode = OFFalse; OFBool vMode = OFFalse;
  if (cmd.findOption("--verbose"))
  {
    vMode = OFTrue;
    i2d.setVerboseMode(OFTrue);
  }
  if (cmd.findOption("--debug"))
  {
    dMode = OFTrue;
    vMode = OFTrue;
    i2d.setDebugMode(OFTrue);
  }
  // Find out which plugin to use
  if (dMode)
    COUT << "Image2Dcm: Instantiating DICOM output plugin" << OFendl;
  cmd.beginOptionBlock();
    if (cmd.findOption("--sec-capture"))
      outPlug = new I2DOutputPlugSC();
    if (cmd.findOption("--vl-photo"))
      outPlug = new I2DOutputPlugVLP();
  cmd.endOptionBlock();
  if (!outPlug) // default is Visual Light Photography
    outPlug = new I2DOutputPlugVLP();

  outPlug->setVerboseMode(vMode);
  outPlug->setDebugMode(dMode);

  OFString pixDataFile, outputFile, tempStr;
  cmd.getParam(1, tempStr);

  if (tempStr.length() == 0)
  {
    CERR << "Error: No image input filename given" << OFendl;
    delete outPlug; outPlug = NULL;
    return EC_IllegalCall;
  }
  else
    pixDataFile = tempStr;

  cmd.getParam(2, tempStr);
  if (tempStr.length() == 0)
  {
    CERR << "Error: No DICOM output filename given" << OFendl;
    delete outPlug; outPlug = NULL;
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
      if (!inputPlug)
      {
        delete outPlug; outPlug = NULL;
        return EC_MemoryExhausted;
      }
    }
    else
    {
       delete outPlug; outPlug = NULL;
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "No plugin for selected input format available");
    }
  }
  else // default is JPEG
    inputPlug = new I2DJpegSource();


  cmd.beginOptionBlock();
    if (cmd.findOption("--write-file"))    writeOnlyDataset = OFFalse;
    if (cmd.findOption("--write-dataset")) writeOnlyDataset = OFTrue;
  cmd.endOptionBlock();

  cmd.beginOptionBlock();
  if (cmd.findOption("--group-length-recalc"))  grpLengthEnc = EGL_recalcGL;
  if (cmd.findOption("--group-length-create"))  grpLengthEnc = EGL_withGL;
  if (cmd.findOption("--group-length-remove"))  grpLengthEnc = EGL_withoutGL;
  cmd.endOptionBlock();

  cmd.beginOptionBlock();
  if (cmd.findOption("--length-explicit"))  lengthEnc = EET_ExplicitLength ;
  if (cmd.findOption("--length-undefined")) lengthEnc = EET_UndefinedLength;
  cmd.endOptionBlock();


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

  // create override attribute dataset (copied from findscu.cc code)
  if (cmd.findOption("--key", 0, OFCommandLine::FOM_First))
  {
    const char *ovKey = NULL;
    do {
      app.checkValue(cmd.getValue(ovKey));
      addOverrideKey(overrideKeys, app, ovKey);
    } while (cmd.findOption("--key", 0, OFCommandLine::FOM_Next));
  }

  if (cmd.findOption("--no-checks"))
    i2d.setValidityChecking(OFFalse);

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
      return EC_IllegalCall;
    if ( cmd.findOption("--disable-progr") )
      jpgSource->setProgrSupport(OFFalse);
    if ( cmd.findOption("--disable-ext") )
      jpgSource->setExtSeqSupport(OFFalse);
  }
  inputPlug->setImageFile(pixDataFile);

  DcmDataset *resultObject = NULL;
  cond = i2d.convert(inputPlug, outPlug, resultObject, writeXfer);
  if (cond.bad())
  {
    delete outPlug; outPlug = NULL;
    delete inputPlug; inputPlug = NULL;
    return cond;
  }
  // Save and return
  if (dMode || vMode)
    COUT << "image2dcm: Saving output DICOM to file " << outputFile << OFendl;
  DcmFileFormat dcmff(resultObject);
  cond = dcmff.saveFile(outputFile.c_str(), writeXfer, lengthEnc, grpLengthEnc, padEnc, filepad, itempad, writeOnlyDataset);
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
    CERR << "Error converting file: " << cond.text() << OFendl;
    return 1;
  }

  return 0;
}



/*
 * CVS/RCS Log:
 * $Log: img2dcm.cc,v $
 * Revision 1.1  2007-11-08 16:00:34  onken
 * Initial checkin of img2dcm application and corresponding library i2dlib.
 *
 *
 */
