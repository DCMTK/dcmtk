/*
 *
 *  Copyright (C) 2021, OFFIS e.V.
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
 *  Author:  Marco Eichelberg, based on the reference implementation
 *   of the ISAAC PRNG by Bob Jenkins (Public Domain).
 *
 *  Purpose: Cryptographically secure PRNG based on Bob Jenkins's ISAAC algorithm
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofrand.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstdinc.h"
#include <ctime>


BEGIN_EXTERN_C
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define ind(mm,x)  ((mm)[(x>>2)&(OFRandom_SIZ-1)])

#define rngstep(mix,a,b,mm,m,m2,r,x) \
{ \
  x = *m;  \
  a = ((a^(mix)) + *(m2++)); \
  *(m++) = y = (ind(mm,x) + a + b); \
  *(r++) = b = (ind(mm,y>>OFRandom_SIZL) + x) & 0xffffffff; \
}

#define mix(a,b,c,d,e,f,g,h) \
{ \
   a^=b<<11;              d+=a; b+=c; \
   b^=(c&0xffffffff)>>2;  e+=b; c+=d; \
   c^=d<<8;               f+=c; d+=e; \
   d^=(e&0xffffffff)>>16; g+=d; e+=f; \
   e^=f<<10;              h+=e; f+=g; \
   f^=(g&0xffffffff)>>4;  a+=f; g+=h; \
   g^=h<<8;               b+=g; h+=a; \
   h^=(a&0xffffffff)>>9;  c+=h; a+=b; \
}

OFRandom::OFRandom()
: randcnt(OFRandom_SIZ)
, randrsl()
, randmem()
, randa(0)
, randb(0)
, randc(0)
{
   /* initialize randrsl based on the current time, CPU clock count, and process ID */
   Uint32 tm = OFstatic_cast(Uint32, time(NULL));
   Uint32 cl = OFstatic_cast(Uint32, clock());
   Uint32 pi = OFstatic_cast(Uint32, OFStandard::getProcessID());

#ifdef _WIN32
    ULARGE_INTEGER wintm;
    /* tm is number of 100ns ticks since Jan 01 1601.
     * We only use the lowest 32 bits of this number.
     */
    GetSystemTimeAsFileTime(OFreinterpret_cast(FILETIME *, &wintm));
    Uint32 tm2 = OFstatic_cast(Uint32, wintm.LowPart);
#else
    struct timeval posixtm;
    gettimeofday(&posixtm, NULL);
    /* sub-second part of current time in microseconds */
    Uint32 tm2 = OFstatic_cast(Uint32, posixtm.tv_usec);
#endif

   for (int i=0; i<OFRandom_SIZ; i += 4)  // OFRandom_SIZ is dividable by 4
   {
     randrsl[i] = tm++;
     randrsl[i+1] = cl++;
     randrsl[i+2] = pi++;
     randrsl[i+3] = tm2++;
   }

   mixSeed();
}

void OFRandom::mixSeed()
{
   int i;
   Uint32 a,b,c,d,e,f,g,h;
   a=b=c=d=e=f=g=h=0x9e3779b9;  /* the golden ratio */

   for (i=0; i<4; ++i) /* scramble it */
   {
     mix(a,b,c,d,e,f,g,h);
   }

   /* initialize using the contents of randrsl[] as the seed */
   for (i=0; i<OFRandom_SIZ; i+=8)
   {
     a+=randrsl[i  ]; b+=randrsl[i+1];
     c+=randrsl[i+2]; d+=randrsl[i+3];
     e+=randrsl[i+4]; f+=randrsl[i+5];
     g+=randrsl[i+6]; h+=randrsl[i+7];
     mix(a,b,c,d,e,f,g,h);
     randmem[i  ]=a; randmem[i+1]=b; randmem[i+2]=c; randmem[i+3]=d;
     randmem[i+4]=e; randmem[i+5]=f; randmem[i+6]=g; randmem[i+7]=h;
   }
   /* do a second pass to make all of the seed affect all of m */
   for (i=0; i<OFRandom_SIZ; i+=8)
   {
     a+=randmem[i  ]; b+=randmem[i+1];
     c+=randmem[i+2]; d+=randmem[i+3];
     e+=randmem[i+4]; f+=randmem[i+5];
     g+=randmem[i+6]; h+=randmem[i+7];
     mix(a,b,c,d,e,f,g,h);
     randmem[i  ]=a; randmem[i+1]=b; randmem[i+2]=c; randmem[i+3]=d;
     randmem[i+4]=e; randmem[i+5]=f; randmem[i+6]=g; randmem[i+7]=h;
   }

   isaac(); /* fill in the first set of results */}

void OFRandom::seed(Uint32 sval)
{
   for (int i=0; i<OFRandom_SIZ; i++)
   {
     randrsl[i] = sval;
   }
   randa = 0;
   randb = 0;
   randc = 0;
   mixSeed();
}

void OFRandom::isaac()
{
   Uint32 x,y,*m,*m2,*mend;
   Uint32 *r = randrsl;
   Uint32 a = randa;
   Uint32 b = randb + (++randc);
   for (m = randmem, mend = m2 = m+(OFRandom_SIZ/2); m<mend; )
   {
      rngstep( a<<13, a, b, randmem, m, m2, r, x);
      rngstep( (a & 0xffffffff) >>6 , a, b, randmem, m, m2, r, x);
      rngstep( a<<2 , a, b, randmem, m, m2, r, x);
      rngstep( (a & 0xffffffff) >>16, a, b, randmem, m, m2, r, x);
   }
   for (m2 = randmem; m2<mend; )
   {
      rngstep( a<<13, a, b, randmem, m, m2, r, x);
      rngstep( (a & 0xffffffff) >>6 , a, b, randmem, m, m2, r, x);
      rngstep( a<<2 , a, b, randmem, m, m2, r, x);
      rngstep( (a & 0xffffffff) >>16, a, b, randmem, m, m2, r, x);
   }
   randb = b; randa = a;
   randcnt = OFRandom_SIZ;
}

Uint32 OFRandom::getRND32()
{
  // check if we still have random numbers left
  if (randcnt == 0) isaac();

  // we use the random numbers from last to first
  return randrsl[--randcnt];
}

Uint16 OFRandom::getRND16()
{
  // use one 32-bit random number and ignore the extra bits
  return OFstatic_cast(Uint16, getRND32());
}

#ifndef OF_NO_UINT64
Uint64 OFRandom::getRND64()
{
  // get a 32-bit random number
  Uint64 result = getRND32();
  // shift into upper 32 bits of the 64-bit word
  result <<= 32;
  // get a second 32-bit random number
  result |= getRND32();
  return result;
}
#endif
