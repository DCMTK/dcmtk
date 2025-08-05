/*
 *
 *  Copyright (C) 2024, OFFIS e.V.
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
 *  Module:  dcmseg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Read and write DICOM SEG files to test the dcmseg library
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcrledrg.h"
#include "dcmtk/dcmdata/dcrleerg.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmseg/segdoc.h"


#ifdef WITH_ZLIB
#include <zlib.h>      /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "segdigest"


static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************

static OFLogger segDigestLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);
#define SHORTCOL 3
#define LONGCOL 21

int main(int argc, char *argv[])
{

  const char *opt_ifname = NULL;
  const char *opt_ofname = NULL;

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Encode DICOM file to RLE transfer syntax", rcsid);
  OFCommandLine cmd;
  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

  cmd.addParam("dcmfile-in",  "DICOM input filename to be converted\n(\"-\" for stdin)");
  cmd.addParam("dcmfile-out", "DICOM output filename\n(\"-\" for stdout)");

  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
    cmd.addOption("--help",                  "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
    cmd.addOption("--version",                         "print version information and exit", OFCommandLine::AF_Exclusive);

  cmd.addGroup("processing options:", LONGCOL, SHORTCOL + 2);
    cmd.addOption("--no-write", "-nw", "do not write output file, just read input file and check for errors", 0);

  cmd.addGroup("output options:", LONGCOL, SHORTCOL + 2);
    cmd.addOption("--ignore-missing",                  "-i",     "ignore missing attributes", 0);

    OFLog::addOptions(cmd);

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

      cmd.getParam(1, opt_ifname);
      cmd.getParam(2, opt_ofname);

      OFLog::configureFromCommandLine(cmd, app);


    }

    /* print resource identifier */
    OFLOG_DEBUG(segDigestLogger, rcsid << OFendl);

    // register RLE compression codec
    DcmRLEEncoderRegistration::registerCodecs();
    DcmRLEDecoderRegistration::registerCodecs();
    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(segDigestLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    // open inputfile
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        OFLOG_FATAL(segDigestLogger, "invalid filename: <empty string>");
        return 1;
    }

    OFLOG_INFO(segDigestLogger, "opening input file " << opt_ifname);

    DcmSegmentation* seg = NULL;
    DcmSegmentation::LoadingFlags flags;
    flags.m_numThreads = 16;
    OFCondition cond = DcmSegmentation::loadFile(opt_ifname, seg, flags);
    if (cond.bad())
    {
        OFLOG_FATAL(segDigestLogger, "error loading file: " << cond.text());
        return 1;
    }
    if (cmd.findOption("--no-write"))
    {
        OFLOG_INFO(segDigestLogger, "not writing output file on user request");
        delete seg;
        return 0;
    }
    // ignore missing values if desired
    if (cmd.findOption("--ignore-missing"))
    {
        std::cout << "Warning: Using --ignore-missing will skip checks for missing input files." << std::endl;
        seg->setValueCheckOnWrite(OFFalse);
        seg->setCheckDimensionsOnWrite(OFFalse);
        seg->setCheckFGOnWrite(OFFalse);
    }
    seg->getFunctionalGroups().setUseThreads(16);
    cond = seg->saveFile(opt_ofname, EXS_LittleEndianExplicit);
    if (cond.bad())
    {
        OFLOG_FATAL(segDigestLogger, "error saving file: " << cond.text());
        return 1;
    }

    OFLOG_INFO(segDigestLogger, "digesting successful");

    // deregister RLE codec
    DcmRLEEncoderRegistration::cleanup();
    DcmRLEDecoderRegistration::cleanup();

    return 0;
}
