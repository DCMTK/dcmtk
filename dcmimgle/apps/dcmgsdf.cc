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
 *  Module:  dcmimgle
 *
 *  Authors: Joerg Riesmeier
 *
 *  Purpose: export display curves to a text file
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-02-23 16:54:30 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/apps/Attic/dcmgsdf.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */
 

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "didispfn.h"


int main(int argc, char *argv[])
{
    if (argc == 3)
    {
        DiDisplayFunction disp(argv[1]); 
        if (disp.isValid())
            return disp.writeCurveData(argv[2]);                                    // return code
    } else
        cerr << "ERROR: program needs exactly two filenames for input and output !" << endl;
    return 1;                                                                       // an error has happened
}


/*
 * CVS/RCS Log:
 * $Log: dcmgsdf.cc,v $
 * Revision 1.1  1999-02-23 16:54:30  joergr
 * Added tool to export display curves to a text file.
 *
 *
 */
