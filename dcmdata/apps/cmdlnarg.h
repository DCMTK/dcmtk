/*
**
** Author: Andrew Hewett      Created:  12-03-96
**
** Module: cmdlnarg.h
**
** Purpose:
** Provide prototype of command line argument gathering routine
** for OS environments which cannot pass arguments on the command line.
**
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-03-12 15:08:05 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/Attic/cmdlnarg.h,v $
** CVS/RCS Revision:	$Revision: 1.1 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

/*
** prepareCmdLineArgs 
**
** Should do nothing on Unix OS's.
** On other OS's (e.g. MacOS with CW6) allows command line arguments
** to be input from the standard input.
*/
void prepareCmdLineArgs(int& argc, char** );



/*
** CVS/RCS Log:
** $Log: cmdlnarg.h,v $
** Revision 1.1  1996-03-12 15:08:05  hewett
** Main program argc, argv handling for environments which do not make
** argc,argv available in main().
**
**
**
*/

