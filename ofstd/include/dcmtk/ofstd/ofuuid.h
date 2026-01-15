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
 *  Author:  Uli Schlachter, Harald Roesen
 *
 *  Purpose: Definition of a Universally Unique IDentifier (UUID), based on RFC 9562., section 5.7.
 *
 */

#ifndef OFUUID_H
#define OFUUID_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofrand.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/oftypes.h"


/**
 * A OFUUID represents a Universally Unique Identifier (UUID) as described in
 * <a href="https://www.rfc-editor.org/info/rfc9562"> RFC 9562</a>, section 5.7.
 */
class DCMTK_OFSTD_EXPORT OFUUID {
public:
  friend class OFUUIDGenerator;

  /**
   *  Writes a OFUUID to a stream in canonical notation.
   *  @param os The stream to write to.
   *  @param uuid The OFUUID to write.
   *
   *  @return The stream written to.
   */
  friend STD_NAMESPACE ostream & operator<<(STD_NAMESPACE ostream & os, OFUUID const & uuid);

  /**
   *  Equality operator.
   *  @param lhs The left OFUUID for comparison.
   *  @param rhs The right OFUUID for comparison.
   *
   *  @return OFTrue iff lhs' and rhs' integer notations are mathematically equal.
   */
  friend OFBool operator==(OFUUID const & lhs, OFUUID const & rhs);

  /**
   *  Inequality operator.
   *  @param lhs The left OFUUID for comparison.
   *  @param rhs The right OFUUID for comparison.
   *
   *  @return OFTrue iff lhs' and rhs' integer notations are mathematically unequal.
   */
  friend OFBool operator!=(OFUUID const & lhs,OFUUID const & rhs);

  /**
   *  Less than operator.
   *  @param lhs The left OFUUID for comparison.
   *  @param rhs The right OFUUID for comparison.
   *
   *  @return OFTrue iff lhs' integer notation is mathematically smaller than rhs' integer notation.
   */
  friend OFBool operator<(OFUUID const & lhs, OFUUID const & rhs);

  /**
   *  Less than or equal to operator.
   *  @param lhs The left OFUUID for comparison.
   *  @param rhs The right OFUUID for comparison.
   *
   *  @return OFTrue iff lhs' integer notation is mathematically less than or equal to rhs' integer notation.
   */
  friend OFBool operator<=(OFUUID const & lhs, OFUUID const & rhs);

  /**
   *  Greater than operator.
   *  @param lhs The left OFUUID for comparison.
   *  @param rhs The right OFUUID for comparison.
   *
   *  @return OFTrue iff lhs' integer notation is mathematically bigger than rhs' integer notation.
   */
  friend OFBool operator>(OFUUID const & lhs, OFUUID const & rhs);

  /**
   *  Greater than or equal to operator.
   *  @param lhs The left OFUUID for comparison.
   *  @param rhs The right OFUUID for comparison.
   *
   *  @return OFTrue iff lhs' integer notation is mathematically bigger than or equal to rhs' integer notation.
   */
  friend OFBool operator>=(OFUUID const & lhs, OFUUID const & rhs);

  /** The possible notations of a OFUUID. */
  typedef enum {
    /** The OFUUID is represented as one, long (unsigned) integer in base 10 (up to 39 digits long). */
    NotationInteger,

    /**
     * The OFUUID is represented in the canonical "hex-and-dash" format consisting of multiple
     * groups of lowercase alphanumeric hexadecimal characters separated by single dashes/hyphens.
    */
    NotationCanonical,

    /**
     *  The OFUUID is represented in integer notation with the prefix <em>2.25.</em>
     */
    NotationOID,

    /** The OFUUID is represented in canonical notation with the prefix <em>urn:uuid:</em>. */
    NotationURN,

    /** The default notation that is used when none is given. */
    NotationDefault = NotationCanonical
  } Notation;

  /**
   *  A OFFUID with all bits set to zero (0).
   */
  static OFUUID const nil;

  /**
   *  A OFFUID with all bits set to one (1).
   */
  static OFUUID const max;

  /**
   * Destructor.
   */
  virtual ~OFUUID();

  /**
   *  Writes a string representation of this OFUUID to a stream.
   *  @param os The stream to write to.
   *  @param notation The Notation to use.
   *
   *  @return os
   */
  STD_NAMESPACE ostream & print(STD_NAMESPACE ostream & os, Notation const notation = NotationDefault) const;

  /**
   *  Returns a string representation of this OFUUID.
   *  @param notation The notation to use.
   *
   *  @return An OFString representing the current OFUUID.
   */
  OFString toString(Notation const notation = OFUUID::NotationDefault) const;

private:
  /**
   *  Given:
   *  \code{.cc}
   *  Uint8 m = n + r * 2^8;
   *  \endcode
   *
   *  This calculates the integer quotient q := m / 10 and
   *  the integer remainder r := m % 10.
   *  @param[in] n     The first summand of m (see above).
   *  @param[out] q    The calculated quotient.
   *  @param[in,out] r
   *  A factor of the second summand of m (see above). [in]\n
   *  The calculated remainder. [out]
   */
  static void divideBy10(Uint8 const n, Uint8 & q, Uint8 & r);

  /**
   *  The number of Uint8 octets that make up this OFUUID.
   */
  static size_t const DATA_LENGTH = 16;

  /**
   *  Default constructor.
   *
   *  To create a OFUUID use OFUUIDGenerator::create().
   */
  OFUUID(Uint8 const fill = 0);

  /**
   *  Copy constructor.
   */
  OFUUID(OFUUID const & other);

  /**
   *  Copy assignment.
   */
  OFUUID & operator=(OFUUID const & rhs);

  /**
   *  Checks if this OFUUID is equal to @ref OFFUID::max.*
   *  See <a href="https://www.rfc-editor.org/info/rfc9562"> RFC 9562</a>, section 5.10 for details.
   *
   *  @return OFTrue iff all bits of data_ are one (1).
   */
  OFBool isMax() const;

  /**
   *  Checks if this OFUUID is equal to @ref OFFUID::max.
   *  See <a href="https://www.rfc-editor.org/info/rfc9562"> RFC 9562</a>, section 5.9 for details.
   *
   *  @return OFTrue iff all bits of data_ are zero (0).
   */
  OFBool isNil() const;

  /**
   *  Writes the OFUUID in canonical notation to a stream.
   *  @param os The stream to write to.
   */
  void printCanonical(STD_NAMESPACE ostream & os) const;

  /**
   *  Writes the OFUUID in integer notation to a stream.
   *  @param os The stream to write to.
   */
  void printInteger(STD_NAMESPACE ostream & os) const;

  /**
   *  The data that makes up an OFUUID.
   */
  Uint8 data_[DATA_LENGTH];
}; // OFUUID

#endif // OFUUID_H
