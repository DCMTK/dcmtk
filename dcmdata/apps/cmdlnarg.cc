/*
**
** Author: Andrew Hewett      Created:  12-03-96
**
** Module: cmdlnarg.cc
**
** Purpose:
** Provide a command line argument gathering routine
** for OS environments which cannot pass arguments on the command line.
**
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-03-12 15:08:06 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/Attic/cmdlnarg.cc,v $
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

#ifdef HAVE_EMPTY_ARGC_ARGV

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <string.h>
#include <iostream.h>
#include <sstream.h>

void prepareCmdLineArgs(int& argc, char* argv[])
{
    const int bufsize = 2024;
    char buf[bufsize];
    char arg[1024];

    argv[0] = "dcmdump";
    argc = 1;
	
    cout << "CmdLineArgs-> ";
    cin.getline(buf, bufsize);

    istringstream is(buf);
    
    arg[0] = '\0';
    while (is.good()) {
        is >> arg;
        if (strlen(arg) > 0) {
            argv[argc] = new char[strlen(arg)+1];
            strcpy(argv[argc], arg);
            argc++;
        }
        arg[0] = '\0';
    }
	
}

#else

void prepareCmdLineArgs(int& /* argc */, char** /* argv */)
{
    /* do nothing */
}

#endif


/*
** CVS/RCS Log:
** $Log: cmdlnarg.cc,v $
** Revision 1.1  1996-03-12 15:08:06  hewett
** Main program argc, argv handling for environments which do not make
** argc,argv available in main().
**
**
**
*/

