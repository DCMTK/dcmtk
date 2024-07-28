/*
 *
 *  Copyright (C) 1997-2024, OFFIS e.V.
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
#include "dcmtk/ofstd/oftimer.h"
#include "dcmtk/ofstd/oflimits.h"
#include <clocale>

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
  tuple(OFnumeric_limits<double>::infinity(), 0, 0, -1,                                 "inf"                   ),
  tuple(-OFnumeric_limits<double>::infinity(), 0, 0, -1,                                "-inf"                  ),
  tuple(-OFnumeric_limits<double>:: quiet_NaN(), 0, 0, -1,                              "nan"                   ),
  tuple(12345.6789, 0, 0, -1,                                                           "12345.7"               ),
  tuple(12345.6789, 0, 20, -1,                                                          "             12345.7"  ),
  tuple(12345.6789, 0, 20, 10,                                                          "          12345.6789"  ),
  tuple(12345.6789, 0, 20, 0,                                                           "               1e+04"  ),
  tuple(12345.6789, OFStandard::ftoa_zeropad, 20, 10,                                   "000000000012345.6789"  ),
  tuple(12345.6789, OFStandard::ftoa_leftadj, 20, 10,                                   "12345.6789          "  ),
  tuple(12345.6789, OFStandard::ftoa_alternate, 0, -1,                                  "12345.7"               ),
  tuple(12345.6789, OFStandard::ftoa_alternate, 20, -1,                                 "             12345.7"  ),
  tuple(12345.6789, OFStandard::ftoa_alternate, 20, 10,                                 "         12345.67890"  ),
  tuple(12345.6789, OFStandard::ftoa_alternate, 20, 0,                                  "              1.e+04"  ),
  tuple(12345.6789, OFStandard::ftoa_format_e, 0, -1,                                   "1.234568e+04"          ),
  tuple(12345.6789, OFStandard::ftoa_format_e, 20, -1,                                  "        1.234568e+04"  ),
  tuple(12345.6789, OFStandard::ftoa_format_e, 20, 10,                                  "    1.2345678900e+04"  ),
  tuple(12345.6789, OFStandard::ftoa_format_e, 20, 0,                                   "               1e+04"  ),
  tuple(12345.6789, OFStandard::ftoa_format_f, 0, -1,                                   "12345.678900"          ),
  tuple(12345.6789, OFStandard::ftoa_format_f, 20, -1,                                  "        12345.678900"  ),
  tuple(12345.6789, OFStandard::ftoa_format_f, 20, 10,                                  "    12345.6789000000"  ),
  tuple(12345.6789, OFStandard::ftoa_format_f, 20, 0,                                   "               12346"  ),
  tuple(12345.6789, OFStandard::ftoa_uppercase, 0, -1,                                  "12345.7"               ),
  tuple(12345.6789, OFStandard::ftoa_uppercase, 20, -1,                                 "             12345.7"  ),
  tuple(12345.6789, OFStandard::ftoa_uppercase, 20, 10,                                 "          12345.6789"  ),
  tuple(12345.6789, OFStandard::ftoa_uppercase, 20, 0,                                  "               1E+04"  ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 0, -1,      "1.234568E+04"          ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, -1,     "        1.234568E+04"  ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 10,     "    1.2345678900E+04"  ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 0,      "               1E+04"  ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 0, -1,      "12345.678900"          ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, -1,     "        12345.678900"  ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 10,     "    12345.6789000000"  ),
  tuple(12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 0,      "               12346"  ),
  tuple(1.23456789E89, 0, 0, -1,                                                        "1.23457e+89"           ),
  tuple(1.23456789E89, 0, 20, -1,                                                       "         1.23457e+89"  ),
  tuple(1.23456789E89, 0, 20, 10,                                                       "      1.23456789e+89"  ),
  tuple(1.23456789E89, 0, 20, 0,                                                        "               1e+89"  ),
  tuple(1.23456789E89, OFStandard::ftoa_zeropad, 20, 10,                                "0000001.23456789e+89"  ),
  tuple(1.23456789E89, OFStandard::ftoa_leftadj, 20, 10,                                "1.23456789e+89      "  ),
  tuple(1.23456789E89, OFStandard::ftoa_alternate, 0, -1,                               "1.23457e+89"           ),
  tuple(1.23456789E89, OFStandard::ftoa_alternate, 20, -1,                              "         1.23457e+89"  ),
  tuple(1.23456789E89, OFStandard::ftoa_alternate, 20, 10,                              "     1.234567890e+89"  ),
  tuple(1.23456789E89, OFStandard::ftoa_alternate, 20, 0,                               "              1.e+89"  ),
  tuple(1.23456789E89, OFStandard::ftoa_format_e, 0, -1,                                "1.234568e+89"          ),
  tuple(1.23456789E89, OFStandard::ftoa_format_e, 20, -1,                               "        1.234568e+89"  ),
  tuple(1.23456789E89, OFStandard::ftoa_format_e, 20, 10,                               "    1.2345678900e+89"  ),
  tuple(1.23456789E89, OFStandard::ftoa_format_e, 20, 0,                                "               1e+89"  ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase, 0, -1,                               "1.23457E+89"           ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase, 20, -1,                              "         1.23457E+89"  ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase, 20, 10,                              "      1.23456789E+89"  ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase, 20, 0,                               "               1E+89"  ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 0, -1,   "1.234568E+89"          ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, -1,  "        1.234568E+89"  ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 10,  "    1.2345678900E+89"  ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 0,   "               1E+89"  ),
  tuple(-12345.6789, 0, 0, -1,                                                          "-12345.7"              ),
  tuple(-12345.6789, 0, 20, -1,                                                         "            -12345.7"  ),
  tuple(-12345.6789, 0, 20, 10,                                                         "         -12345.6789"  ),
  tuple(-12345.6789, 0, 20, 0,                                                          "              -1e+04"  ),
  tuple(-12345.6789, OFStandard::ftoa_zeropad, 20, 10,                                  "-00000000012345.6789"  ),
  tuple(-12345.6789, OFStandard::ftoa_leftadj, 20, 10,                                  "-12345.6789         "  ),
  tuple(-12345.6789, OFStandard::ftoa_alternate, 0, -1,                                 "-12345.7"              ),
  tuple(-12345.6789, OFStandard::ftoa_alternate, 20, -1,                                "            -12345.7"  ),
  tuple(-12345.6789, OFStandard::ftoa_alternate, 20, 10,                                "        -12345.67890"  ),
  tuple(-12345.6789, OFStandard::ftoa_alternate, 20, 0,                                 "             -1.e+04"  ),
  tuple(-12345.6789, OFStandard::ftoa_format_e, 0, -1,                                  "-1.234568e+04"         ),
  tuple(-12345.6789, OFStandard::ftoa_format_e, 20, -1,                                 "       -1.234568e+04"  ),
  tuple(-12345.6789, OFStandard::ftoa_format_e, 20, 10,                                 "   -1.2345678900e+04"  ),
  tuple(-12345.6789, OFStandard::ftoa_format_e, 20, 0,                                  "              -1e+04"  ),
  tuple(-12345.6789, OFStandard::ftoa_format_f, 0, -1,                                  "-12345.678900"         ),
  tuple(-12345.6789, OFStandard::ftoa_format_f, 20, -1,                                 "       -12345.678900"  ),
  tuple(-12345.6789, OFStandard::ftoa_format_f, 20, 10,                                 "   -12345.6789000000"  ),
  tuple(-12345.6789, OFStandard::ftoa_format_f, 20, 0,                                  "              -12346"  ),
  tuple(-12345.6789, OFStandard::ftoa_uppercase, 0, -1,                                 "-12345.7"              ),
  tuple(-12345.6789, OFStandard::ftoa_uppercase, 20, -1,                                "            -12345.7"  ),
  tuple(-12345.6789, OFStandard::ftoa_uppercase, 20, 10,                                "         -12345.6789"  ),
  tuple(-12345.6789, OFStandard::ftoa_uppercase, 20, 0,                                 "              -1E+04"  ),
  tuple(-12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 0, -1,     "-1.234568E+04"         ),
  tuple(-12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, -1,    "       -1.234568E+04"  ),
  tuple(-12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 10,    "   -1.2345678900E+04"  ),
  tuple(-12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 0,     "              -1E+04"  ),
  tuple(-12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 0, -1,     "-12345.678900"         ),
  tuple(-12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, -1,    "       -12345.678900"  ),
  tuple(-12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 10,    "   -12345.6789000000"  ),
  tuple(-12345.6789, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 0,     "              -12346"  ),
  tuple(-1.23456789E89, 0, 0, -1,                                                       "-1.23457e+89"          ),
  tuple(-1.23456789E89, 0, 20, -1,                                                      "        -1.23457e+89"  ),
  tuple(-1.23456789E89, 0, 20, 10,                                                      "     -1.23456789e+89"  ),
  tuple(-1.23456789E89, 0, 20, 0,                                                       "              -1e+89"  ),
  tuple(-1.23456789E89, OFStandard::ftoa_zeropad, 20, 10,                               "-000001.23456789e+89"  ),
  tuple(-1.23456789E89, OFStandard::ftoa_leftadj, 20, 10,                               "-1.23456789e+89     "  ),
  tuple(-1.23456789E89, OFStandard::ftoa_alternate, 0, -1,                              "-1.23457e+89"          ),
  tuple(-1.23456789E89, OFStandard::ftoa_alternate, 20, -1,                             "        -1.23457e+89"  ),
  tuple(-1.23456789E89, OFStandard::ftoa_alternate, 20, 10,                             "    -1.234567890e+89"  ),
  tuple(-1.23456789E89, OFStandard::ftoa_alternate, 20, 0,                              "             -1.e+89"  ),
  tuple(-1.23456789E89, OFStandard::ftoa_format_e, 0, -1,                               "-1.234568e+89"         ),
  tuple(-1.23456789E89, OFStandard::ftoa_format_e, 20, -1,                              "       -1.234568e+89"  ),
  tuple(-1.23456789E89, OFStandard::ftoa_format_e, 20, 10,                              "   -1.2345678900e+89"  ),
  tuple(-1.23456789E89, OFStandard::ftoa_format_e, 20, 0,                               "              -1e+89"  ),
  tuple(-1.23456789E89, OFStandard::ftoa_uppercase, 0, -1,                              "-1.23457E+89"          ),
  tuple(-1.23456789E89, OFStandard::ftoa_uppercase, 20, -1,                             "        -1.23457E+89"  ),
  tuple(-1.23456789E89, OFStandard::ftoa_uppercase, 20, 10,                             "     -1.23456789E+89"  ),
  tuple(-1.23456789E89, OFStandard::ftoa_uppercase, 20, 0,                              "              -1E+89"  ),
  tuple(-1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 0, -1,  "-1.234568E+89"         ),
  tuple(-1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, -1, "       -1.234568E+89"  ),
  tuple(-1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 10, "   -1.2345678900E+89"  ),
  tuple(-1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_e, 20, 0,  "              -1E+89"  ),
  tuple(0.1,     0, 0, 1,                                                               "0.1"                   ),
  tuple(0.1,     0, 0, 3,                                                               "0.1"                   ),
  tuple(0.1,     0, 0, 3,                                                               "0.1"                   ),
  tuple(0.1,     0, 0, 10,                                                              "0.1"                   ),
  tuple(0.01,    0, 0, 1,                                                               "0.01"                  ),
  tuple(0.01,    0, 0, 3,                                                               "0.01"                  ),
  tuple(0.01,    0, 0, 5,                                                               "0.01"                  ),
  tuple(0.01,    0, 0, 10,                                                              "0.01"                  ),
  tuple(0.01,    0, 0, 17,                                                              "0.01"                  ),
  tuple(0.001,   0, 0, 1,                                                               "0.001"                 ),
  tuple(0.001,   0, 0, 3,                                                               "0.001"                 ),
  tuple(0.001,   0, 0, 5,                                                               "0.001"                 ),
  tuple(0.001,   0, 0, 10,                                                              "0.001"                 ),
  tuple(0.001,   0, 0, 17,                                                              "0.001"                 ),
  tuple(0.0001,  0, 0, 1,                                                               "0.0001"                ),
  tuple(0.0001,  0, 0, 3,                                                               "0.0001"                ),
  tuple(0.0001,  0, 0, 5,                                                               "0.0001"                ),
  tuple(0.0001,  0, 0, 10,                                                              "0.0001"                ),
  tuple(0.0001,  0, 0, 17,                                                              "0.0001"                ),
  tuple(0.00001, 0, 0, 1,                                                               "1e-05"                 ),
  tuple(0.00001, 0, 0, 3,                                                               "1e-05"                 ),
  tuple(0.00001, 0, 0, 5,                                                               "1e-05"                 ),
  tuple(0.00001, 0, 0, 10,                                                              "1e-05"                 ),
  tuple(0.0045678901234567, 0, 0, 1,                                                    "0.005"                 ),
  tuple(0.0045678901234567, 0, 0, 3,                                                    "0.00457"               ),
  tuple(0.0045678901234567, 0, 0, 5,                                                    "0.0045679"             ),
  tuple(0.0045678901234567, 0, 0, 12,                                                   "0.00456789012346"      ),
  tuple(0.0005678901234567, 0, 0, 1,                                                    "0.0006"                ),
  tuple(0.0005678901234567, 0, 0, 3,                                                    "0.000568"              ),
  tuple(0.0005678901234567, 0, 0, 5,                                                    "0.00056789"            ),
  tuple(0.0005678901234567, 0, 0, 12,                                                   "0.000567890123457"     ),
  tuple(0.1,     0, 0, -2,                                                              "0.1"                   ),
  tuple(0.00001, 0, 0, -2,                                                              "1e-05"                 ),

#if !defined(ENABLE_OLD_OFSTD_FTOA_IMPLEMENTATION) && !defined(HAVE__SET_OUTPUT_FORMAT)

  // This is the expected output of the new routine
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 0, -1,                                "1234567890000000012898796547947496515996930375163" ),
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 20, -1,                               "1234567890000000012898796547947496515996930375163" ),
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 20, 10,                               "1234567890000000012898796547947496515996930375163" ),
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 20, 0,                                "1234567890000000012898796547947496515996930375163" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 0, -1,   "1234567890000000012898796547947496515996930375163" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, -1,  "1234567890000000012898796547947496515996930375163" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 10,  "1234567890000000012898796547947496515996930375163" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 0,   "1234567890000000012898796547947496515996930375163" ),
  tuple(0.1,     0, 0, 17,                                                              "0.10000000000000001"   ),
  tuple(0.00001, 0, 0, 17,                                                              "1.0000000000000001e-05"),
  tuple(0.0045678901234567, 0, 0, 17,                                                   "0.0045678901234567004" ),
  tuple(0.0005678901234567, 0, 0, 17,                                                   "0.00056789012345669998"),
  tuple(0.0005678901234567, 0, 0, -2,                                                   "0.0005678901234567"    )

#elif defined(HAVE__SET_OUTPUT_FORMAT)

  // This is the expected output of the old Microsoft Visual C Runtime (VS 2013 and older)
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 0, -1,                                "1234567890000000000000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 20, -1,                               "1234567890000000000000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 20, 10,                               "1234567890000000000000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 20, 0,                                "1234567890000000000000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 0, -1,   "1234567890000000000000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, -1,  "1234567890000000000000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 10,  "1234567890000000000000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 0,   "1234567890000000000000000000000000000000000000000" ),
  tuple(0.1,     0, 0, 17,                                                              "0.10000000000000001"   ),
  tuple(0.00001, 0, 0, 17,                                                              "1.0000000000000001e-05"),
  tuple(0.0045678901234567, 0, 0, 17,                                                   "0.0045678901234567004" ),
  tuple(0.0005678901234567, 0, 0, 17,                                                   "0.00056789012345669998"),
  tuple(0.0005678901234567, 0, 0, -2,                                                   "0.0005678901234567"    )

#else
  // This is the expected output of the old routine, due to rounding errors
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 0, -1,                                "1234567890000000500000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 20, -1,                               "1234567890000000500000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 20, 10,                               "1234567890000000500000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_format_f, 20, 0,                                "1234567890000000500000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 0, -1,   "1234567890000000500000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, -1,  "1234567890000000500000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 10,  "1234567890000000500000000000000000000000000000000" ),
  tuple(1.23456789E89, OFStandard::ftoa_uppercase | OFStandard::ftoa_format_f, 20, 0,   "1234567890000000500000000000000000000000000000000" ),
  tuple(0.0005678901234567, 0, 0, -2,                                                   "0.00056789012345670002"    )
#endif

};


OFTEST(ofstd_ftoa)
{
  std::setlocale(LC_ALL, "C");

  char buf[50];
  size_t numTuples = sizeof(values)/sizeof(tuple);
  OFString s;

  for (size_t i=0; i<numTuples; i++)
  {
    OFStandard::ftoa(buf, 50, values[i].val, values[i].flags, values[i].width, values[i].prec);
    s = values[i].output;

    OFCHECK_EQUAL(buf, s);
  }


// define this symbol if you want to compare the performance of the
// alternative implementations of OFStandard::ftoa():
// #define ENABLE_TIMER_TEST_FOR_FTOA

#ifdef ENABLE_TIMER_TEST_FOR_FTOA
  std::ofstream outstream("ftoa_performance.txt");
  OFTimer timer;
  for (size_t i=0; i < 50000; ++i)
  {
     OFStandard::ftoa(buf, 50, 0.5, 0, 0, -2);
  }
  outstream << timer << OFendl;
#endif

#if !defined(ENABLE_OLD_OFSTD_FTOA_IMPLEMENTATION) && !defined(HAVE__SET_OUTPUT_FORMAT)
  // regression test for DCMTK issue #860: Make sure that we can convert
  // OFnumeric_limits<double>::max() into a string, convert the string back to double,
  // with a result equal to OFnumeric_limits<double>::max().

  // This does not work with the old implementation in DCMTK, and old versions
  // of Microsoft's MSVCRT introduce rounding errors here

  OFStandard::ftoa(buf, 50, OFnumeric_limits<double>::max(), OFStandard::ftoa_format_e, 0, 17);
  s = "1.79769313486231571e+308";
  OFCHECK_EQUAL(buf, s);
  OFBool success = OFFalse;
  double d = OFStandard::atof(buf, &success);
  OFCHECK_EQUAL(OFTrue, success);
  OFCHECK_EQUAL(d, OFnumeric_limits<double>::max());

  // now repeat the test for OFnumeric_limits<double>::min()
  OFStandard::ftoa(buf, 50, OFnumeric_limits<double>::min(), OFStandard::ftoa_format_e, 0, 17);
  s = "2.22507385850720138e-308";
  OFCHECK_EQUAL(buf, s);
  success = OFFalse;
  d = OFStandard::atof(buf, &success);
  OFCHECK_EQUAL(OFTrue, success);
  OFCHECK_EQUAL(d, OFnumeric_limits<double>::min());

  // try to activate a locale that uses ',' as a decimal point.
  // If activating the locale fails, we still continue with the test
  std::setlocale(LC_ALL, "de_DE");
  OFStandard::ftoa(buf, 50, 0.1, 0, 0, 3);
  s = "0.1";
  OFCHECK_EQUAL(buf, s);

  // try to activate a locale that uses <U066B> as a decimal point.
  // If activating the locale fails, we still continue with the test
  std::setlocale(LC_ALL, "ps_AF");
  OFStandard::ftoa(buf, 50, 0.1, 0, 0, 3);
  s = "0.1";
  OFCHECK_EQUAL(buf, s);

  std::setlocale(LC_ALL, "C");

#endif

}
