/*
 *
 *  Copyright (C) 2026, OFFIS e.V.
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
 *  Purpose: Implementation of a Universally Unique IDentifier (UUID) Generator.
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofuuidgn.h"


BEGIN_EXTERN_C
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


OFUUIDGenerator::Uint32_32 OFUUIDGenerator::last_ms_since_epoch_ = OFUUIDGenerator::Uint32_32();
Uint16 OFUUIDGenerator::rand_b_field_ = 0;


OFshared_ptr<OFUUID>
OFUUIDGenerator::create(rnd8 const & rnd) {
  static OFMutex mutex;

  mutex.lock();

  OFUUIDGenerator::initializeIfNecessary();

  Uint32_32 const ms_since_epoch = getMillisecondsSinceEpoch();
  if (ms_since_epoch > OFUUIDGenerator::last_ms_since_epoch_) {
    OFUUIDGenerator::last_ms_since_epoch_ = ms_since_epoch;
    OFUUIDGenerator::rand_b_field_ = 1;
  }
  else {
    ++OFUUIDGenerator::rand_b_field_;

    // The field 'rand_b' in UUIDv7 uses 12 bits (0x0fff), so look out for
    // possible overflow.
    if (OFUUIDGenerator::rand_b_field_ > 0x0fff) {
      OFUUIDGenerator::last_ms_since_epoch_.ladd(1);
      OFUUIDGenerator::rand_b_field_ = 1;
    }
  }

  mutex.unlock();

  OFUUID * const ofuuid = new (std::nothrow) OFUUID();
  if (ofuuid != NULL) {
    setupOFUUIDDataMember(ofuuid, rnd);
  }

  return OFshared_ptr<OFUUID>(ofuuid);
}

OFUUIDGenerator::Uint32_32
OFUUIDGenerator::getMillisecondsSinceEpoch() {
  // Will ultimately hold the milliseconds from January 1, 1970.
  Uint32_32 ms_since_unix_epoch;

#ifdef _WIN32

  // 'ft' is the number of 100ns ticks since January 1, 1601 (UTC).
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft));

  // 'uli' is (also) the number of 100ns ticks since January 1, 1601 (UTC),
  // but we want the number of milliseconds since January 1, 1970; so lets
  // calculate...
  ULARGE_INTEGER uli;
  uli.LowPart = ft.dwLowDateTime;
  uli.HighPart = ft.dwHighDateTime;

  // 'uli' is now the number of milliseconds since January 1, 1601 (UTC).
  uli.QuadPart /= 10;

  // Add the number of milliseconds from January 1, 1601 (UTC) until
  // January 1, 1970. The number of days between these two days is 36524.
  uli.QuadPart += (  OFstatic_cast(unsigned __int64, 36524)
                   * OFstatic_cast(unsigned __int64, 60 * 60 * 24) // seconds per day
                   * OFstatic_cast(unsigned __int64, 1000 * 1000)) // milliseconds per second
  ms_since_unix_epoch.ladd(OFstatic_cast, Uint32, uli.LowPart);
  ms_since_unix_epoch.hadd(OFstatic_cast, Uint32, uli.HighPart);

  #else

  // struct timeval {
  //   time_t      tv_sec;     /* seconds */
  //   suseconds_t tv_usec;    /* microseconds */
  // };
  struct timeval tp;

  // 'tp' is offset from January 1, 1970 (Unix epoch)
  gettimeofday(&tp, NULL);

  // 'tp.tv_usec' holds microseconds but milliseconds are needed
  ms_since_unix_epoch = tp.tv_usec / 1000; // 1000 microseconds per millisecond

  // 'tp.tv_sec' holds seconds but milliseconds are needed
  // Multiplication by 1000, i.e. the number of milliseconds per second will
  // likely overflow a 32bit integer
  //
  // Some math:
  // Let: (ah * 2^16 + al) = tp.tv_sec   (i) and (ii)
  //      (bh * 2^16 + bl) = 1000        (iii) and (iv)
  //
  // It follows:    tp.tv_sec * 1000
  //             =  (ah * 2^16 + al) * (bh * 2^16 + bl)
  //             =     al * bl                              (v)
  //                +  (ah * bl + al * bh) * 2^16           (vii) and (viii)
  //                +  ah * bh * 2^32                       (vi)
  //
  Uint32 const ah = OFstatic_cast(Uint32, tp.tv_sec >> 16);    // (i)
  Uint32 const al = tp.tv_sec & 0xffff;                        // (ii)
  Uint32 const bh = 1000 >> 16;    // milliseconds per second  // (iii)
  Uint32 const bl = 1000 & 0xffff; // milliseconds per second  // (iv)

  ms_since_unix_epoch.ladd(al * bl);                           // (v)
  ms_since_unix_epoch.hadd(ah * bh);                           // (vi)

  Uint32 add = ((ah * bl + al * bh) & 0x0000ffff) << 16;       // (vii)
  ms_since_unix_epoch.ladd(add);

  add = ((ah * bl + al * bh) & 0xffff0000) >> 16;              // (viii)
  ms_since_unix_epoch.hadd(add);

