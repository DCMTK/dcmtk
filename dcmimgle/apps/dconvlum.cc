/*
 *
 *  Copyright (C) 1999-2019, OFFIS e.V.
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
 *  Purpose: convert VeriLUM CCx_xx.dat files to DCMTK display files
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/dcmdata/dcuid.h"    /* for dcmtk version name */


#define OFFIS_CONSOLE_APPLICATION "dconvlum"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


int main(int argc, char *argv[])
{
    /* check number of arguments */
    if ((argc >= 3) && (argc <= 4))
    {
#if defined(HAVE_IOS_NOCREATE) && !defined(HAVE_CXX11)
        STD_NAMESPACE ifstream input(argv[1], STD_NAMESPACE ios::in | STD_NAMESPACE ios::nocreate);
#else
        STD_NAMESPACE ifstream input(argv[1], STD_NAMESPACE ios::in);
#endif
        if (input)
        {
            STD_NAMESPACE ofstream output(argv[2]);
            if (output)
            {
                const unsigned int maxddl = 255;                                    // maximum DDL value (8 bits)
                output << "# Monitor characteristic file (converted from VeriLUM to DCMTK)" << OFendl << "# ";
                char c;
                while (input.get(c) && (c != '\n') && (c != '\r'))                  // read description
                    output << c;                                                    // ... and write it to output file
                output << OFendl << OFendl;
                output << "# maximum DDL value" << OFendl << OFendl;
                output << "  max   " << maxddl << OFendl << OFendl;
                if (argc == 4)
                {
                    double ambient = OFStandard::atof(argv[3]);
                    output << "# ambient light value" << OFendl << OFendl;
                    output << "  amb   " << ambient << OFendl << OFendl;
                }
                output << "# DDL   LumVal" << OFendl << OFendl;
                double lum;
                unsigned int ddl;
                unsigned int i = 0;
                while ((i <= maxddl) && input.good() && output.good())
                {
                    while (input.get(c) && !isdigit(OFstatic_cast(unsigned char, c))); // skip non-numeric chars
                    input.putback(c);
                    input >> ddl;                                                   // read DDL value
                    while (input.get(c) && !isdigit(OFstatic_cast(unsigned char, c))); // skip non-numeric chars
                    input.putback(c);
                    input >> lum;                                                   // read luminance value
                    output.width(5);
                    output << ddl;                                                  // write DDL value
                    output.width(12);
                    output.setf(STD_NAMESPACE ios::fixed, STD_NAMESPACE ios::floatfield);
                    output << lum << OFendl;                                          // write luminance value
                    ++i;
                }
                if (i <= maxddl)
                    CERR << "ERROR: can't convert input file ... error in line #" << i << OFendl;
                else {
                    output << OFendl << "# eof of file";
                    return 0;                                                       // everything is OK
                }
             } else
                CERR << "ERROR: can't create output file" << OFendl;
        } else
            CERR << "ERROR: can't open input file" << OFendl;
    } else {
        /* print usage */
        CERR << rcsid << OFendl << OFendl;
        CERR << OFFIS_CONSOLE_APPLICATION << ": Convert VeriLUM \"CCx_xx.dat\" files to DCMTK display files" << OFendl;
        CERR << "usage: " << OFFIS_CONSOLE_APPLICATION << " in-file out-file [ambient]" << OFendl << OFendl;
        CERR << "parameters:" << OFendl;
        CERR << "  in-file   VeriLUM characteristic curve file to be converted" << OFendl;
        CERR << "  out-file  DCMTK display file to be written" << OFendl;
        CERR << "  ambient   ambient light (cd/m^2, floating point value)" << OFendl;
    }
    return 1;                                                                       // an error has happened
}
