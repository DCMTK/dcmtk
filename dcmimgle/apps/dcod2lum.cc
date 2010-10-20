/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Authors: Joerg Riesmeier
 *
 *  Purpose: convert hardcopy characteristic curve file to softcopy format
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-10-20 07:41:35 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CCTYPE
#define INCLUDE_CSTRING
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
        STD_NAMESPACE ifstream input(argv[1], STD_NAMESPACE ios::in | STD_NAMESPACE ios::nocreate);
#else
        STD_NAMESPACE ifstream input(argv[1], STD_NAMESPACE ios::in);
#endif
        if (input)
        {
            STD_NAMESPACE ofstream output(argv[2]);
            if (output)
            {
                char c;
                int i = 0;
                int maxddl = 0;
                int order = -1;
                double amb = 0;
                double lum = 0;
                output << "# Softcopy characteristic curve file (converted from hardcopy format)" << OFendl;
                while (input.get(c) && !error)
                {
                    if (c == '#')                                               // comment character
                    {
                        while (input.get(c) && (c != '\n') && (c != '\r'));     // skip comments
                    }
                    else if (!isspace(OFstatic_cast(unsigned char, c)))         // skip whitespaces
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
                                    output << OFendl << "# maximum DDL value" << OFendl;
                                    output << "max " << maxddl << OFendl;
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
                                    output << OFendl << "# reflected ambient light (already included)" << OFendl;
                                    output << "# amb " << amb << OFendl;
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
                                    output << OFendl << "# illumination (already included)" << OFendl;
                                    output << "# lum " << lum << OFendl;
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
                                    output << OFendl << "# order of the polynomial curve fitting algorithm (optional)" << OFendl;
                                    output << "ord " << order << OFendl;
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
                                    output << OFendl << "# DDL   LumVal" << OFendl;
                                output << ddlval << "\t";
                                output.setf(STD_NAMESPACE ios::fixed, STD_NAMESPACE ios::floatfield);
                                output << DiDisplayFunction::convertODtoLum(odval, amb, lum) << OFendl;
                                ++i;
                            } else
                                error = 1;
                        }
                    }
                }
                if (error)
                {
                    output << OFendl << "# error: corrupted input file" << OFendl;
                    CERR << "ERROR: corrupted input file !" << OFendl;
                } else
                    output << OFendl << "# end of file" << OFendl;
             } else
                CERR << "ERROR: can't create output file !" << OFendl;
        } else
            CERR << "ERROR: can't open input file !" << OFendl;
    } else {
        /* print usage */
        CERR << rcsid << OFendl << OFendl;
        CERR << OFFIS_CONSOLE_APPLICATION << ": Convert hardcopy characteristic curve file to softcopy format" << OFendl;
        CERR << "usage: " << OFFIS_CONSOLE_APPLICATION << " in-file out-file" << OFendl << OFendl;
        CERR << "parameters:" << OFendl;
        CERR << "  in-file   hardcopy characteristic curve file to be converted" << OFendl;
        CERR << "  out-file  softcopy characteristic curve file to be written" << OFendl;
    }
    return error;
}


/*
 * CVS/RCS Log:
 * $Log: dcod2lum.cc,v $
 * Revision 1.10  2010-10-20 07:41:35  uli
 * Made sure isalpha() & friends are only called with valid arguments.
 *
 * Revision 1.9  2010-10-14 13:13:36  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.8  2010-06-25 09:15:19  uli
 * Fixed issues with compiling with HAVE_STD_STRING.
 *
 * Revision 1.7  2006-08-15 16:30:09  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.6  2006/07/27 14:01:23  joergr
 * Updated copyright date.
 *
 * Revision 1.5  2005/12/08 15:42:40  meichel
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
