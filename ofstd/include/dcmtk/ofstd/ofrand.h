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
 *  Author:  Marco Eichelberg, based on the reference implementation
 *   of the ISAAC PRNG by Bob Jenkins (Public Domain).
 *
 *  Purpose: Cryptographically secure PRNG based on Bob Jenkins's ISAAC algorithm
 *
 */

#ifndef OFRAND_H
#define OFRAND_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"

#define OFRandom_SIZL   (8)
#define OFRandom_SIZ    (1<<OFRandom_SIZL)

class DCMTK_OFSTD_EXPORT OFRandom
{
public:

  /** default constructor, initializes the PRNG based on time(2) and clock(2).
   */
  OFRandom();

  /// destructor
  virtual ~OFRandom() {}

#ifndef OF_NO_UINT64
  /** return a random Uint64 value
   *  @return random unsigned 64-bit number
   */
  Uint64 getRND64();
#endif

  /** return a random Uint32 value
   *  @return random unsigned 32-bit number
   */
  Uint32 getRND32();

  /** return a random Uint16 value
   *  @return random unsigned 16-bit number
   */
  Uint16 getRND16();

  /** Re-seed the PRNG with the given parameter.
   *  This method should only be used when a reproducible sequence of pseudo-random numbers is desired, e.g. in test routines.
   *  @param sval seed value
   */
  void seed(Uint32 sval);

private:
  /** generate next round of random numbers. Calling this method will cause
   *  the algorithm to generate a new set of random numbers in randrsl.
   */
  void isaac();

  /** mix the seed values and then generate the first round of random numbers.
   */
  void mixSeed();

  /// number of random bytes still available before isaac() needs to called again
  Uint32 randcnt;

  /// random memory
  Uint32 randrsl[OFRandom_SIZ];

  ///internal state of the PRNG
  Uint32 randmem[OFRandom_SIZ];

  /// internal state of the PRNG
  Uint32 randa;

  /// internal state of the PRNG
  Uint32 randb;

  /// internal state of the PRNG
  Uint32 randc;
};
#endif
