/*
 *
 *  Copyright (C) 1999, OFFIS
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
 *  Module:  ofstd
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Handle console applications (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-02-08 12:00:14 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/libsrc/ofconapp.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "ofconapp.h"
#include "ofstring.h"



/*------------------*
 *  implementation  *
 *------------------*/

OFConsoleApplication::OFConsoleApplication(const char *app,
                                           const char *desc,
                                           const char *rcsid,
                                           ostream *output)
 : Name(app),
   Description(desc),
   Identification(rcsid),
   Output((output != NULL) ? output : &cerr),
   CmdLine(NULL)
{
}


OFConsoleApplication::~OFConsoleApplication()
{
}


OFBool OFConsoleApplication::parseCommandLine(OFCommandLine &cmd,
                                              int argCount,
                                              char *argValue[],
                                              const char *parmDesc,
                                              const int minCount,
                                              const int maxCount,
                                              const int flags,
                                              const int startPos)
{
    CmdLine = &cmd;                                                     // store reference to cmdline object
    OFString str;
    switch (cmd.parseLine(argCount, argValue, flags, startPos))    
    {
        case OFCommandLine::PS_NoArguments:
            printUsage(parmDesc);
            break;
        case OFCommandLine::PS_UnknownOption:
            cmd.getStatusString(OFCommandLine::PS_UnknownOption, str);
            printError(str.c_str());
            break;
        case OFCommandLine::PS_MissingValue:
            cmd.getStatusString(OFCommandLine::PS_MissingValue, str);
            printError(str.c_str());
            break;
        case OFCommandLine::PS_Normal:
            if ((cmd.getArgCount() == 1) && cmd.findOption("--help"))
                printUsage(parmDesc);
            else if (cmd.getParamCount() < minCount)
                printError("Missing arguments");
            else if ((maxCount >= 0) && (cmd.getParamCount() > maxCount))
                printError("Too many arguments");
            else 
                return OFTrue;
    }
    return OFFalse;
}


void OFConsoleApplication::printHeader()
{
    if (!Identification.empty())
        (*Output) << Identification << endl << endl;
    (*Output) << Name;
    if (!Description.empty())
        (*Output) << ": " << Description;
    (*Output) << endl;
}


void OFConsoleApplication::printUsage(const char *parm,
                                      const OFCommandLine *cmd)
{
    if (cmd == NULL)
        cmd = CmdLine;
    printHeader();
    (*Output) << "usage: " << Name << " [options] " << parm << endl << endl;
    if (cmd != NULL)
    {
        OFString str;
        cmd->getOptionString(str);
        (*Output) << str << endl;
    }
    exit(0);
}


void OFConsoleApplication::printError(const char *str)
{
    printHeader();
    (*Output) << "error: " << str << endl;
    exit(1);
}


void OFConsoleApplication::checkValue(const OFCommandLine::E_ValueStatus status,
                                      OFCommandLine *cmd)
{
    if (cmd == NULL)
        cmd = CmdLine;
    if (status != OFCommandLine::VS_Normal)
    {
        OFString str;
        if (cmd != NULL)
            cmd->getStatusString(status, str);
        printError(str.c_str());
    }
}


/*
 *
 * CVS/RCS Log:
 * $Log: ofconapp.cc,v $
 * Revision 1.1  1999-02-08 12:00:14  joergr
 * Added class to handle console applications (with or w/o command line
 * arguments).
 *
 *
 *
 */
