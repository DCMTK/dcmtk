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
 *  Purpose: Definition of a Universally Unique IDentifier (UUID) Generator.
 *
 */

#ifndef OFUUID_GENERATOR_H
#define OFUUID_GENERATOR_H

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofdefine.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/ofthread.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofuuid.h"

/**
 *  The OFUUIDGenerator class provides a means to create an OFUUID.
 */
class DCMTK_OFSTD_EXPORT OFUUIDGenerator {
public:
  /**
   *  The type of a user providable random number generator (RNG) functor.
   */
  typedef Uint8 (*rnd8)();

  /**
   *  Creates an OFUUID based on
   *  <a href="https://www.rfc-editor.org/info/rfc9562">RFC 9562</a>, section 5.7.
   *  @param rnd A random number generator (RNG) functor. Provide if special
   *  requirements with respect to unguessability of the to be created OFUUID
   *  exist. See <a href="https://www.rfc-editor.org/info/rfc9562">RFC 9562</a>,
   *  section 6.9 for details.
   *
   *  @return A OFshared_ptr encapsulating a newly created OFUUID instance.
   *  @warning The returned object may contain a null pointer (@ref
   * OFnullptr_t).
   */
  static OFshared_ptr<OFUUID> create(rnd8 const & = getRND8);

private:
  /**
   *  Helper type for internal use in @ref OFUUIDGenerator.
   *  Encapsulating two Uint32 members to mimic an Uint64. These members are
   *  referred to as high, i.e. most significant, and low, i.e. least
   *  significant, respectively.
   */
  class DCMTK_OFSTD_EXPORT Uint32_32 {
  public:
    /**
     *  Equality operator. Interpret Uint32_32 as Uint64.
     *  @param rhs The object to compare with.
     *
     *  @return OFTrue iff \p this and \p rhs are equal.
     */
    OFBool operator==(Uint32_32 const & rhs) const;

    /**
     *  Inequality operator. Interpret Uint32_32 as Uint64.
     *  @param rhs The object to compare with.
     *
     *  @return @ref OFTrue iff \p this and \p rhs are unequal.
     */
    OFBool operator!=(Uint32_32 const & rhs) const;

    /**
     *  Less than operator. Interpret Uint32_32 as Uint64.
     *  @param rhs The object to compare with.
     *
     *  @return OFTrue iff \p this is smaller than \p rhs.
     */
    OFBool operator<(Uint32_32 const & rhs) const;

    /**
     *  Less than or equal to operator. Interpret Uint32_32 as Uint64.
     *  @param rhs The object to compare with.
     *
     *  @return OFTrue iff \p this is smaller than or equal to \p rhs.
     */
    OFBool operator<=(Uint32_32 const & rhs) const;

    /**
     *  Greater than operator. Interpret Uint32_32 as Uint64.
     *  @param rhs The object to compare with.
     *
     *  @return OFTrue iff \p this is bigger than \p rhs.
     */
    OFBool operator>(Uint32_32 const & rhs) const;

    /**
     *  Greater than or equal to operator. Interpret Uint32_32 as Uint64.
     *  @param rhs The object to compare with.
     *
     *  @return OFTrue iff \p this is bigger than or equal to \p rhs.
     */
    OFBool operator>=(Uint32_32 const & rhs) const;

    /**
     *  Default constructor.
     */
    Uint32_32();

    /**
     *  Destructor.
     */
    virtual ~Uint32_32();

    /**
     *  Copy constructor.
     */
     Uint32_32(Uint32_32 const & other);

    /**
     *  Custom constructor.
     *  Creates a Uint32_32 with @ref low_ set to value and \ref high_ set to 0.
     *  @param value Value to be assigned to @ref low_.
     */
    Uint32_32(Uint32 const value);

    /**
     *  Custom assignment operator.
     *  Assigns a value to @ref low_ and resets @ref high_ to 0.
     *  @param value Value to be assigned to @ref low_.
     *
     *  @return A reference to \p *this.
     */
    Uint32_32 & operator=(Uint32 const value);

    /**
     *  Adds a value to @ref high_.
     *  @param value Value to be added to @ref high_.
     *
     *  @return A reference to \p *this.
     */
    Uint32_32 & hadd(Uint32 const value);

    /**
     *  Adds a value to @ref low_.
     *  @param value Value to be added to @ref low_.
     *
     *  @return A reference to \p *this.
     */
    Uint32_32 & ladd(Uint32 const value);

    /**
     *  Most significant member.
     */
    Uint32 high_;

    /**
     *  Least significant member.
     */
    Uint32 low_;
  }; // Uint32_32

  /**
   *  Calculates the number of milliseconds since January 1, 1970.
   *  @return The number of milliseconds calculated.
   *
   *  @warning The result "is affected by discontinuous jumps in the system time
   *  (e.g., if the system administrator manually changes the system time)."
   *  gettimeofday(2)
   */
  static Uint32_32 getMillisecondsSinceEpoch();

  /**
   *  Wrapper for @ref OFRandom::getRND8.
   *  @return A random Uint8 value.
   */
  static Uint8 getRND8();

  /**
   *  Checks whether initialization is necessary and performs it if required.
   *  @return OFTrue iff initialization succeeded.
   */
  static OFBool initializeIfNecessary();

  /**
   *  Sets up the individual fields (@ref OFUUID::data_) of an OFUUID.
   *  @param[in,out] ofuuid The to be assembled OFUUID.
   *  @param[in] rnd The random number genaratr functor to use.
   */
  static void setupOFUUIDDataMember(OFUUID * const ofuuid, rnd8 const & rnd);

  static Uint8 const OFUUID_VARIANT_MASK = 0x80; // cf. RFC9562, section 4.1
  static Uint8 const OFUUID_VERSION_MASK = 0x70; // cf. RFC9562, section 4.2

  static Uint32_32 last_ms_since_epoch_;
  static Uint16 rand_b_field_;

  /**
   *  Default constructor.
   */
  OFUUIDGenerator();

  /**
   *  Destructor
   */
  virtual ~OFUUIDGenerator();
}; // OFUUIDGenerator

#endif // OFUUID_GENERATOR_H
