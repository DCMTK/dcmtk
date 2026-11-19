/*
 *
 *  Copyright (C) 2011-2026, OFFIS e.V.
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
#include "dcmtk/ofstd/ofthread.h"
#include "dcmtk/ofstd/ofuuidgn.h"
#include "dcmtk/ofstd/ofvector.h"

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>

#ifdef WITH_THREADS

static size_t const THREAD_COUNT = 4;
static size_t const OFUUIDS_PER_THREAD = 25000;
static size_t const CANONICAL_NOTATION_SIZE = 37; // incl. terminating NUL

class OFUUIDThread : public OFThread
{
public:
  OFUUIDThread() : OFThread(), buffer_(NULL) {}

  virtual void run() {
    for (size_t i = 0; i < OFUUIDS_PER_THREAD; ++i) {
      OFshared_ptr<OFUUID> const a = OFUUIDGenerator::create();
      if (a) {
        OFStandard::strlcpy(buffer_ + i * CANONICAL_NOTATION_SIZE,
          a->toString(OFUUID::NotationCanonical).c_str(), CANONICAL_NOTATION_SIZE);
      }
    }
  }

  // Where to store the canonical notations of the OFUUIDs created.
  char * buffer_;
};

static int
compare_strings(void const * a,
                void const * b) {
  return strcmp(OFstatic_cast(char const *, a), OFstatic_cast(char const *, b));
}

// Counts the strings in 'uuids' whose 'length' characters at 'offset' equal
// those of another string.
static size_t
count_duplicates(OFVector<char> const & uuids,
                 size_t const offset,
                 size_t const length) {
  size_t const count = uuids.size() / CANONICAL_NOTATION_SIZE;
  OFVector<char> keys(count * (length + 1), '\0');
  for (size_t i = 0; i < count; ++i) {
    memcpy(&keys[i * (length + 1)], &uuids[i * CANONICAL_NOTATION_SIZE + offset], length);
  }

  qsort(&keys[0], count, length + 1, compare_strings);

  size_t duplicates = 0;
  for (size_t i = 1; i < count; ++i) {
    if (strcmp(&keys[(i - 1) * (length + 1)], &keys[i * (length + 1)]) == 0) {
      ++duplicates;
    }
  }

  return duplicates;
}

#endif // WITH_THREADS

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

#ifdef WITH_THREADS

OFTEST(ofstd_OFUUID_threads)
{
  // Verify that OFUUIDs created concurrently by several threads are unique.
  OFVector<char> uuids(THREAD_COUNT * OFUUIDS_PER_THREAD * CANONICAL_NOTATION_SIZE, '\0');
  OFUUIDThread threads[THREAD_COUNT];
  for (size_t i = 0; i < THREAD_COUNT; ++i) {
    threads[i].buffer_ = &uuids[i * OFUUIDS_PER_THREAD * CANONICAL_NOTATION_SIZE];
    OFCHECK(threads[i].start() == 0);
  }

  for (size_t i = 0; i < THREAD_COUNT; ++i) {
    OFCHECK(threads[i].join() == 0);
  }

  // The timestamp and the counter, i.e. the first 18 characters of the
  // canonical notation, must be unique within a process.
  size_t const counterDuplicates = count_duplicates(uuids, 0, 18);
  OFCHECK_EQUAL(counterDuplicates, 0U);

  // The random part, i.e. the last 17 characters of the canonical notation
  // (62 random bits), must not repeat either. The probability of a collision
  // among 100000 OFUUIDs is about 1e-9.
  size_t const randomDuplicates = count_duplicates(uuids, 19, 17);
  OFCHECK_EQUAL(randomDuplicates, 0U);
}

#endif // WITH_THREADS


OFTEST(ofstd_OFUUID_timestamp)
{
  // Verify that the timestamp of an OFUUID holds the number of milliseconds
  // since January 1, 1970 (cf. RFC 9562, section 5.7), i.e. the first twelve
  // hex digits of the canonical notation. Allow for a tolerance of one minute.
  Uint64 const before = OFstatic_cast(Uint64, time(NULL));
  OFshared_ptr<OFUUID> const a = OFUUIDGenerator::create();
  Uint64 const after = OFstatic_cast(Uint64, time(NULL));
  OFCHECK_MSG(a, "a is NULL");
  if (!a) {
    return;
  }

  OFString const s = a->toString(OFUUID::NotationCanonical);
  // The timestamp occupies the first 48 bits (6 bytes) of the UUID, i.e. the
  // first 12 hex digits of the canonical notation "xxxxxxxx-xxxx-...". The
  // hyphen at position 8, counting from 0, has to be ignored in the canonical
  // notation.
  Uint64 timestamp = 0;
  for (size_t i = 0; i < 13; ++i) {
    if (i == 8) { // s[8] == '-' -> ignore
      continue;
    }
    char const c = s[i];
    timestamp <<= 4;
    timestamp |= OFstatic_cast(Uint64, (c >= 'a') ? (c - 'a' + 10) : (c - '0'));
  }

  Uint64 const seconds = timestamp / 1000;
  OFCHECK(before - 60 <= seconds);
  OFCHECK(seconds <= after + 60);
}


