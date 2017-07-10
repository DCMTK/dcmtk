/*
 *  Copyright (C) 2017, OFFIS e.V.
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
 *  Module:  config
 *
 *  Author:  Jan Schlamelcher
 *
 *  Purpose: Testing for explicit large file support (LFS64)
 */

#define _LARGEFILE64_SOURCE
#include <stdio.h>

using namespace std;

int main()
{
  FILE *f = fopen64("name", "r");
  return 0;
}
