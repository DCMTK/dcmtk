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
 *  Module:  dcmimgle
 *
 *  Authors: Joerg Riesmeier
 *
 *  Purpose: convert VeriLUM CCx_xx.dat files to DCMTK display files
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-04-16 13:52:55 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/apps/dconvlum.cc,v $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */
 

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

BEGIN_EXTERN_C
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
END_EXTERN_C

#include "ofstream.h"
#include "ofconsol.h"


int main(int argc, char *argv[])
{
    if ((argc >= 3) && (argc <= 4))
    {
#ifdef HAVE_IOS_NOCREATE
        ifstream input(argv[1], ios::in|ios::nocreate);
#else
        ifstream input(argv[1], ios::in);
#endif
        if (input)
        {
            ofstream output(argv[2]);
            if (output)
            {
                const unsigned int maxddl = 255;                                    // maximum DDL value (8 bits)
                output << "# Monitor characteristic file (converted from VeriLUM to DCMTK)" << endl << "# ";
                char c;
                while (input.get(c) && (c != '\n') && (c != '\r'))                  // read description
                    output << c;                                                    // ... and write it to output file
                output << endl << endl;
                output << "# maximum DDL value" << endl << endl;
                output << "  max   " << maxddl << endl << endl;
                if (argc == 4)
                {
                    double ambient = atof(argv[3]);
                    output << "# ambient light value" << endl << endl;
                    output << "  amb   " << ambient << endl << endl;                    
                }
                output << "# DDL   LumVal" << endl << endl;
                double lum;
                unsigned int ddl;
                unsigned int i = 0;
                while ((i <= maxddl) && input.good() && output.good())
                {
                    while (input.get(c) && !isdigit(c));                            // skip non-numeric chars
                    input.putback(c);
                    input >> ddl;                                                   // read DDL value
                    while (input.get(c) && !isdigit(c));                            // skip non-numeric chars
                    input.putback(c);
                    input >> lum;                                                   // read luminance value
                    output.width(5);
                    output << ddl;                                                  // write DDL value
                    output.width(12);
                    output.setf(ios::fixed, ios::floatfield);
                    output << lum << endl;                                          // write luminance value
                    i++;
                }
                if (i <= maxddl)
                    CERR << "ERROR: can't convert input file ... error in line #" << i << " !" << endl;
                else {
                    output << endl << "# eof of file";
                    return 0;                                                       // everything is OK
                }
             } else
                CERR << "ERROR: can't create output file !" << endl;
        } else
            CERR << "ERROR: can't open input file !" << endl;
    } else {
        CERR << "ERROR: program needs exactly two filenames for input and output" << endl;
        CERR << "       (and an optional floating point value for the ambient light) !" << endl;
    }
    return 1;                                                                       // an error has happened
}


/*
 * CVS/RCS Log:
 * $Log: dconvlum.cc,v $
 * Revision 1.13  2002-04-16 13:52:55  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 * Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
 * contribution.
 *
 * Revision 1.12  2001/09/28 12:55:53  joergr
 * Added check whether ios::nocreate exists.
 *
 * Revision 1.11  2001/06/01 15:49:38  meichel
 * Updated copyright header
 *
 * Revision 1.10  2000/03/08 16:24:10  meichel
 * Updated copyright header.
 *
 * Revision 1.9  2000/03/03 14:09:08  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.8  2000/02/23 15:12:13  meichel
 * Corrected macro for Borland C++ Builder 4 workaround.
 *
 * Revision 1.7  2000/02/01 10:52:34  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.6  1999/04/28 14:44:11  joergr
 * Added newline to error output.
 *
 * Revision 1.5  1999/03/03 11:41:53  joergr
 * Added support to specify ambient light value (re: Barten transformation).
 *
 * Revision 1.4  1999/02/23 16:54:10  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.3  1999/02/11 16:55:23  joergr
 * Added mode ios::nocreate when opening file streams for reading to avoid
 * implicit creation of non-existing files.
 *
 * Revision 1.2  1999/02/08 12:35:23  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.1  1999/02/05 15:16:16  joergr
 * Added console application to convert VeriLUM LUT files to dcmtk DISPLAY
 * file used for Barten transformation.
 *
 *
 */
