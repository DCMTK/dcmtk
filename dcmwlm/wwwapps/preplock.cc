/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Module:  dcmwlm
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose:
 *   This application creates binary coded "struct flock"  fields for  use
 *   within the "fcntl" function of a perl script. The output of this file
 *   is system dependent, therefore it should be called dynamically  (e.g.
 *   upon startup of the perl script). This  will  prevent  problems  with
 *   systems supporting fat binaries (e.g. NeXTStep).
 *
 *   Usage example (from perl):
 *     $fcntldata=`./preplock`;
 *     eval $fcntldata;
 *   This will create four global perl variables:
 *     $const_F_SETLKW      (2nd fnctl parameter for locking files)
 *     $const_RDLCK_STRUCT  (3rd   "      "       "  read lock)
 *     $const_WRLCK_STRUCT  ( "    "      "       "  write lock)
 *     $const_UNLCK_STRUCT  ( "    "      "       "  unlock)
 *
 *   A perl fcntl call using these (packed binary) values will lock/unlock a
 *   whole file, which must be a successfully opened perl  file  descriptor.
 *   See perl man page for details.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstream.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#include <fcntl.h>
END_EXTERN_C

int main()
{
#ifdef __MINGW32__
  /* MinGW does not support flock, there is no way to make this work. */
  printf("die \"Sorry, this application is not supported on the MinGW platform\";\n");
#else
  struct flock lockdata;
  unsigned int i;
  unsigned char *c;

  /* first, we initialize the data */
  c = (unsigned char *)(&lockdata);
  for (i=0; i<sizeof(struct flock); i++) c[i]=0;

  lockdata.l_whence=0;
  lockdata.l_start=0;
  lockdata.l_len=0;

  /* now we create the output */
  printf("$const_F_SETLKW = %ld;\n",(long)F_SETLKW);

  lockdata.l_type = F_RDLCK;
  printf("$const_RDLCK_STRUCT = pack(\"");
  for (i=0; i<sizeof(struct flock); i++) printf("c");
  printf("\"");
  for (i=0; i<sizeof(struct flock); i++) printf(",%d",(int)(c[i]));
  printf(");\n");

  lockdata.l_type = F_WRLCK;
  printf("$const_WRLCK_STRUCT = pack(\"");
  for (i=0; i<sizeof(struct flock); i++) printf("c");
  printf("\"");
  for (i=0; i<sizeof(struct flock); i++) printf(",%d",(int)(c[i]));
  printf(");\n");

  lockdata.l_type = F_UNLCK;
  printf("$const_UNLCK_STRUCT = pack(\"");
  for (i=0; i<sizeof(struct flock); i++) printf("c");
  printf("\"");
  for (i=0; i<sizeof(struct flock); i++) printf(",%d",(int)(c[i]));
  printf(");\n");

#endif
  return 0;
}
