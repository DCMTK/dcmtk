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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:30 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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


/*
** CVS/RCS Log:
** $Log: dcmodify.cc,v $
** Revision 1.12  2010-10-14 13:13:30  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.11  2009-11-04 09:58:06  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.10  2009-01-15 16:11:55  onken
** Reworked dcmodify to work with the new DcmPath classes for supporting
** wildcard paths and automatic insertion of missing attributes and items.
** Added options for private tag handling and modification of UN values and
** for ignoring errors resulting from missing tags during modify and erase
** operations. Further cleanups.
**
** Revision 1.9  2008-09-25 11:19:48  joergr
** Added support for printing the expanded command line arguments.
** Always output the resource identifier of the command line tool in debug mode.
**
** Revision 1.8  2006/08/15 15:50:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.7  2006/07/27 13:34:35  joergr
** Updated copyright date.
**
** Revision 1.6  2005/12/08 15:40:49  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.5  2004/10/22 16:53:26  onken
** - fixed ignore-errors-option
** - major enhancements for supporting private tags
** - removed '0 Errors' output
** - modifications to groups 0000,0001,0002,0003,0005 and 0007 are blocked,
**   removing tags with group 0001,0003,0005 and 0007 is still possible
** - UID options:
**   - generate new study, series and instance UIDs
**   - When changing UIDs in dataset, related metaheader tags are updated
**     automatically
** - minor code improvements
**
** Revision 1.4  2003/10/13 14:52:59  onken
** error-message adapted to mdfconen.cc
**
** Revision 1.3  2003/09/19 12:47:21  onken
** return-value is now only zero, if no error occurred
**
** Revision 1.2  2003/07/09 12:13:13  meichel
** Included dcmodify in MSVC build system, updated headers
**
** Revision 1.1  2003/06/26 09:17:18  onken
** Added commandline-application dcmodify.
**
**
*/
