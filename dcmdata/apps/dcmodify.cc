/*
 *
 *  Copyright (C) 2003-2010, OFFIS e.V.
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
 *  Purpose: Commandline-Application to modify tags in DICOM-Files
 *
 */

#include "dcmtk/config/osconfig.h"   // make sure OS specific configuration is included first
#include "mdfconen.h"
#include "dcmtk/dcmdata/dcpath.h"

#define OFFIS_CONSOLE_APPLICATION "dcmodify"

static OFLogger dcmodifyLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

int main(int argc, char *argv[])
{
  int error_count = 0;
  MdfConsoleEngine engine(argc, argv, OFFIS_CONSOLE_APPLICATION);
  error_count = engine.startProvidingService();
  if (error_count == 1)
    OFLOG_ERROR(dcmodifyLogger, "There was 1 error");
  else if (error_count > 1)
    OFLOG_ERROR(dcmodifyLogger, "There were " << error_count << " errors");
  return(error_count);
}
