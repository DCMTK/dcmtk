/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Purpose: test programm for class OfCommandLine
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 12:41:07 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/tests/Attic/tstcmdln.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include <iostream.h>
#include "ofcmdln.h"


int main(int argc, char *argv[])
{
    OFCommandLine cmd;

    cmd.addGroup("Options:");
    cmd.addOption("--help", "-h", "this help");
    cmd.addSubGroup("Input:");
    cmd.addOption("+Frame", "+F", 1, "n", "select frame n");
    cmd.addSubGroup("Output:");
    cmd.addOption("+Scale", "+S", 2, "x y", "scale image");
    cmd.addOption("+Option", "some option");

    switch (cmd.parseLine(argc, argv))    
    {
        case OFCommandLine::PS_UnknownOption:
            break;
        case OFCommandLine::PS_MissingValue:
            break;
        case OFCommandLine::PS_Normal:
            if (cmd.findOption("--help"))
            {
                cout << "usage: tstcmdln [options] ... [input file] ...[options] ... [output file] ... [options] ..." << endl;
                OFString str;
                cmd.getOptionString(str);
                cout << str;
            } else {
                if (cmd.findOption("+Frame", 1))
                {
                    OFCmdSignedInt frame;
                    if (cmd.getValue(frame, 1, 10) == OFCommandLine::VS_Normal)
                        cout << "frame: " << frame << endl;
                    else
                        cout << "invalid frame value (" << frame << ")" << endl;
                }
                if (cmd.findOption("+Scale", 2))
                {
                    OFCmdFloat xscale;
                    OFCmdFloat yscale;
                    if (cmd.getValue(xscale, 0.0, 10.0) == OFCommandLine::VS_Normal)
                        cout << "xscale: " << xscale << endl;
                    else
                        cout << "invalid xscale value (" << xscale << ")" << endl;
                    if (cmd.getValue(yscale, 0.0, 10.0) == OFCommandLine::VS_Normal)
                        cout << "yscale: " << yscale << endl;
                    else
                        cout << "invalid yscale value (" << yscale << ")" << endl;
                }
                if (cmd.findOption("+Option"))
                {
                    cout << "some option" << endl;
                }
                OFCmdString in_file;
                OFCmdString out_file;
                if (cmd.getParam(1, in_file))
                {
                    cout << "input: " << in_file << endl;
                    if (cmd.getParam(2, out_file))
                        cout << "output: " << out_file << endl;
                    else
                        cout << "no output file specified" << endl;
                }
                else
                    cout << "no input file specified" << endl;
            }
            break;
        default:
            break;
    }
    
    return 0;
}


/*
**
** CVS/RCS Log:
** $Log: tstcmdln.cc,v $
** Revision 1.1  1998-11-27 12:41:07  joergr
** Added class to handle command line arguments.
**
**
**
*/
