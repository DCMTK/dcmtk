/*
 *
 *  Copyright (C) 2018, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Common exit codes for command line tools
 *
 */


#ifndef OFEXIT_H
#define OFEXIT_H

#include "dcmtk/config/osconfig.h"


/* common exit codes for DCMTK command line tools
 * (EXIT_SUCCESS and EXIT_FAILURE are standard codes
 *  but their numerical values are not standardized)
 */

// general (0-19)
#define EXITCODE_NO_ERROR                         0
#define EXITCODE_COMMANDLINE_SYNTAX_ERROR         1
#define EXITCODE_INSUFFICIENT_PRIVILEGES          2
#define EXITCODE_SETUID_FAILED                    3

// input file errors (20-39)
#define EXITCODE_CANNOT_READ_INPUT_FILE          20
#define EXITCODE_NO_INPUT_FILES                  21
#define EXITCODE_INVALID_INPUT_FILE              22
#define EXITCODE_NO_VALID_INPUT_FILES            23
#define EXITCODE_INVALID_INPUT_DIRECTORY         25

// output file errors (40-59)
#define EXITCODE_CANNOT_WRITE_OUTPUT_FILE        40
#define EXITCODE_INVALID_OUTPUT_DIRECTORY        45

// network errors (60-79)
// - defined in the respective modules / tools

// processing errors (80-99)
// - defined in the respective modules / tools

// user-defined errors (100-119)
// - defined in the respective modules / tools

// GNU recommends that the codes 128-255 be reserved for serious errors:
// - https://www.gnu.org/software/libc/manual/html_node/Exit-Status.html

#endif
