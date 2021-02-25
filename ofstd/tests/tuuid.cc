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
 *  Author:  Uli Schlachter
 *
 *  Purpose: test program for OFUUID
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofuuid.h"

OFTEST(ofstd_OFUUID_1)
{
    /* Verify that two UUIDs are different and a UUID is equal to itself */
    OFUUID::BinaryRepresentation rep_a, rep_b;
    OFUUID a, b;

    OFCHECK(a != b);
    OFCHECK_EQUAL(a, a);
    OFCHECK_EQUAL(b, b);

    a.getBinaryRepresentation(rep_a);
    b.getBinaryRepresentation(rep_b);
    OFCHECK(memcmp(&rep_a.value[0], &rep_b.value[0], sizeof(rep_a.value)) != 0);
    OFCHECK(memcmp(&rep_a.value[0], &rep_a.value[0], sizeof(rep_a.value)) == 0);
    OFCHECK(memcmp(&rep_b.value[0], &rep_b.value[0], sizeof(rep_a.value)) == 0);

    /* This checks that the binary representation works */
    OFUUID c(rep_a);
    OFCHECK(b != c);
    OFCHECK_EQUAL(a, c);

    /* This checks that generate() really does generate a new UUID */
    c.generate();
    OFCHECK(a != c);
    OFCHECK(b != c);
}


OFTEST(ofstd_OFUUID_2)
{
    OFString str;
    OFUUID::BinaryRepresentation rep;
    /* This is just a randomly generated UUID */
    rep.value[ 0] = 0xba;
    rep.value[ 1] = 0xa0;
    rep.value[ 2] = 0x05;
    rep.value[ 3] = 0x5e;
    rep.value[ 4] = 0x20;
    rep.value[ 5] = 0xd4;
    rep.value[ 6] = 0x01;
    rep.value[ 7] = 0xe1;
    rep.value[ 8] = 0x80;
    rep.value[ 9] = 0x4a;
    rep.value[10] = 0x67;
    rep.value[11] = 0xc6;
    rep.value[12] = 0x69;
    rep.value[13] = 0x73;
    rep.value[14] = 0x51;
    rep.value[15] = 0xff;
    OFUUID a(rep);

    OFCHECK_EQUAL(a.toString(str, OFUUID::ER_RepresentationInteger), "248067283583015040850042404479733813759");
    OFCHECK_EQUAL(a.toString(str, OFUUID::ER_RepresentationHex), "baa0055e-20d4-01e1-804a-67c6697351ff");
    OFCHECK_EQUAL(a.toString(str, OFUUID::ER_RepresentationOID), "2.25.248067283583015040850042404479733813759");
    OFCHECK_EQUAL(a.toString(str, OFUUID::ER_RepresentationURN), "urn:uuid:baa0055e-20d4-01e1-804a-67c6697351ff");

    /* This example comes from the ITU-T X.667 and is mentioned in Section 8.
     * Since both the hexadecimal and the integer version is given, this mainly
     * tests if ER_RepresentationInteger works correctly.
     */
    rep.value[ 0] = 0xf8;
    rep.value[ 1] = 0x1d;
    rep.value[ 2] = 0x4f;
    rep.value[ 3] = 0xae;
    rep.value[ 4] = 0x7d;
    rep.value[ 5] = 0xec;
    rep.value[ 6] = 0x11;
    rep.value[ 7] = 0xd0;
    rep.value[ 8] = 0xa7;
    rep.value[ 9] = 0x65;
    rep.value[10] = 0x00;
    rep.value[11] = 0xa0;
    rep.value[12] = 0xc9;
    rep.value[13] = 0x1e;
    rep.value[14] = 0x6b;
    rep.value[15] = 0xf6;
    a = OFUUID(rep);

    OFCHECK_EQUAL(a.toString(str, OFUUID::ER_RepresentationInteger), "329800735698586629295641978511506172918");
    OFCHECK_EQUAL(a.toString(str, OFUUID::ER_RepresentationHex), "f81d4fae-7dec-11d0-a765-00a0c91e6bf6");
    OFCHECK_EQUAL(a.toString(str, OFUUID::ER_RepresentationOID), "2.25.329800735698586629295641978511506172918");
    OFCHECK_EQUAL(a.toString(str, OFUUID::ER_RepresentationURN), "urn:uuid:f81d4fae-7dec-11d0-a765-00a0c91e6bf6");
}
