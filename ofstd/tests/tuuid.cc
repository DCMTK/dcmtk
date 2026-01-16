/*
 *
 *  Copyright (C) 2011-2021, OFFIS e.V.
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
 *  Author:  Harald Roesen
 *
 *  Purpose: test program for OFUUID
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofuuidgn.h"


#include <iostream>

static void
verify_notation_canonical(OFString const & s) {
  // Verify that the canonical notation of an OFUUID is 36 characters long.
  size_t const size = s.size();
  OFCHECK(size == 36);

  // Verify that the canonical notation of an OFUUID contains four hyphens
  // ('-'). Namely at positions 8, 13, 18, and 23 (counting from position 0).
  OFCHECK(s[8]  == '-');
  OFCHECK(s[13] == '-');
  OFCHECK(s[18] == '-');
  OFCHECK(s[23] == '-');

  // Verify that the characters in the canonical notation of an OFUUID are
  // exclusively from the ranges '0' to '9 or 'a' to 'f' respectively, except
  // for the positions where a hyphen '-' is expected - see above.
  OFString const allowedChars("0123456789abcdef");
  for (size_t i = 1; i < size; ++i) {
    if (i == 8 || i == 13 || i == 18 || i == 23) {
      continue;
    }

    OFString msg("find(");
    (msg += s[i]) += ')';
    OFCHECK_MSG(allowedChars.find(s[i]) != OFString_npos, msg.c_str());
  }
}

static void
verify_notation_integer(OFString const & s) {
  // Verify that the integer notation of an OFUUID is at least one character long.
  size_t const size = s.size();
  OFCHECK(size > 0);

  // Verify that the integer notation of an OFUUID starts with a '0' character
  // iff its size/length is one (1).
  if (size == 1) {
    OFCHECK(s[0] == '0');
  }
  else {
    OFCHECK(s[0] != '0');

    // Verify that the characters in the integer notation of an OFUUID from the
    // second character onwards are exclusively from the range '0' to '9'.
    OFString const allowedChars("0123456789");
    for (size_t i = 1; i < size; ++i) {
      OFString msg("find(");
      (msg += s[i]) += ')';
      OFCHECK_MSG(allowedChars.find(s[i]) != OFString_npos, msg.c_str());
    }
  }
}


OFTEST(ofstd_OFUUID_equalities)
{
  // Verify that an OFUUID is equal to itself.
  OFshared_ptr<OFUUID> const a = OFUUIDGenerator::create();
  OFCHECK_MSG(a, "a is NULL");
  if (!a) {
    return;
  }

  OFCHECK(*a == *a);
  OFCHECK(!(*a != *a));
}

OFTEST(ofstd_OFUUID_inequalities)
{
  // Verify that two OFUUIDs created one after the other are unequal.
  OFshared_ptr<OFUUID> const a = OFUUIDGenerator::create();
  OFCHECK_MSG(a, "a is NULL");
  if (!a) {
    return;
  }

  OFshared_ptr<OFUUID> const b = OFUUIDGenerator::create();
  OFCHECK_MSG(b, "b is NULL");
  if (!b) {
    return;
  }

  OFCHECK(*a != *b);
  OFCHECK(!(*a == *b));

  OFCHECK(*b != *a);
  OFCHECK(!(*b == *a));
}

OFTEST(ofstd_OFUUID_notation_integer)
{
  OFshared_ptr<OFUUID> const a = OFUUIDGenerator::create();
  OFCHECK_MSG(a, "a is NULL");
  if (!a) {
    return;
  }

  OFString const s = a->toString(OFUUID::NotationInteger);
  verify_notation_integer(s);
}

OFTEST(ofstd_OFUUID_notation_canonical)
{
  OFshared_ptr<OFUUID> const a = OFUUIDGenerator::create();
  OFCHECK_MSG(a, "a is NULL");
  if (!a) {
    return;
  }

  OFString const s = a->toString(OFUUID::NotationCanonical);
  verify_notation_canonical(s);
}

OFTEST(ofstd_OFUUID_notation_oid)
{
  // Verify that the oid notation of an OFUUID starts with the character
  // sequence '2.25.' (five (5) characters).
  OFshared_ptr<OFUUID> const a = OFUUIDGenerator::create();
  OFCHECK_MSG(a, "a is NULL");
  if (!a) {
    return;
  }

  OFString const s = a->toString(OFUUID::NotationOID);
  OFString const oid("2.25.");
  OFCHECK(s.find(oid) == 0);

  // Verify that the 'beheaded' oid notation of an OFUUID equals its
  // integer notation.
  verify_notation_integer(s.substr(oid.size()));
}

OFTEST(ofstd_OFUUID_notation_urn)
{
  // Verify that the urn notation of an OFUUID starts with the character
  // sequence 'urn:uuid:' (nine (9) characters).
  OFshared_ptr<OFUUID> const a = OFUUIDGenerator::create();
  OFCHECK_MSG(a, "a is NULL");
  if (!a) {
    return;
  }

  OFString const s = a->toString(OFUUID::NotationURN);
  OFString const urn("urn:uuid:");
  OFCHECK(s.find(urn) == 0);

  // Verify that the 'beheaded' urn notation of an OFUUID equals its
  // canonical notation.
  verify_notation_canonical(s.substr(urn.size()));
}

OFTEST(ofstd_OFUUID_variant)
{
  // Verify that an OFUUID's variant is 0b10 (two (2)).
  // The canonical notation is required to hold one of characters '8', '9', 'a',
  // or 'b' at position 19 (counting from 0) - cf. RFC 9562, section 4.1.
  OFshared_ptr<OFUUID> const a = OFUUIDGenerator::create();
  OFCHECK_MSG(a, "a is NULL");
  if (!a) {
    return;
  }

  OFString const s = a->toString(OFUUID::NotationCanonical);
  OFString const allowedChars("89ab");
  OFCHECK(allowedChars.find(s[19]) != OFString_npos);
}

OFTEST(ofstd_OFUUID_version)
{
  // Verify that an OFUUID's version is 0b0111 (seven (7)).
  // The canonical notation is required to hold the character '7' at position
  // 14 (counting from 0) - cf. RFC 9562, section 4.2.
  OFshared_ptr<OFUUID> const a = OFUUIDGenerator::create();
  OFCHECK_MSG(a, "a is NULL");
  if (!a) {
    return;
  }

  OFString const s = a->toString(OFUUID::NotationCanonical);
  OFCHECK(s[14] == '7');
}

OFTEST(ofstd_OFUUID_monotonicity)
{
  // Verify that each OFUUID generated is greater than the last.
  OFshared_ptr<OFUUID> a = OFUUIDGenerator::create();
  OFCHECK_MSG(a, "a is NULL");
    if (!a) {
    return;
  }

  for (size_t i = 0; i < 5000; ++i) {
    OFshared_ptr<OFUUID> const b(OFUUIDGenerator::create());
    OFCHECK_MSG(b, "b is NULL");
    if (!b) {
      return;
    }

    OFCHECK(*a < *b);
    OFCHECK(!(*a >= *b));

    OFCHECK(*b > *a);
    OFCHECK(!(*b <= *a));

    a = b;
  }
}
