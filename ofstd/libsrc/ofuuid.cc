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


#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofuuid.h"

#include "dcmtk/ofstd/ofdefine.h"
#include "dcmtk/ofstd/ofthread.h"
#include "dcmtk/ofstd/ofstd.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


static OFMutex UUIDMutex;
static Uint32 last_time[2];
static int uuids_this_tick;
static Uint16 last_clock_sequence;
static Uint8 last_node[6];
static OFBool initialized = OFFalse;


static void get_random(void *dest, size_t num)
{
    /* FIXME: We are supposed to use a cryptographic-quality random number
     * generator. However, finding a portable one is a little hard, so
     * rand() will do for now.
     */
    Uint8* ptr = OFreinterpret_cast(Uint8*, dest);
    while (num > 0) {
        *ptr = rand();
        num--;
        ptr++;
    }
}

static void get_node()
{
    /* FIXME: This is supposed to be a MAC address and we are supposed to
     * re-check the MAC address each time we generate a UUID and do some stuff
     * if the MAC changes.
     */
    get_random(&last_node[0], sizeof(last_node));
}

#ifdef _WIN32
static void get_system_time(Uint32 *out)
{
    ULARGE_INTEGER tm;

    GetSystemTimeAsFileTime(OFreinterpret_cast(FILETIME *, &tm));

    /* tm is number of 100ns ticks since Jan 01 1601, but we want the number of
     * ticks since Oct 15 1582 (since a new calendar system has been introduced
     * at that time).
     */
    tm.QuadPart += OFstatic_cast(unsigned __int64, 1000 * 1000 * 10) // seconds
        * OFstatic_cast(unsigned __int64, 60 * 60 * 24) // days
        * OFstatic_cast(unsigned __int64, 17 + 30 + 31 + 365 * 18 + 5); // number of days
    out[0] = tm.LowPart;
    out[1] = tm.HighPart;
}
#else
static void get_system_time(Uint32 *out)
{
    struct timeval tp;
    Uint32 sec_factor = 1000 * 1000 * 10;
    Uint32 add;
    Uint32 ah, al, bh, bl;

    gettimeofday(&tp, NULL);

    /* tp is offset from Jan 1 1970, but we want Oct 15 1582 */
    out[1] = 0x01B21DD2;
    out[0] = 0x13814000;

    add = tp.tv_usec * 10;
    if (OFStandard::check32BitAddOverflow(out[0], add))
        out[1]++;
    out[0] += add;

    // We have to add tp.tv_sec * sec_factor, but that doesn't fit into 32 bits.
    ah = tp.tv_sec >> 16;
    al = tp.tv_sec & 0xffff;
    bh = sec_factor >> 16;
    bl = sec_factor & 0xffff;

    // tv_sec * sec_factor = (ah * 2^16 + al) * (bh * 2^16 + bl)
    //  = ah * bh * 2^32 + (ah * bl + al * bh) * 2^16 + (al * bl)
    add = al*bl;
    if (OFStandard::check32BitAddOverflow(out[0], add))
        out[1]++;
    out[0] += add;

    out[1] += ah * bh;

    // We have to add (ah * bl + al * bh) * 2^16. First we add the lower
    // 16 bits of that summand to out[0], then we add the higher 16 bits
    // directly to out[1].
    add = (((ah * bl) + (al * bh)) & 0xffff) << 16;
    if (OFStandard::check32BitAddOverflow(out[0], add))
        out[1]++;
    out[0] += add;

    add = (((ah * bl) + (al * bh)) & 0xffff0000) >> 16;
    out[1] += add;
}
#endif

static void get_time(Uint32 *out)
{
    get_system_time(out);
    if (out[0] == last_time[0] && out[1] == last_time[1]) {
        uuids_this_tick++;
        /* FIXME: If this gets to high, we are supposed to busy loop. For now
         * let's assume that we aren't called that often. (See A.3.3)
         * FIXME: Do we have to handle the integer overflow? I think no since
         * the lowest bits should always be zero and if we fix the above FIXME,
         * this one should be gone, too.
         */
        out[0] += uuids_this_tick;
    } else
        uuids_this_tick = 0;
}

