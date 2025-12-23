/*
 *
 *  Copyright (C) TBD, OFFIS e.V.
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
 *  Author:  TBD
 *
 *  Purpose: TBD
 *
 */

#ifndef OFUUID_GENERATOR_H
#define OFUUID_GENERATOR_H


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofdefine.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofuuid.h"


struct DCMTK_OFSTD_EXPORT OFUUIDGenerator {
// type(s)
public:
  typedef Uint8 (*rnd8)();

private:
  struct Uint32_32;


// static(s)
// method(s)
public:
  static OFUUID create(rnd8 const & = getRND8);

private:
  static Uint32_32 getMillisecondsSinceEpoch();
  static Uint8 getRND8();  
  static OFBool initialize();
  static void setupOFUUIDDataMember(OFUUID & ofuuid, rnd8 const & rnd);


// member(s)
private:
  static Uint8 const OFUUID_VARIANT_MASK = 0x80; // cf. RFC9562 section 4.1 
  static Uint8 const OFUUID_VERSION_MASK = 0x70; // cf. RFC9562 section 4.2 

  static Uint32_32 last_ms_since_epoch_;
  static OFMutex mutex_;
  static Uint16 rand_b_field_;
  static OFRandom rnd_;


// non-static(s)
// method(s)
private:
  OFUUIDGenerator();
}; // OFUUIDGenerator


struct DCMTK_OFSTD_EXPORT OFUUIDGenerator::Uint32_32 {
// friend(s)
public:
  friend OFBool
  operator==(Uint32_32 const & lhs,
             Uint32_32 const & rhs) {
    return (lhs.high_ == rhs.high_) && (lhs.low_ == rhs.low_);
  }

  friend OFBool
  operator!=(Uint32_32 const & lhs,
             Uint32_32 const & rhs) {
    return !(lhs == rhs);
  }

  friend OFBool
  operator<(Uint32_32 const & lhs,
            Uint32_32 const & rhs) {
    if (lhs.high_ < rhs.high_) {
      return OFTrue;
    }

    if (lhs.high_ > rhs.high_) {
      return OFFalse;
    }

    return (lhs.low_ < rhs.low_);
  }

  friend OFBool
  operator<=(Uint32_32 const & lhs,
             Uint32_32 const & rhs) {
    return (lhs < rhs) || (lhs == rhs);
  }

  friend OFBool
  operator>(Uint32_32 const & lhs,
            Uint32_32 const & rhs) {
    return !(lhs <= rhs);
  }

  friend OFBool
  operator>=(Uint32_32 const & lhs,
             Uint32_32 const & rhs) {
    return !(lhs < rhs);
  }


// non-static(s)
// method(s)
public:
  Uint32_32();
  Uint32_32(Uint32_32 const & other);
  Uint32_32(Uint32 const value);

  Uint32_32& operator=(Uint32 const rhs);
  Uint32_32& hadd(Uint32 const rhs);
  Uint32_32& ladd(Uint32 const rhs);


// member(s)
public:
  Uint32 high_;
  Uint32 low_;
}; // Uint32_32


#endif // OFUUID_GENERATOR_H