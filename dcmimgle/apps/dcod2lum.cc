/*
 *
 *  Copyright (C) 2002-2005, OFFIS
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
 *  Purpose: convert hardcopy characteristic curve file to softcopy format
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-12-08 15:42:40 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/dcmimgle/didispfn.h"
#include "dcmtk/dcmdata/dcuid.h"    /* for dcmtk version name */


#define OFFIS_CONSOLE_APPLICATION "dcod2lum"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


int main(int argc, char *argv[])
{
    int error = 0;
    /* check number of arguments */
    if (argc == 3)
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
                char c;
                int i = 0;
                int maxddl = 0;
                int order = -1;
                double amb = 0;
                double lum = 0;
                output << "# Softcopy characteristic curve file (converted from hardcopy format)" << endl;
                while (input.get(c) && !error)
                {
                    if (c == '#')                                               // comment character
                    {
                        while (input.get(c) && (c != '\n') && (c != '\r'));     // skip comments
                    }
                    else if (!isspace(c))                                       // skip whitespaces
                    {
                        input.putback(c);
                        if (maxddl == 0)                                        // read maxvalue
                        {
                            char str[4];
                            input.get(str, sizeof(str));
                            if (strcmp(str, "max") == 0)                        // check for key word: max
                            {
                                input >> maxddl;
                                if (maxddl > 0)
                                {
                                    output << endl << "# maximum DDL value" << endl;
                                    output << "max " << maxddl << endl;
                                } else
                                    error = 1;
                            } else
                                error = 1;
                        }
                        else if ((amb <= 0) && (c == 'a'))                      // read ambient light value (optional)
                        {
                            char str[4];
                            input.get(str, sizeof(str));
                            if (strcmp(str, "amb") == 0)                        // check for key word: amb
                            {
                                input >> amb;
                                if (amb >= 0)
                                {
                                    output << endl << "# reflected ambient light (already included)" << endl;
                                    output << "# amb " << amb << endl;
                                } else
                                    error = 1;
                            } else
                                error = 1;
                        }
                        else if ((lum <= 0) && (c == 'l'))                     // read illumination value (optional)
                        {
                            char str[4];
                            input.get(str, sizeof(str));
                            if (strcmp(str, "lum") == 0)                       // check for key word: lum
                            {
                                input >> lum;
                                if (lum >= 0)
                                {
                                    output << endl << "# illumination (already included)" << endl;
                                    output << "# lum " << lum << endl;
                                } else
                                    error = 1;
                            } else
                                error = 1;
                        }
                        else if ((order < 0) && (c == 'o'))                     // read polynomial order (optional)
                        {
                            char str[4];
                            input.get(str, sizeof(str));
                            if (strcmp(str, "ord") == 0)                        // check for key word: ord
                            {
                                input >> order;
                                if (order >= 0)
                                {
                                    output << endl << "# order of the polynomial curve fitting algorithm (optional)" << endl;
                                    output << "ord " << order << endl;
                                } else
                                    error = 1;
                            } else
                                error = 1;
                        } else {
                            int ddlval;
                            double odval;
                            input >> ddlval;                                    // read DDL value
                            input >> odval;                                     // read OD value
                            if (input.good() && (ddlval <= maxddl))
                            {
                                if (i == 0)
                                    output << endl << "# DDL   LumVal" << endl;
                                output << ddlval << "\t";
                                output.setf(ios::fixed, ios::floatfield);
                                output << DiDisplayFunction::convertODtoLum(odval, amb, lum) << endl;
                                ++i;
                            } else
                                error = 1;
                        }
                    }
                }
                if (error)
                {
                    output << endl << "# error: corrupted input file" << endl;
                    CERR << "ERROR: corrupted input file !" << endl;
                } else
                    output << endl << "# end of file" << endl;
             } else
                CERR << "ERROR: can't create output file !" << endl;
        } else
            CERR << "ERROR: can't open input file !" << endl;
    } else {
        /* print usage */
        CERR << rcsid << endl << endl;
        CERR << OFFIS_CONSOLE_APPLICATION << ": Convert hardcopy characteristic curve file to softcopy format" << endl;
        CERR << "usage: " << OFFIS_CONSOLE_APPLICATION << " in-file out-file" << endl << endl;
        CERR << "parameters:" << endl;
        CERR << "  in-file   hardcopy characteristic curve file to be converted" << endl;
        CERR << "  out-file  softcopy characteristic curve file to be written" << endl;
    }
    return error;
}


/*
 * CVS/RCS Log:
 * $Log: dcod2lum.cc,v $
 * Revision 1.5  2005-12-08 15:42:40  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.4  2003/12/23 15:40:13  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.3  2003/12/08 19:24:03  joergr
 * Updated copyright header.
 *
 * Revision 1.2  2002/11/27 14:08:00  meichel
 * Adapted module dcmimgle to use of new header file ofstdinc.h
 *
 * Revision 1.1  2002/07/18 12:23:54  joergr
 * Added command line tool to convert hardcopy to softcopy display files.
 *
 *
 *
 */
