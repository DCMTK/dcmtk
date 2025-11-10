/*
 *
 *  Copyright (C) 2018-2025, OFFIS e.V.
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
 *  Author:  Pedro Arizpe, Marco Eichelberg
 *
 *  Purpose: Encapsulate document into DICOM format
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first*/
#include "dcmtk/dcmdata/dcencdoc.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/ofstd/ofconapp.h"


#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmencap"

static OFLogger dcmencapLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

int main(int argc, char *argv[])
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Encapsulate document into DICOM format", rcsid);
    OFCommandLine cmd;
    OFCondition result = EC_Normal;
    dcmEnableGenerationOfNewVRs();

    // process command line options
    DcmEncapsulatedDocument encapsulator;
    encapsulator.addCommandlineOptions(cmd);
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);

    if (app.parseCommandLine(cmd, argc, argv))
    {
        OFLOG_TRACE (dcmencapLogger, "Checking exclusive options first");
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
    OFLOG_DEBUG(dcmencapLogger, rcsid << OFendl);

    OFLOG_DEBUG(dcmencapLogger, "making sure data dictionary is loaded");
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcmencapLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    // read input document and place it into the dataset.
    // This call will also determine the file type (unless set by command line option).
    OFLOG_INFO(dcmencapLogger, "reading file '" << encapsulator.getInputFileName() << "'");
    result = encapsulator.insertEncapsulatedDocument();
    if (result.bad()) return EXITCODE_INVALID_INPUT_FILE;

    // read series file (if any) and otherwise generate study and series UID
    result = encapsulator.createIdentifiers();
    if (result.bad()) return EXITCODE_INVALID_INPUT_FILE;

    // perform document format specific processing (such as reading information from the CDA content)
    result = encapsulator.formatSpecificProcessing();
    if (result.bad()) return EXITCODE_INVALID_INPUT_FILE;

    // write DICOM header attributes to dataset
    result = encapsulator.createHeader();
    if (result.bad()) return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;

    // apply command line override keys (if any)
    result = encapsulator.applyOverrideKeys();
    if (result.bad()) return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;

    OFLOG_INFO(dcmencapLogger, "writing encapsulated DICOM object as file '" << encapsulator.getOutputFileName() << "'");

    // write DICOM file
    result = encapsulator.saveFile();
    if (result.bad()) return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;

    OFLOG_INFO(dcmencapLogger, "DICOM encapsulation successful");
    return EXITCODE_NO_ERROR;
}
