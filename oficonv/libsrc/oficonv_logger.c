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

#include "dcmtk/config/osconfig.h"
#include "dcmtk/oficonv/iconv.h"
#include <stdio.h>

static oficonv_logger_callback_t logger_callback = NULL;
static int log_level = 3;

void set_oficonv_logger_callback(oficonv_logger_callback_t callback)
{
  logger_callback = callback;
}

oficonv_logger_callback_t get_oficonv_logger_callback()
{
  return logger_callback;
}

void set_oficonv_log_level(int level)
{
  log_level = level;
}

void oficonv_log(int level, const char *text1, const char *text2, const char *text3)
{
  if (logger_callback) logger_callback(level, text1, text2, text3);
  else
  {
    const char *level_text;
    switch (level)
    {
      case 0:
        level_text = "T:";
        break;
      case 1:
        level_text = "D:";
        break;
      case 2:
        level_text = "I:";
        break;
      case 3:
        level_text = "W:";
        break;
      case 4:
        level_text = "E:";
        break;
      default:
        level_text = "F:";
        break;
    }
    if (level >= log_level) fprintf(stderr, "%s %s%s%s\n", level_text, text1, text2, text3);
  }
}
