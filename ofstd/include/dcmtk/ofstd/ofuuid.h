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

#ifndef OFUUID_H
#define OFUUID_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofrand.h"
#include "dcmtk/ofstd/ofthread.h"

#include <iostream>

/** Helper class for generating and storing UUIDs, as specified in ITU-T X.667.
 *  A UUID is an Universally Unique IDentifier. If UUIDs are generated
 *  correctly, it's almost impossible that the same UUID is generated twice.
 *  @warning The implementation of this class still contains a few "FIXME"
 *    comments and should, therefore, not be used in productive environments
 *    as long as these corrections have not yet been implemented.
 */
struct DCMTK_OFSTD_EXPORT OFUUID {
// type(s)
public:
  /** The possible notations of a UUID. */
  typedef enum {
    /** The UUID is represented as one, long integer in base 10 (up to 39 digits long) */
    NotationInteger,

    /** The UUID is represented in hexadecimal notation with hyphens (-) separating groups */
    NotationHex,

    /** The UUID is represented as one, long integer with the prefix "2.25." */
    NotationOID,

    /** The UUID is represented in hexadecimal notation with the prefix "urn:uuid:" */
    NotationURN,

    /** The default representation that is used when none is given */
    NotationDefault = NotationHex
  } Notation;


// friend(s)
public:
  friend class OFUUIDGenerator;

  friend STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream& os, const OFUUID& uuid);

  friend OFBool operator==(OFUUID const & lhs, OFUUID const & rhs);
  friend OFBool operator!=(OFUUID const & lhs,OFUUID const & rhs);

  friend OFBool operator<(OFUUID const & lhs, OFUUID const & rhs);
  friend OFBool operator<=(OFUUID const & lhs, OFUUID const & rhs);

  friend OFBool operator>(OFUUID const & lhs, OFUUID const & rhs);
  friend OFBool operator>=(OFUUID const & lhs, OFUUID const & rhs);


// static(s)
// member(s)
  static size_t const DATA_LENGTH = 16;

  
// method(s)
  static void divide_by_10(Uint8 n, Uint8& q, Uint8& r);


// non-static(s)
// method(s)
public:
  OFUUID(OFUUID const & other);
  OFUUID& operator=(OFUUID const & rhs);
  ~OFUUID();

  /** Write the string representation of this UUID to a stream.
   *  @param stream the output stream to write to
   *  @param notation the Notation to use
   *  @return stream
   */
  STD_NAMESPACE ostream& print(STD_NAMESPACE ostream& stream, Notation const notation = NotationDefault) const;

  /** Get the string representation of this UUID.
   *  @param result string instance to save the result in
   *  @param representation the representation to use
   *  @return result
   */
  OFString toString(Notation notation = OFUUID::NotationDefault) const;

private:
  OFUUID();

  OFBool isNil() const;
  
  /** Print the UUID in hexadecimal notation to the given stream.
   *  @param stream stream to print to.
   */
  void printHex(STD_NAMESPACE ostream& os) const;

  /** Print the UUID in integer notation to the given stream.
   *  @param stream stream to print to
   */
  void printInteger(STD_NAMESPACE ostream& os) const;


// member(s)
private:
  Uint8 data_[DATA_LENGTH];
}; // OFUUID

#endif // OFUUID_H
