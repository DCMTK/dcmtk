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
 *  Purpose: convert VeriLUM CCx_xx.dat files to DCMTK display files
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-04-28 14:44:11 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/apps/dconvlum.cc,v $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */
 

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <iostream.h>
#include <fstream.h>

#ifdef HAVE_STDLIB_H
 #include <stdlib.h>
#endif

BEGIN_EXTERN_C
 #ifdef HAVE_CTYPE_H
  #include <ctype.h>
 #endif
END_EXTERN_C


int main(int argc, char *argv[])
{
    if ((argc >= 3) && (argc <= 4))
    {
        ifstream input(argv[1], ios::in|ios::nocreate);
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
                    cerr << "ERROR: can't convert input file ... error in line #" << i << " !" << endl;
                else {
                    output << endl << "# eof of file";
                    return 0;                                                       // everything is OK
                }
             } else
                cerr << "ERROR: can't create output file !" << endl;
        } else
            cerr << "ERROR: can't open input file !" << endl;
    } else {
        cerr << "ERROR: program needs exactly two filenames for input and output" << endl;
        cerr << "       (and an optional floating point value for the ambient light) !" << endl;
    }
    return 1;                                                                       // an error has happened
}


/*
 * CVS/RCS Log:
 * $Log: dconvlum.cc,v $
 * Revision 1.6  1999-04-28 14:44:11  joergr
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
