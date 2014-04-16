/*
 *
 *  Copyright (C) 1997-2014, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Test code of double/ascii conversion methods in class OFStandard
 *
 *
 */

#include "dcmtk/config/osconfig.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofstd.h"

struct tuple
{
  double val;
  int flags;
  int width;
  int prec;
  const char *output;

  tuple(const tuple &old)
  : val(old.val)
  , flags(old.flags)
  , width(old.width)
  , prec(old.prec)
  , output(old.output)
  {
  }

  tuple(double v, int f, int w, int p, const char *o)
  : val(v)
  , flags(f)
  , width(w)
  , prec(p)
  , output(o)
  {
  }

private:

  /// private undefined assignment operator
  tuple &operator=(const tuple &obj);
};


static const tuple values[] =
{
  tuple(12345.6789, 0, 0, -1,                                                        "12345.7" ),
  tuple(12345.6789, 0, 20, -1,                                                       "             12345.7" ),
  tuple(12345.6789, 0, 20, 10,                                                       "          12345.6789" ),
  tuple(12345.6789, 0, 20, 0,                                                        "               1e+04" ),
  tuple(12345.6789, OFStandard::ftoa_zeropad, 20, 10,                                "000000000012345.6789"),
  tuple(12345.6789, OFStandard::ftoa_leftadj, 20, 10,                                "12345.6789          " ),
  tuple(12345.6789, OFStandard::ftoa_alternate, 0, -1,                               "12345.7" ),
  tuple(12345.6789, OFStandard::ftoa_alternate, 20, -1,                              "             12345.7" ),
  tuple(12345.6789, OFStandard::ftoa_alternate, 20, 10,                              "         12345.67890" ),
  tuple(12345.6789, OFStandard::ftoa_alternate, 20, 0,                               "              1.e+04" ),
  tuple(12345.6789, OFStandard::ftoa_format_e, 0, -1,                                "1.234568e+04" ),
  tuple(12345.6789, OFStandard::ftoa_format_e, 20, -1,                               "        1.234568e+04" ),
  tuple(12345.6789, OFStandard::ftoa_format_e, 20, 10,                               "    1.2345678900e+04" ),
  tuple(12345.6789, OFStandard::ftoa_format_e, 20, 0,                                "               1e+04" ),
  tuple(12345.6789, OFStandard::ftoa_format_f, 0, -1,                                "12345.678900" ),
  tuple(12345.6789, OFStandard::ftoa_format_f, 20, -1,                               "        12345.678900" ),
  tuple(12345.6789, OFStandard::ftoa_format_f, 20, 10,                               "    12345.6789000000" ),
  tuple(12345.6789, OFStandard::ftoa_format_f, 20, 0,                                "               12346" ),
  tuple(12345.6789, OFStandard::ftoa_uppercase, 0, -1,                               "12345.7" ),
  tuple(12345.6789, OFStandard::ftoa_uppercase, 20, -1,                              "             12345.7" ),
  tuple(12345.6789, OFStandard::ftoa_uppercase, 20, 10,                              "          12345.6789" ),
  tuple(12345.6789, OFStandard::ftoa_uppercase, 20, 0,                               "               1E+04" ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 0, -1,   "1.234568E+04" ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, -1,  "        1.234568E+04" ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 10,  "    1.2345678900E+04" ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 0,   "               1E+04" ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 0, -1,   "12345.678900" ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, -1,  "        12345.678900" ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 10,  "    12345.6789000000" ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 0,   "               12346" ),
  tuple(1.23456789E89, 0, 0, -1,                                                        "1.23457e+89"                                       ),
  tuple(1.23456789E89, 0, 20, -1,                                                       "         1.23457e+89"                              ),
  tuple(1.23456789E89, 0, 20, 10,                                                       "      1.23456789e+89"                              ),
  tuple(1.23456789E89, 0, 20, 0,                                                        "               1e+89"                              ),
  tuple(1.23456789E89, OFStandard::ftoa_zeropad, 20, 10,                                "0000001.23456789e+89"                              ),
  tuple(1.23456789E89, OFStandard::ftoa_leftadj, 20, 10,                                "1.23456789e+89      "                              ),
  tuple(1.23456789E89, OFStandard::ftoa_alternate, 0, -1,                               "1.23457e+89"                                       ),
  tuple(1.23456789E89, OFStandard::ftoa_alternate, 20, -1,                              "         1.23457e+89"                              ),
  tuple(1.23456789E89, OFStandard::ftoa_alternate, 20, 10,                              "     1.234567890e+89"                              ),
  tuple(1.23456789E89, OFStandard::ftoa_alternate, 20, 0,                               "              1.e+89"                              ),
  tuple(1.23456789E89, OFStandard::ftoa_format_e, 0, -1,                                "1.234568e+89"                                      ),
  tuple(1.23456789E89, OFStandard::ftoa_format_e, 20, -1,                               "        1.234568e+89"                              ),
  tuple(1.23456789E89, OFStandard::ftoa_format_e, 20, 10,                               "    1.2345678900e+89"                              ),
  tuple(1.23456789E89, OFStandard::ftoa_format_e, 20, 0,                                "               1e+89"                              ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase, 0, -1,                               "1.23457E+89"                                       ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase, 20, -1,                              "         1.23457E+89"                              ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase, 20, 10,                              "      1.23456789E+89"                              ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase, 20, 0,                               "               1E+89"                              ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 0, -1,   "1.234568E+89"                                      ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, -1,  "        1.234568E+89"                              ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 10,  "    1.2345678900E+89"                              ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 0,   "               1E+89"                              ),

  // these tests fail on Solaris 7 when compiling with -DDISABLE_OFSTD_FTOA because of different rounding.
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 0, -1,                                "1234567890000000500000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 20, -1,                               "1234567890000000500000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 20, 10,                               "1234567890000000500000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 20, 0,                                "1234567890000000500000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 0, -1,   "1234567890000000500000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, -1,  "1234567890000000500000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 10,  "1234567890000000500000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 0,   "1234567890000000500000000000000000000000000000000" )

  /* this is what Solaris actually delivers when compiled with -DDISABLE_OFSTD_FTOA
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 0, -1,                                "1234567890000000012898796547947496515996930375163" ),
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 20, -1,                               "1234567890000000012898796547947496515996930375163" ),
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 20, 10,                               "1234567890000000012898796547947496515996930375163" ),
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 20, 0,                                "1234567890000000012898796547947496515996930375163" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 0, -1,   "1234567890000000012898796547947496515996930375163" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, -1,  "1234567890000000012898796547947496515996930375163" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 10,  "1234567890000000012898796547947496515996930375163" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 0,   "1234567890000000012898796547947496515996930375163" )
  */
};

OFTEST(ofstd_ftoa)
{
  char buf[50];
  size_t numTuples = sizeof(values)/sizeof(tuple);
  OFString s;

  for (size_t i=0; i<numTuples; i++)
  {
    OFStandard::ftoa(buf, 50, values[i].val, values[i].flags, values[i].width, values[i].prec);
    s = values[i].output;

    OFCHECK_EQUAL(s, buf);
  }
}
