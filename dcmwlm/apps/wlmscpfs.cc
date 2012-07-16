/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Module:  dcmwlm
 *
 *  Author:  Thomas Wilkens
 *
 *  Purpose: Basic work list management service class provider based on a
 *           set of files as data source.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/dcmwlm/wltypdef.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "wlcefs.h"
#include "dcmtk/dcmwlm/wlds.h"
#include "dcmtk/dcmwlm/wldsfs.h"

#define OFFIS_CONSOLE_APPLICATION "wlmscpfs"

int main( int argc, char *argv[] )
{
  // Initialize object which provides a connection to the data source
  WlmDataSourceFileSystem *dataSource = new WlmDataSourceFileSystem();

  // Initialize and provide service. After having terminated free memory.
  WlmConsoleEngineFileSystem *consoleEngine = new WlmConsoleEngineFileSystem( argc, argv, OFFIS_CONSOLE_APPLICATION, dataSource );
  int result = consoleEngine->StartProvidingService();

  // Free memory
  delete consoleEngine;
  delete dataSource;

  return( result );
}
