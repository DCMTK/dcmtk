/*
 *
 *  Copyright (C) 1994-2002, OFFIS
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
 *  Author:  Gerd Ehlers
 *
 *  Purpose: Commandline-Application to modify tags in DICOM-Files
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2003-06-26 09:17:18 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmodify.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "mdfconen.h"

#define OFFIS_CONSOLE_APPLICATION "dcmodify"

int main(int argc, char *argv[])
{
    int error_count=0;
    MdfConsoleEngine c_engine(argc,argv,OFFIS_CONSOLE_APPLICATION);
    error_count=c_engine.startProvidingService();
    CERR << "There were " << error_count << " Error(s)" << endl;
    return 0;
}

/*
** CVS/RCS Log:
** $Log: dcmodify.cc,v $
** Revision 1.1  2003-06-26 09:17:18  onken
** Added commandline-application dcmodify.
**
**
*/

    

    