void OFUUID::generate()
{
    Uint32 system_time[2];
    Uint16 clock_sequence;

    /* See ITU-T X.667, Annex A */
    UUIDMutex.lock();

    if (!initialized) {
        get_node();
        get_random(&last_clock_sequence, sizeof(last_clock_sequence));
        initialized = OFTrue;
    }

    get_time(&system_time[0]);
    /* If time went backwards, increment clock sequence */
    if (system_time[0] < last_time[0] ||
            (system_time[0] == last_time[0] && system_time[1] < last_time[1]))
        last_clock_sequence++;
    clock_sequence = last_clock_sequence;

    last_time[0] = system_time[0];
    last_time[1] = system_time[1];

    UUIDMutex.unlock();

    /* Bits 0-31 of time (32 bits) */
    time_low = system_time[0];
    /* Bits 32-47 of time (16 bits) */
    time_mid = OFstatic_cast(Uint16, system_time[1] & 0xffff);
    /* Bits 48-59 of time (11 bits) */
    version_and_time_high = OFstatic_cast(Uint16, (system_time[1] >> 16) & 0xeff);
    /* Version number, bits 15 to 12 of version_and_time_high */
    version_and_time_high |= 0x100;
    /* Sequence, lowest 8 bits of clock_sequence */
    clock_seq_low = clock_sequence & 0xff;
    /* Bits 8-13 of clock_sequence */
    variant_and_clock_seq_high = (clock_sequence >> 8) & 0xcf;
    /* Version */
    variant_and_clock_seq_high |= 0x80;
    /* And the node value */
    memcpy(&node, &last_node, sizeof(node));
}

OFUUID::OFUUID()
: time_low(0),
  time_mid(0),
  version_and_time_high(0),
  variant_and_clock_seq_high(0),
  clock_seq_low(0),
  node()
{
    generate();
}

OFUUID::OFUUID(const struct BinaryRepresentation& rep)
: time_low(0),
  time_mid(0),
  version_and_time_high(0),
  variant_and_clock_seq_high(0),
  clock_seq_low(0),
  node()
{
    time_low = rep.value[0];
    time_low = time_low << 8 | rep.value[1];
    time_low = time_low << 8 | rep.value[2];
    time_low = time_low << 8 | rep.value[3];
    time_mid = rep.value[4];
    time_mid = time_mid << 8 | rep.value[5];
    version_and_time_high = rep.value[6];
    version_and_time_high = version_and_time_high << 8 | rep.value[7];
    variant_and_clock_seq_high = rep.value[8];
    clock_seq_low = rep.value[9];
    memcpy(&node[0], &rep.value[10], sizeof(node));
}

void OFUUID::getBinaryRepresentation(struct BinaryRepresentation& rep) const
{
    rep.value[0] = OFstatic_cast(Uint8, (time_low >> 24) & 0xff);
    rep.value[1] = OFstatic_cast(Uint8, (time_low >> 16) & 0xff);
    rep.value[2] = OFstatic_cast(Uint8, (time_low >>  8) & 0xff);
    rep.value[3] = OFstatic_cast(Uint8, (time_low >>  0) & 0xff);
    rep.value[4] = (time_mid >>  8) & 0xff;
    rep.value[5] = (time_mid >>  0) & 0xff;
    rep.value[6] = (version_and_time_high >> 8) & 0xff;
    rep.value[7] = (version_and_time_high >> 0) & 0xff;
    rep.value[8] = variant_and_clock_seq_high;
    rep.value[9] = clock_seq_low;
    memcpy(&rep.value[10], &node[0], sizeof(node));
}

