/*
 *
 *  Copyright (C) 2011-2013, OFFIS e.V.
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
 *  Author:  Uli Schlachter
 *
 *  Purpose: Definitions for generating UUIDs, as defined by ITU-T X.667
 *
 */

#ifndef OFUUID_H
#define OFUUID_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstream.h"

/** Helper class for generating and storing UUIDs, as specified in ITU-T X.667.
 *  A UUID is an Universally Unique IDentifier. If UUIDs are generated
 *  correctly, it's almost impossible that the same UUID is generated twice.
 */
class DCMTK_OFSTD_EXPORT OFUUID {
public:

    /** The possible ways to represent a UUID */
    enum E_Representation {
        /** The UUID is printed as one, long integer in base 10 (up to 39 digits long) */
        ER_RepresentationInteger,
        /** The UUID is printed in hexadecimal notation with hyphens (-) separating groups */
        ER_RepresentationHex,
        /** The UUID is printed as one, long integer with the prefix "2.25." */
        ER_RepresentationOID,
        /** The UUID is printed in hexadecimal notation with the prefix "urn:uuid:" */
        ER_RepresentationURN,
        /** The default representation that is used when none is given */
        ER_RepresentationDefault = ER_RepresentationHex
    };

    struct BinaryRepresentation {
        Uint8 value[16];
    };

    /** Default constructor. Generates a new UUID. */
    OFUUID();

    /** Construct a new UUID from its binary representation.
     *  @param val the binary representation
     *  @see getBinaryRepresentation
     */
    OFUUID(const struct BinaryRepresentation& val);

    /* This class uses the default assignment operator */

    /** Generate a new UUID. The old UUID is discarded. */
    void generate();

    /** Get the string representation of this UUID.
     *  @param result string instance to save the result in
     *  @param representation the representation to use
     *  @return result
     */
    OFString& toString(OFString& result, E_Representation representation = ER_RepresentationDefault) const;

    /** Write the string representation of this UUID to a stream.
     *  @param stream the output stream to write to
     *  @param representation the representation to use
     *  @return stream
     */
    STD_NAMESPACE ostream& print(STD_NAMESPACE ostream& stream, E_Representation representation = ER_RepresentationDefault) const;

    /** Get the binary representation of this UUID.
     *  @param val the structure where the result should be saved to
     */
    void getBinaryRepresentation(struct BinaryRepresentation& val) const;

    /** Compare this instance to another OFUUID instance.
     *  @param other the other instance
     *  @return OFTrue if both UUIDs are equal.
     */
    OFBool operator==(const OFUUID& other) const;

    /** Compare this instance to another OFUUID instance.
     *  @param other the other instance
     *  @return OFFalse if both UUIDs are equal.
     */
    OFBool operator!=(const OFUUID& other) const
    {
        return !(*this == other);
    }

private:

    /** Print the integer representation to the given stream.
     *  @param stream stream to print to.
     */
    void printInteger(STD_NAMESPACE ostream& stream) const;

    /** Print the hexadecimal representation to the given stream.
     *  @param stream stream to print to.
     */
    void printHex(STD_NAMESPACE ostream& stream) const;

    /* The fields of an UUID, as defined by ITU-T X.667 */

    /** Octets 0-3 of the time field */
    Uint32 time_low;
    /** Octets 4-5 of the time field */
    Uint16 time_mid;
    /** 4 bits for the version and the 12 highest bits of the time */
    Uint16 version_and_time_high;
    /** 2 bits for the variant and the 6 highest bits of the clock sequence */
    Uint8 variant_and_clock_seq_high;
    /** The lowest 8 bits of the clock sequence */
    Uint8 clock_seq_low;
    /** The node value in the form of a MAC address */
    Uint8 node[6];

};

/** Print an UUID to a stream with OFUUID::ER_RepresentationDefault.
 *  @param stream the output stream to use
 *  @param uuid the UUID to print
 *  @return stream
 */
static inline STD_NAMESPACE ostream& operator<< (STD_NAMESPACE ostream& stream, const OFUUID& uuid)
{
    return uuid.print(stream);
}

#endif
