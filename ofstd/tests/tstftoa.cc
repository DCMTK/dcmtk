/*
 *
 *  Copyright (C) 1997-2002, OFFIS
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Test code of double/ascii conversion methods in class OFStandard
 *
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-12-04 09:13:04 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/tests/tstftoa.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "ofstd.h"
#include "oftypes.h"
#include "ofconsol.h"

struct tuple
{
  double val;
  int flags;
  int width;
  int prec;
  const char *output;
};

const tuple values[] =
{
  { 12345.6789, 0, 0, -1,                                                        "12345.7" },
  { 12345.6789, 0, 20, -1,                                                       "             12345.7" },
  { 12345.6789, 0, 20, 10,                                                       "          12345.6789" },
  { 12345.6789, 0, 20, 0,                                                        "               1e+04" },
  { 12345.6789, OFStandard::ftoa_zeropad, 20, 10,                                "000000000012345.6789" },
  { 12345.6789, OFStandard::ftoa_leftadj, 20, 10,                                "12345.6789          " },
  { 12345.6789, OFStandard::ftoa_alternate, 0, -1,                               "12345.7" },
  { 12345.6789, OFStandard::ftoa_alternate, 20, -1,                              "             12345.7" },
  { 12345.6789, OFStandard::ftoa_alternate, 20, 10,                              "         12345.67890" },
  { 12345.6789, OFStandard::ftoa_alternate, 20, 0,                               "              1.e+04" },
  { 12345.6789, OFStandard::ftoa_format_e, 0, -1,                                "1.234568e+04" },
  { 12345.6789, OFStandard::ftoa_format_e, 20, -1,                               "        1.234568e+04" },
  { 12345.6789, OFStandard::ftoa_format_e, 20, 10,                               "    1.2345678900e+04" },
  { 12345.6789, OFStandard::ftoa_format_e, 20, 0,                                "               1e+04" },
  { 12345.6789, OFStandard::ftoa_format_f, 0, -1,                                "12345.678900" },
  { 12345.6789, OFStandard::ftoa_format_f, 20, -1,                               "        12345.678900" },
  { 12345.6789, OFStandard::ftoa_format_f, 20, 10,                               "    12345.6789000000" },
  { 12345.6789, OFStandard::ftoa_format_f, 20, 0,                                "               12346" },
  { 12345.6789, OFStandard::ftoa_uppercase, 0, -1,                               "12345.7" },
  { 12345.6789, OFStandard::ftoa_uppercase, 20, -1,                              "             12345.7" },
  { 12345.6789, OFStandard::ftoa_uppercase, 20, 10,                              "          12345.6789" },
  { 12345.6789, OFStandard::ftoa_uppercase, 20, 0,                               "               1E+04" },
  { 12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 0, -1,   "1.234568E+04" },
  { 12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, -1,  "        1.234568E+04" },
  { 12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 10,  "    1.2345678900E+04" },
  { 12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 0,   "               1E+04" },
  { 12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 0, -1,   "12345.678900" },
  { 12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, -1,  "        12345.678900" },
  { 12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 10,  "    12345.6789000000" },
  { 12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 0,   "               12346" },

  { 1.23456789E89, 0, 0, -1,                                                        "1.23457e+89"                                       },
  { 1.23456789E89, 0, 20, -1,                                                       "         1.23457e+89"                              },
  { 1.23456789E89, 0, 20, 10,                                                       "      1.23456789e+89"                              },
  { 1.23456789E89, 0, 20, 0,                                                        "               1e+89"                              },
  { 1.23456789E89, OFStandard::ftoa_zeropad, 20, 10,                                "0000001.23456789e+89"                              },
  { 1.23456789E89, OFStandard::ftoa_leftadj, 20, 10,                                "1.23456789e+89      "                              },
  { 1.23456789E89, OFStandard::ftoa_alternate, 0, -1,                               "1.23457e+89"                                       },
  { 1.23456789E89, OFStandard::ftoa_alternate, 20, -1,                              "         1.23457e+89"                              },
  { 1.23456789E89, OFStandard::ftoa_alternate, 20, 10,                              "     1.234567890e+89"                              },
  { 1.23456789E89, OFStandard::ftoa_alternate, 20, 0,                               "              1.e+89"                              },
  { 1.23456789E89, OFStandard::ftoa_format_e, 0, -1,                                "1.234568e+89"                                      },
  { 1.23456789E89, OFStandard::ftoa_format_e, 20, -1,                               "        1.234568e+89"                              },
  { 1.23456789E89, OFStandard::ftoa_format_e, 20, 10,                               "    1.2345678900e+89"                              },
  { 1.23456789E89, OFStandard::ftoa_format_e, 20, 0,                                "               1e+89"                              },
  { 1.23456789E89, OFStandard::ftoa_uppercase, 0, -1,                               "1.23457E+89"                                       },
  { 1.23456789E89, OFStandard::ftoa_uppercase, 20, -1,                              "         1.23457E+89"                              },
  { 1.23456789E89, OFStandard::ftoa_uppercase, 20, 10,                              "      1.23456789E+89"                              },
  { 1.23456789E89, OFStandard::ftoa_uppercase, 20, 0,                               "               1E+89"                              },
  { 1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 0, -1,   "1.234568E+89"                                      },
  { 1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, -1,  "        1.234568E+89"                              },
  { 1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 10,  "    1.2345678900E+89"                              },
  { 1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 0,   "               1E+89"                              },

  // these tests fail on Solaris 7 when compiling with -DDISABLE_OFSTD_FTOA because of different rounding.
  { 1.23456789E89, OFStandard::ftoa_format_f, 0, -1,                                "1234567890000000500000000000000000000000000000000" },
  { 1.23456789E89, OFStandard::ftoa_format_f, 20, -1,                               "1234567890000000500000000000000000000000000000000" },
  { 1.23456789E89, OFStandard::ftoa_format_f, 20, 10,                               "1234567890000000500000000000000000000000000000000" },
  { 1.23456789E89, OFStandard::ftoa_format_f, 20, 0,                                "1234567890000000500000000000000000000000000000000" },
  { 1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 0, -1,   "1234567890000000500000000000000000000000000000000" },
  { 1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, -1,  "1234567890000000500000000000000000000000000000000" },
  { 1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 10,  "1234567890000000500000000000000000000000000000000" },
  { 1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 0,   "1234567890000000500000000000000000000000000000000" }

  /* this is what Solaris actually delivers when compiled with -DDISABLE_OFSTD_FTOA
  { 1.23456789E89, OFStandard::ftoa_format_f, 0, -1,                                "1234567890000000012898796547947496515996930375163" },
  { 1.23456789E89, OFStandard::ftoa_format_f, 20, -1,                               "1234567890000000012898796547947496515996930375163" },
  { 1.23456789E89, OFStandard::ftoa_format_f, 20, 10,                               "1234567890000000012898796547947496515996930375163" },
  { 1.23456789E89, OFStandard::ftoa_format_f, 20, 0,                                "1234567890000000012898796547947496515996930375163" },
  { 1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 0, -1,   "1234567890000000012898796547947496515996930375163" },
  { 1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, -1,  "1234567890000000012898796547947496515996930375163" },
  { 1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 10,  "1234567890000000012898796547947496515996930375163" },
  { 1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 0,   "1234567890000000012898796547947496515996930375163" }
  */
};


int main()
{
  char buf[50];
  unsigned long numTuples = sizeof(values)/sizeof(tuple);
  unsigned long numPassed = 0;
  OFString s;

  for (unsigned long i=0; i<numTuples; i++)
  {
    OFStandard::ftoa(buf, 50, values[i].val, values[i].flags, values[i].width, values[i].prec); 
    s = values[i].output;

    if (s == buf)
    {
      CERR << "test #" << i+1 << " passed" << endl;
      numPassed++;
    }
    else
    {
      CERR << "test #" << i+1 << " failed, expected \"" << values[i].output << "\", got \"" << buf << "\"" << endl;
    }
  }

  if (numTuples == numPassed)
  {
    CERR << "All tests passed." << endl;
    return 0;
  }
  else
  {
    CERR << "Failures: " << numPassed << " of " << numTuples << " tests passed." << endl;
  }

  return 1;
}

/*
 *
 * CVS/RCS Log:
 * $Log: tstftoa.cc,v $
 * Revision 1.1  2002-12-04 09:13:04  meichel
 * Implemented a locale independent function OFStandard::ftoa() that
 *   converts double to string and offers all the flexibility of the
 *   sprintf family of functions.
 *
 *
 */