#endif

  return ms_since_unix_epoch;
}

Uint8
OFUUIDGenerator::getRND8() {
  static OFRandom rnd;

  return rnd.getRND8();
}

OFBool
OFUUIDGenerator::initializeIfNecessary() {
  if (OFUUIDGenerator::last_ms_since_epoch_ != 0) {
    return OFTrue;
  }

  OFUUIDGenerator::last_ms_since_epoch_ = OFUUIDGenerator::getMillisecondsSinceEpoch();

  return (OFUUIDGenerator::last_ms_since_epoch_ != 0);
}

OFUUIDGenerator::OFUUIDGenerator() {
  // DELIBERATELY EMPTY
}

void
OFUUIDGenerator::setupOFUUIDDataMember(OFUUID * const ofuuid,
                                       rnd8 const & rnd) {
  // [ofuuid.data_[0], ofuuid.data_[1]] := last_ms_since_epoch_.high_ & 0x00ff
  // Only the lower 16 bits are used, so mask the upper ones.  (cf. RFC9562, section 5.7)
  Uint32 value = OFUUIDGenerator::last_ms_since_epoch_.high_;
  for (int i = 1; i >=0 ; --i) {
    ofuuid->data_[OFstatic_cast(size_t, i)] = OFstatic_cast(Uint8, value);
    value >>= 8;
  }

  // [ofuuid.data_[2], ofuuid.data_[5]] := last_ms_since_epoch_.low_
  // All 32 bits are used.  (cf. RFC9562,section 5.7)
  value = OFUUIDGenerator::last_ms_since_epoch_.low_;
  for (int i = 5; i >= 2; --i) {
    ofuuid->data_[OFstatic_cast(size_t, i)] = OFstatic_cast(Uint8, value);
    value >>= 8;
  }

  // [ofuuid.data_[6], ofuuid.data_[7]] := OFUUIDGenerator::rand_b_field_,
  // except for the version bits - see below.
  value = OFUUIDGenerator::rand_b_field_;
  for (int i = 7; i >= 6; --i) {
    ofuuid->data_[OFstatic_cast(size_t, i)] = OFstatic_cast(Uint8, value);
    value >>= 8;
  }

  // The four (4) most significant bits of ofuuid.data_[6] will contain the UUID
  // version  (cf. RFC9562,sections 4.2 and 5.7)
  (ofuuid->data_[6] &= 0x0f) |= OFUUIDGenerator::OFUUID_VERSION_MASK;

  // [ofuuid.data_[8], ofuuid.data_[15]] are filled with random data, except for
  // the variant bits - see below.
  for (size_t i = 8; i < 16; ++i) {
    ofuuid->data_[i] = rnd();
  }

  // The two (2) most significant bits of ofuuid.data_[8] will contain the UUID
  // variant  (cf. RFC9562,sections 4.2 and 5.7)
  (ofuuid->data_[8] &= 0x3f) |= OFUUIDGenerator::OFUUID_VARIANT_MASK;
}

OFUUIDGenerator::~OFUUIDGenerator() {
  // DELIBERATELY EMPTY
}



OFUUIDGenerator::Uint32_32 &
OFUUIDGenerator::Uint32_32::hadd(Uint32 const value) {
  high_ += value;

  return *this;
}

OFUUIDGenerator::Uint32_32 &
OFUUIDGenerator::Uint32_32::ladd(Uint32 const value) {
  if (OFStandard::check32BitAddOverflow(low_, value)) {
    ++high_;
  }
  low_ += value;

  return *this;
}

OFUUIDGenerator::Uint32_32 &
OFUUIDGenerator::Uint32_32::operator=(Uint32 const value) {
  high_ = 0;
  low_ = value;

  return *this;
}

OFUUIDGenerator::Uint32_32::Uint32_32() :
  high_(0),
  low_(0) {
}

OFUUIDGenerator::Uint32_32::Uint32_32(OFUUIDGenerator::Uint32_32 const & other) :
  high_(other.high_),
  low_(other.low_) {
}

OFUUIDGenerator::Uint32_32::Uint32_32(Uint32 const value) :
  high_(0),
  low_(value) {
}

OFUUIDGenerator::Uint32_32::~Uint32_32() {
  // DELIBERATELY EMPTY
}