OFString& OFUUID::toString(OFString& result, E_Representation representation) const
{
    OFOStringStream stream;
    print(stream, representation);
    OFSTRINGSTREAM_GETSTR(stream, result_ptr)
    result = result_ptr;
    OFSTRINGSTREAM_FREESTR(result_ptr);
    return result;
}

STD_NAMESPACE ostream& OFUUID::print(STD_NAMESPACE ostream& stream, E_Representation representation) const
{
    switch (representation) {
    case ER_RepresentationOID:
        stream << "2.25.";
        /* Fall through */
    case ER_RepresentationInteger:
        printInteger(stream);
        break;
    case ER_RepresentationURN:
        stream << "urn:uuid:";
        /* Fall through */
    case ER_RepresentationHex:
        printHex(stream);
        break;
    }

    return stream;
}

void OFUUID::printHex(STD_NAMESPACE ostream& stream) const
{
    STD_NAMESPACE ios_base::fmtflags flags = stream.flags(STD_NAMESPACE ios_base::hex);
    char fill_char = stream.fill('0');

    stream << STD_NAMESPACE setw(8) << time_low << "-";
    stream << STD_NAMESPACE setw(4) << time_mid << "-";
    stream << STD_NAMESPACE setw(4) << version_and_time_high << "-";
    stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, variant_and_clock_seq_high);
    stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, clock_seq_low) << "-";
    stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, node[0]);
    stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, node[1]);
    stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, node[2]);
    stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, node[3]);
    stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, node[4]);
    stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, node[5]);

    stream.flags(flags);
    stream.fill(fill_char);
}

static void divide_by(Uint32 n, Uint32 d, Uint32& res, Uint32& rem)
{
    // This calculates res = m / d and rem = m % d where m = n + rem * 2^32

    // First we do the higher half of the division
    Uint32 next = (rem << 16) | (n >> 16);
    Uint32 tmp = next / 10;
    rem = next % 10;

    // Then we do the same with the lower half
    next = (rem << 16) | (n & 0xffff);
    res = (next / 10) + (tmp << 16);
    rem = next % 10;
}

void OFUUID::printInteger(STD_NAMESPACE ostream& stream) const
{
    // Oh sh.., converting a 128-bit integer to its base 10 representation. Not funny.

    // A 128-bit int has at most 39 characters in base 10
    char buffer[40];
    // Our current 128-bit state
    Uint32 data[4];
    struct BinaryRepresentation representation;
    /* Current buffer index (we are generating the last character of the output
     * first, so we have write it into buffer in reverse order) */
    int idx = sizeof(buffer) - 1;

    // First we convert the 128-bit integer into four 32-bit integers
    getBinaryRepresentation(representation);
    for (int i = 0; i < 4; i++) {
        data[i]  = representation.value[0 + 4*i] << 24;
        data[i] |= representation.value[1 + 4*i] << 16;
        data[i] |= representation.value[2 + 4*i] <<  8;
        data[i] |= representation.value[3 + 4*i] <<  0;
    }

    if (data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 0) {
        // This should never happen, but we can still handle it! :-)
        stream << "0";
        return;
    }

    // As long as the result isn't 0, divide by 10 and print the remainder
    while (data[0] != 0 || data[1] != 0 || data[2] != 0 || data[3] != 0) {
        Uint32 rem = 0;
        divide_by(data[0], 10, data[0], rem);
        divide_by(data[1], 10, data[1], rem);
        divide_by(data[2], 10, data[2], rem);
        divide_by(data[3], 10, data[3], rem);

        assert(rem <= 9);
        buffer[--idx] = OFstatic_cast(char, rem + '0');
    }
    assert(idx >= 0);
    buffer[sizeof(buffer) - 1] = '\0';
    stream << &buffer[idx];
}

OFBool OFUUID::operator==(const OFUUID& o) const
{
    struct BinaryRepresentation own;
    struct BinaryRepresentation other;
    getBinaryRepresentation(own);
    o.getBinaryRepresentation(other);

    return memcmp(&own, &other, sizeof(struct BinaryRepresentation)) == 0;
}
