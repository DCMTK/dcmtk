/*
 *
 *  Copyright (C) 1996-2001, OFFIS
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
 *  Module:  dcmwlm
 *
 *  Author:  Thomas Wilkens
 *
 *  Purpose: Basic work list management service call provider based on a
 *           database as data source.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-01-08 17:44:45 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/apps/Attic/wlmscpdb.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "dicom.h"
#include "ofcmdln.h"
#include "wltypdef.h"
#include "dcxfer.h"
#include "ofconapp.h"
#include "wlmceng.h"

#define OFFIS_CONSOLE_APPLICATION "wlmscpdb"

int main( int argc, char *argv[] )
{
  // Initialize and provide service. After having terminated free memory.
  WlmConsoleEngine *consoleEngine = new WlmConsoleEngine( argc, argv, DATA_SOURCE_IS_DATABASE, OFFIS_CONSOLE_APPLICATION );
  int result = consoleEngine->StartProvidingService();
  delete consoleEngine;

  return( result );
}

/*
** CVS Log
** $Log: wlmscpdb.cc,v $
** Revision 1.2  2002-01-08 17:44:45  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.1  2002/01/08 16:29:50  joergr
** Added new module "dcmwlm" developed by Thomas Wilkens (initial release for
** Windows, dated 2001-12-20).
**
**
*/
