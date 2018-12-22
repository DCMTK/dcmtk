/*
 *
 *  Copyright (C) 2018, OFFIS e.V.
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
 *  Author:  Pedro Arizpe
 *
 *  Purpose: Encapsulate STL file into a DICOM file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first*/
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcencdoc.h"

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "stl2dcm"

static OFLogger stl2dcmLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

int main(int argc, char *argv[])
{
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Encapsulate STL file into DICOM format", rcsid);
  OFCommandLine cmd;
  int errorCode = EXITCODE_NO_ERROR;
  OFCondition result = EC_Normal;
  DcmFileFormat fileformat;
  DcmEncapsulatedDocument encapsulator;

  OFLOG_TRACE(stl2dcmLogger, "Including necessary options");
  encapsulator.addSTLCommandlineOptions(cmd);
  OFLOG_TRACE(stl2dcmLogger, "Evaluating command line");
  prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);

  if (app.parseCommandLine(cmd, argc, argv)) {
    OFLOG_TRACE(stl2dcmLogger, "Checking exclusive options first");
    if (cmd.hasExclusiveOption())
    {
      if (cmd.findOption("--version"))
      {
        app.printHeader(OFTrue /*print host identifier*/);
        COUT << OFendl << "External libraries used: ";
#ifdef WITH_ZLIB
        COUT << OFendl << "- ZLIB, Version " << zlibVersion() << OFendl;
#else
        COUT << " none" << OFendl;
#endif
        return EXITCODE_NO_ERROR;
      }
    }
    encapsulator.parseArguments(app, cmd);
  }
  //print resource identifier
  OFLOG_DEBUG(stl2dcmLogger, rcsid << OFendl);

  OFLOG_DEBUG(stl2dcmLogger, "making sure data dictionary is loaded");
  if (!dcmDataDict.isDictionaryLoaded())
  {
    OFLOG_WARN(stl2dcmLogger, "no data dictionary loaded, check environment variable: "
      << DCM_DICT_ENVIRONMENT_VARIABLE);
  }
  OFLOG_DEBUG(stl2dcmLogger, "Creating identifiers (and reading series data)");
  result = encapsulator.createIdentifiers(stl2dcmLogger);
  if (result.bad())
  {
    OFLOG_FATAL(stl2dcmLogger, "There was an error while reading the series data");
    return EXITCODE_INVALID_INPUT_FILE;
  }
  OFLOG_INFO(stl2dcmLogger, "creating encapsulated STL object");
  errorCode = encapsulator.insertEncapsulatedDocument(fileformat.getDataset(), stl2dcmLogger);
  if (errorCode != EXITCODE_NO_ERROR)
  {
    OFLOG_ERROR(stl2dcmLogger, "unable to create STL encapsulation to DICOM format");
    return errorCode;
  }
  OFLOG_INFO(stl2dcmLogger, "Generating an instance number that is guaranteed to be unique within a series.");
  result = encapsulator.createHeader(fileformat.getDataset(), stl2dcmLogger);
  if (result.bad())
  {
    OFLOG_ERROR(stl2dcmLogger, "unable to create DICOM header: " << result.text());
    return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
  }
  OFLOG_INFO(stl2dcmLogger, "writing encapsulated STL object as file " << encapsulator.getOutputFileName());

  OFLOG_INFO(stl2dcmLogger, "Check if new output transfer syntax is possible");

  DcmXfer opt_oxferSyn(encapsulator.getTransferSyntax());

  fileformat.getDataset()->chooseRepresentation(encapsulator.getTransferSyntax(), NULL);
  if (fileformat.getDataset()->canWriteXfer(encapsulator.getTransferSyntax()))
  {
    OFLOG_INFO(stl2dcmLogger, "Output transfer syntax " << opt_oxferSyn.getXferName() << " can be written");
  }
  else {
    OFLOG_ERROR(stl2dcmLogger, "No conversion to transfer syntax " << opt_oxferSyn.getXferName() << " possible!");
    return EXITCODE_COMMANDLINE_SYNTAX_ERROR;
  }
  OFLOG_INFO(stl2dcmLogger, "Checking for DICOM key overriding");
  result = encapsulator.applyOverrideKeys(fileformat.getDataset());
  if (result.bad())
  {
    OFLOG_ERROR(stl2dcmLogger, "There was a problem while overriding a key:" << OFendl
      << result.text());
    return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
  }
  OFLOG_INFO(stl2dcmLogger, "write converted DICOM file with metaheader");
  result = encapsulator.saveFile(fileformat);
  if (result.bad())
  {
    OFLOG_ERROR(stl2dcmLogger, result.text() << ": writing file: " << encapsulator.getOutputFileName());
    return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
  }

  OFLOG_INFO(stl2dcmLogger, "STL encapsulation successful");

  return EXITCODE_NO_ERROR;
}
