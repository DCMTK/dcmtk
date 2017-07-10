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
 *  Purpose: Testing for large file support (LFS)
 */

#include <sys/types.h>
/* Check that off_t can represent 2**63 - 1 correctly. */
/* We can't simply define LARGE_OFF_T to be 9223372036854775807, */
/* since some C++ compilers masquerading as C compilers */
/* incorrectly reject 9223372036854775807. */
#define LARGE_OFF_T (((off_t) 1 << 62) - 1 + ((off_t) 1 << 62))
int main()
{
  int off_t_is_large[(LARGE_OFF_T % 2147483629 == 721 && LARGE_OFF_T % 2147483647 == 1) ? 1 : -1];
  return 0;
}
