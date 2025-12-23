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


#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofuuid.h"
#include "dcmtk/ofstd/ofdefine.h"
#include "dcmtk/ofstd/ofstd.h"


STD_NAMESPACE ostream&
operator<< (STD_NAMESPACE ostream& os,
            const OFUUID& ofuuid) {
  return ofuuid.print(os);               
}

OFBool
operator==(OFUUID const & lhs,
           OFUUID const & rhs) {
  for (size_t i = 0; i < OFUUID::DATA_LENGTH; ++i) {
    if (lhs.data_[i] != rhs.data_[i]) {
      return OFFalse;
    }
  }

  return OFTrue;
}

OFBool
operator!=(OFUUID const & lhs,
           OFUUID const & rhs) {
  return !(lhs == rhs);
}

OFBool
operator<(OFUUID const & lhs,
          OFUUID const & rhs) {
  for (size_t i = 0; i < OFUUID::DATA_LENGTH; ++i) {
    if (lhs.data_[i] < rhs.data_[i]) {
      return OFTrue;
    }

    if (lhs.data_[i] > rhs.data_[i]) {
      return OFFalse;
    }
  }

  return OFFalse;
}

OFBool
operator<=(OFUUID const & lhs,
           OFUUID const & rhs) {
  return (lhs < rhs) || (lhs == rhs);
}

OFBool
operator>(OFUUID const & lhs,
          OFUUID const & rhs) {
  return !(lhs <= rhs);
}

OFBool
operator>=(OFUUID const & lhs,
           OFUUID const & rhs) {
  return !(lhs < rhs);
}


OFUUID::OFUUID() {
  // DELIBERATELY EMPTY
}

OFUUID::OFUUID(OFUUID const & other) {
  assert(sizeof(data_) == sizeof(other.data_));
  memcpy(data_, other.data_, sizeof(data_));
}

OFUUID&
OFUUID::operator=(OFUUID const & rhs) {
  if (this == &rhs) {
    return *this;
  }
  
  for (size_t i = 0; i < OFUUID::DATA_LENGTH; ++i) {
    data_[i] = rhs.data_[i];
  }

  return *this;
}

OFUUID::~OFUUID() {
  // DELIBERATELY EMPTY  
}

void
OFUUID::divide_by_10(Uint8 const n,
                        Uint8 & q,
                        Uint8 & r) {
    // This calculates q = m / d and r = m % d where m := n + r * 2^8

    // First we do the higher half of the division
    Uint8 next = (r << 4) | (n >> 4);
    Uint8 tmp = next / 10;
    r = next % 10;

    // Then we do the same with the lower half
    next = (r << 4) | (n & 0x0f);
    q = (next / 10) + (tmp << 4);
    r = next % 10;
}


OFBool
OFUUID::isNil() const {
  for (size_t i = 0; i < OFUUID::DATA_LENGTH; ++i) {
    if (data_[i] != 0) {
      return OFFalse;
    }
  }

  return OFTrue;
}

STD_NAMESPACE ostream&
OFUUID::print(STD_NAMESPACE ostream & os,
                 Notation const notation) const {
  switch (notation) {
  case NotationOID:
    os << "2.25.";
    // Fall through !
  
  case NotationInteger:
    printInteger(os);
    break;
  
  case NotationURN:
    os << "urn:uuid:";
    // Fall through !    

  case NotationHex:
    printHex(os);
    break;

  default:
    // We should never get here.
    assert(OFFalse);    
  }

  return os;
}

void
OFUUID::printHex(STD_NAMESPACE ostream& os) const {
  // set new 'os' state and save 'os' old one
  STD_NAMESPACE ios_base::fmtflags const osFlags = os.flags(STD_NAMESPACE ios_base::hex);
  unsigned char const osFillChar = os.fill('0');

  size_t i = 0;
  for (/* EMPTY */; i < 4; ++i) {
    os << STD_NAMESPACE setw(2) << OFstatic_cast(int, data_[i]);
  }
  os << '-';
  for (/* EMPTY */; i < 6; ++i) {
    os << STD_NAMESPACE setw(2) << OFstatic_cast(int, data_[i]);
  }
  os << '-';
  for (/* EMPTY */; i < 8; ++i) {
    os << STD_NAMESPACE setw(2) << OFstatic_cast(int, data_[i]);
  }
  os << '-';
  for (/* EMPTY */; i < 10; ++i) {
    os << STD_NAMESPACE setw(2) << OFstatic_cast(int, data_[i]);
  }
  os << '-';
  for (/* EMPTY */; i < OFUUID::DATA_LENGTH; ++i) {
    os << STD_NAMESPACE setw(2) << OFstatic_cast(int, data_[i]);
  }
  
  // restore old 'os' state
  os.flags(osFlags);
  os.fill(osFillChar);
}

void
OFUUID::printInteger(STD_NAMESPACE ostream& os) const {
  if (isNil()) {
    os << "0";

    return;
  }

  OFUUID copy(*this);
  
  // A 128-bit integer has at most 39 characters in base 10. 
  // Plus the usual trailing '\0' sums up to 40 characters.
  unsigned char chars[40];
  int idx = OFstatic_cast(int, sizeof(chars)) - 1;
  while (!copy.isNil()) {
    Uint8 r = 0;
    for (size_t i = 0; i < DATA_LENGTH; ++i) {
      divide_by_10(copy.data_[i], copy.data_[i], r);
    }
    chars[--idx] = OFstatic_cast(unsigned char, (r + '0'));
  }
  chars[sizeof(chars) - 1] = '\0';
  os << &chars[idx];
}

OFString
OFUUID::toString(Notation notation) const {
  OFOStringStream osstream;
  print(osstream, notation);
  OFString s;
  OFSTRINGSTREAM_GETSTR(osstream, string_ptr)
  s = string_ptr;
  OFSTRINGSTREAM_FREESTR(string_ptr);
  
  return s;
}
