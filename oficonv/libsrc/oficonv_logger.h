/*
 *
 *  Copyright (C) 2022, OFFIS e.V.
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
 *  Purpose: logging function for oficonv
 *
 */

#ifndef OFICONV_LOGGER_H
#define OFICONV_LOGGER_H

#include "dcmtk/config/osconfig.h"

/** log a message to the ofconv logger
 *  @param level log level, 0=trace, 1=debug, 2=info, 3=warn, 4=error, 5=fatal
 *  @param text1 first part of the string to be logged, must not be NULL
 *  @param text2 second part of the string to be logged, must not be NULL
 *  @param text3 third part of the string to be logged, must not be NULL
*/
void oficonv_log(int level, const char *text1, const char *text2, const char *text3);

#endif