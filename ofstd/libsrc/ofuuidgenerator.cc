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

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofuuidgenerator.h"


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
OFMutex OFUUIDGenerator::mutex_;
Uint16 OFUUIDGenerator::rand_b_field_ = 0;
OFRandom OFUUIDGenerator::rnd_ = OFRandom();


OFUUIDGenerator::OFUUIDGenerator() {
  // DELIBERATELY EMPTY
}

OFUUID
OFUUIDGenerator::create(rnd8 const & rnd) {
  // See ITU-T X.667, Annex A
  mutex_.lock();

  OFUUIDGenerator::initialize();

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

  mutex_.unlock();

  OFUUID ofuuid;
  setupOFUUIDDataMember(ofuuid, rnd);

  return ofuuid;
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
  Uint32 ah = OFstatic_cast(Uint32, tp.tv_sec >> 16);
  Uint32 const al = tp.tv_sec & 0xffff;
  Uint32 const bh = 1000 >> 16;    // milliseconds per second
  Uint32 const bl = 1000 & 0xffff; // milliseconds per second

  Uint32 add = al * bl;
  // if (OFStandard::check32BitAddOverflow(ms_since_unix_epoch.low_, add)) {
  //   ms_since_unix_epoch.high_ += 1;
  // }
  // ms_since_unix_epoch.low_ += add;
  ms_since_unix_epoch.ladd(add);

  // ms_since_unix_epoch.high_ += (ah * bh);
  ms_since_unix_epoch.hadd(ah * bh);

  add = (((ah * bl) + (al * bh)) & 0x0000ffff) << 16;
  // if (OFStandard::check32BitAddOverflow(ms_since_unix_epoch.low_, add)) {
  //   ms_since_unix_epoch.high_ += 1;
  // }
  // ms_since_unix_epoch.low_ += add;
  ms_since_unix_epoch.ladd(add);

  add = (((ah * bl) + (al * bh)) & 0xffff0000) >> 16;
  // ms_since_unix_epoch.high_ += add;
  ms_since_unix_epoch.hadd(add);

#endif

  return ms_since_unix_epoch;
}

Uint8
OFUUIDGenerator::getRND8() {
  return OFstatic_cast(Uint8, OFUUIDGenerator::rnd_.getRND16());
}

OFBool
OFUUIDGenerator::initialize() {
  if (OFUUIDGenerator::last_ms_since_epoch_ != 0) {
    return OFTrue;
  }

  OFUUIDGenerator::last_ms_since_epoch_ = OFUUIDGenerator::getMillisecondsSinceEpoch();

  return (OFUUIDGenerator::last_ms_since_epoch_ != 0);
}


void
OFUUIDGenerator::setupOFUUIDDataMember(OFUUID & ofuuid,
                                       rnd8 const & rnd) {
  // [ofuuid.data_[0], ofuuid.data_[1]] := last_ms_since_epoch_.high_& 0x00ff
  // Only the lower 16 bits are used, see mask the upper ones.  (cf. RFC9562 section 5.7)
  Uint32 value = OFUUIDGenerator::last_ms_since_epoch_.high_;
  for (int i = 1; i >=0 ; --i) {
    ofuuid.data_[OFstatic_cast(size_t, i)] = OFstatic_cast(Uint8, value);
    value >>= 8;
  }

  // [ofuuid.data_[2], ofuuid.data_[5]] := last_ms_since_epoch_.low_
  // All 32 bits are used.  (cf. RFC9562 section 5.7)
  value = OFUUIDGenerator::last_ms_since_epoch_.low_;
  for (int i = 5; i >= 2; --i) {
    ofuuid.data_[OFstatic_cast(size_t, i)] = OFstatic_cast(Uint8, value);
    value >>= 8;
  }

  // [ofuuid.data_[6], ofuuid.data_[7]] := OFUUIDGenerator::rand_b_field_,
  // except for the version bits - see below.
  value = OFUUIDGenerator::rand_b_field_;
  for (int i = 7; i >= 6; --i) {
    ofuuid.data_[OFstatic_cast(size_t, i)] = OFstatic_cast(Uint8, value);
    value >>= 8;
  }

  // The four (4) most significant bits of ofuuid.data_[6] will contain the UUID
  // version  (cf. RFC9562 sections 4.2 and 5.7)
  (ofuuid.data_[6] &= 0x0f) |= OFUUIDGenerator::OFUUID_VERSION_MASK;

  // [ofuuid.data_[8], ofuuid.data_[15]] are filled with random data, except for
  // the variant bits - see below.
  for (size_t i = 8; i < 16; ++i) {
    ofuuid.data_[i] = rnd(); //OFstatic_cast(Uint8, OFUUIDGenerator::rnd_.getRND16());
  }

  // The two (2) most significant bits of ofuuid.data_[8] will contain the UUID
  // variant  (cf. RFC9562 sections 4.2 and 5.7)
  (ofuuid.data_[8] &= 0x3f) |= OFUUIDGenerator::OFUUID_VARIANT_MASK;
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

OFUUIDGenerator::Uint32_32&
OFUUIDGenerator::Uint32_32::operator=(Uint32 const rhs) {
  high_ = 0;
  low_ = rhs;

  return *this;
}

OFUUIDGenerator::Uint32_32&
OFUUIDGenerator::Uint32_32::hadd(Uint32 const value) {
  high_ += value;

  return *this;
}

OFUUIDGenerator::Uint32_32&
OFUUIDGenerator::Uint32_32::ladd(Uint32 const value) {
  if (OFStandard::check32BitAddOverflow(low_, value)) {
    ++high_;
  }
  low_ += value;

  return *this;
}


// static OFMutex UUIDMutex;
// static Uint32 last_time[2];
// static int uuids_this_tick;
// static Uint16 last_clock_sequence;
// static Uint8 last_node[6];
// static OFBool initialized = OFFalse;


// static void get_random(OFRandom &rnd, void *dest, size_t num)
// {
//     Uint8* ptr = OFreinterpret_cast(Uint8*, dest);
//     while (num > 0) {
//         *ptr++ = OFstatic_cast(Uint8, rnd.getRND16());
//         num--;
//     }
// }

// static void get_node(OFRandom &rnd)
// {
//     /* FIXME: This is supposed to be a MAC address and we are supposed to
//      * re-check the MAC address each time we generate a UUID and do some stuff
//      * if the MAC changes.
//      */
//     get_random(rnd, &last_node[0], sizeof(last_node));
// }

// #ifdef _WIN32
// static void get_system_time(Uint32 *out)
// {
//     ULARGE_INTEGER tm;

//     GetSystemTimeAsFileTime(OFreinterpret_cast(FILETIME *, &tm));

//     /* tm is number of 100ns ticks since Jan 01 1601, but we want the number of
//      * ticks since Oct 15 1582 (since a new calendar system has been introduced
//      * at that time).
//      */
//     tm.QuadPart += OFstatic_cast(unsigned __int64, 1000 * 1000 * 10) // seconds
//         * OFstatic_cast(unsigned __int64, 60 * 60 * 24) // days
//         * OFstatic_cast(unsigned __int64, 17 + 30 + 31 + 365 * 18 + 5); // number of days
//     out[0] = tm.LowPart;
//     out[1] = tm.HighPart;
// }
// #else
// static void get_system_time(Uint32 *out)
// {
//     struct timeval tp;
//     Uint32 sec_factor = 1000 * 1000 * 10;
//     Uint32 add;
//     Uint32 ah, al, bh, bl;

//     gettimeofday(&tp, NULL);

//     /* tp is offset from Jan 1 1970, but we want Oct 15 1582 */
//     out[1] = 0x01B21DD2;
//     out[0] = 0x13814000;

//     add = OFstatic_cast(Uint32, tp.tv_usec * 10);
//     if (OFStandard::check32BitAddOverflow(out[0], add))
//         out[1]++;
//     out[0] += add;

//     // We have to add tp.tv_sec * sec_factor, but that doesn't fit into 32 bits.
//     ah = OFstatic_cast(Uint32, tp.tv_sec >> 16);
//     al = tp.tv_sec & 0xffff;
//     bh = sec_factor >> 16;
//     bl = sec_factor & 0xffff;

//     // tv_sec * sec_factor = (ah * 2^16 + al) * (bh * 2^16 + bl)
//     //  = ah * bh * 2^32 + (ah * bl + al * bh) * 2^16 + (al * bl)
//     add = al*bl;
//     if (OFStandard::check32BitAddOverflow(out[0], add))
//         out[1]++;
//     out[0] += add;

//     out[1] += ah * bh;

//     // We have to add (ah * bl + al * bh) * 2^16. First we add the lower
//     // 16 bits of that summand to out[0], then we add the higher 16 bits
//     // directly to out[1].
//     add = (((ah * bl) + (al * bh)) & 0xffff) << 16;
//     if (OFStandard::check32BitAddOverflow(out[0], add))
//         out[1]++;
//     out[0] += add;

//     add = (((ah * bl) + (al * bh)) & 0xffff0000) >> 16;
//     out[1] += add;
// }
// #endif

// static void get_time(Uint32 *out)
// {
//     get_system_time(out);
//     if (out[0] == last_time[0] && out[1] == last_time[1]) {
//         uuids_this_tick++;
//         /* FIXME: If this gets to high, we are supposed to busy loop. For now
//          * let's assume that we aren't called that often. (See A.3.3)
//          * FIXME: Do we have to handle the integer overflow? I think no since
//          * the lowest bits should always be zero and if we fix the above FIXME,
//          * this one should be gone, too.
//          */
//         out[0] += uuids_this_tick;
//     } else
//         uuids_this_tick = 0;
// }

// void OFUUID::generate()
// {
//     Uint32 system_time[2];
//     Uint16 clock_sequence;

//     /* See ITU-T X.667, Annex A */
//     UUIDMutex.lock();

//     if (!initialized) {
//         get_node(rnd);
//         get_random(rnd, &last_clock_sequence, sizeof(last_clock_sequence));
//         initialized = OFTrue;
//     }

//     get_time(&system_time[0]);
//     /* If time went backwards, increment clock sequence */
//     if (system_time[0] < last_time[0] || (system_time[0] == last_time[0] && system_time[1] < last_time[1]))
//         last_clock_sequence++;
//     clock_sequence = last_clock_sequence;

//     last_time[0] = system_time[0];
//     last_time[1] = system_time[1];

//     UUIDMutex.unlock();

//     /* Bits 0-31 of time (32 bits) */
//     time_low = system_time[0];
//     /* Bits 32-47 of time (16 bits) */
//     time_mid = OFstatic_cast(Uint16, system_time[1] & 0xffff);
//     /* Bits 48-59 of time (11 bits) */
//     version_and_time_high = OFstatic_cast(Uint16, (system_time[1] >> 16) & 0xeff);
//     /* Version number, bits 15 to 12 of version_and_time_high */
//     version_and_time_high |= 0x100;
//     /* Sequence, lowest 8 bits of clock_sequence */
//     clock_seq_low = OFstatic_cast(Uint8, clock_sequence & 0xff);
//     /* Bits 8-13 of clock_sequence */
//     variant_and_clock_seq_high = OFstatic_cast(Uint8, (clock_sequence >> 8) & 0xcf);
//     /* Version */
//     variant_and_clock_seq_high |= 0x80;
//     /* And the node value */
//     memcpy(&node, &last_node, sizeof(node));
// }

// OFUUID::OFUUID()
// : time_low(0),
//   time_mid(0),
//   version_and_time_high(0),
//   variant_and_clock_seq_high(0),
//   clock_seq_low(0),
//   node(),
//   rnd()
// {
//     generate();
// }

// OFUUID::OFUUID(const struct BinaryRepresentation& rep)
// : time_low(0),
//   time_mid(0),
//   version_and_time_high(0),
//   variant_and_clock_seq_high(0),
//   clock_seq_low(0),
//   node()
// {
//     time_low = rep.value[0];
//     time_low = time_low << 8 | rep.value[1];
//     time_low = time_low << 8 | rep.value[2];
//     time_low = time_low << 8 | rep.value[3];
//     time_mid = rep.value[4];
//     time_mid = OFstatic_cast(Uint16, time_mid << 8 | rep.value[5]);
//     version_and_time_high = rep.value[6];
//     version_and_time_high = OFstatic_cast(Uint16, version_and_time_high << 8 | rep.value[7]);
//     variant_and_clock_seq_high = rep.value[8];
//     clock_seq_low = rep.value[9];
//     memcpy(&node[0], &rep.value[10], sizeof(node));
// }

// void OFUUID::getBinaryRepresentation(struct BinaryRepresentation& rep) const
// {
//     rep.value[0] = OFstatic_cast(Uint8, (time_low >> 24) & 0xff);
//     rep.value[1] = OFstatic_cast(Uint8, (time_low >> 16) & 0xff);
//     rep.value[2] = OFstatic_cast(Uint8, (time_low >>  8) & 0xff);
//     rep.value[3] = OFstatic_cast(Uint8, (time_low >>  0) & 0xff);
//     rep.value[4] = OFstatic_cast(Uint8, (time_mid >>  8) & 0xff);
//     rep.value[5] = OFstatic_cast(Uint8, (time_mid >>  0) & 0xff);
//     rep.value[6] = OFstatic_cast(Uint8, (version_and_time_high >> 8) & 0xff);
//     rep.value[7] = OFstatic_cast(Uint8, (version_and_time_high >> 0) & 0xff);
//     rep.value[8] = variant_and_clock_seq_high;
//     rep.value[9] = clock_seq_low;
//     memcpy(&rep.value[10], &node[0], sizeof(node));
// }

// OFString& OFUUID::toString(OFString& result, E_Representation representation) const
// {
//     OFOStringStream stream;
//     print(stream, representation);
//     OFSTRINGSTREAM_GETSTR(stream, result_ptr)
//     result = result_ptr;
//     OFSTRINGSTREAM_FREESTR(result_ptr);
//     return result;
// }

// STD_NAMESPACE ostream& OFUUID::print(STD_NAMESPACE ostream& stream, E_Representation representation) const
// {
//     switch (representation) {
//     case ER_RepresentationOID:
//         stream << "2.25.";
//         /* Fall through */
//     case ER_RepresentationInteger:
//         printInteger(stream);
//         break;
//     case ER_RepresentationURN:
//         stream << "urn:uuid:";
//         /* Fall through */
//     case ER_RepresentationHex:
//         printHex(stream);
//         break;
//     }

//     return stream;
// }

// void OFUUID::printHex(STD_NAMESPACE ostream& stream) const
// {
//     STD_NAMESPACE ios_base::fmtflags flags = stream.flags(STD_NAMESPACE ios_base::hex);
//     char fill_char = stream.fill('0');

//     stream << STD_NAMESPACE setw(8) << time_low << "-";
//     stream << STD_NAMESPACE setw(4) << time_mid << "-";
//     stream << STD_NAMESPACE setw(4) << version_and_time_high << "-";
//     stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, variant_and_clock_seq_high);
//     stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, clock_seq_low) << "-";
//     stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, node[0]);
//     stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, node[1]);
//     stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, node[2]);
//     stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, node[3]);
//     stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, node[4]);
//     stream << STD_NAMESPACE setw(2) << OFstatic_cast(int, node[5]);

//     stream.flags(flags);
//     stream.fill(fill_char);
// }

// static void divide_by_10(Uint32 n, Uint32& res, Uint32& rem)
// {
//     // This calculates res = m / d and rem = m % d where m = n + rem * 2^32

//     // First we do the higher half of the division
//     Uint32 next = (rem << 16) | (n >> 16);
//     Uint32 tmp = next / 10;
//     rem = next % 10;

//     // Then we do the same with the lower half
//     next = (rem << 16) | (n & 0xffff);
//     res = (next / 10) + (tmp << 16);
//     rem = next % 10;
// }

// void OFUUID::printInteger(STD_NAMESPACE ostream& stream) const
// {
//     // Oh sh.., converting a 128-bit integer to its base 10 representation. Not funny.

//     // A 128-bit int has at most 39 characters in base 10
//     char buffer[40];
//     // Our current 128-bit state
//     Uint32 data[4];
//     struct BinaryRepresentation representation;
//     /* Current buffer index (we are generating the last character of the output
//      * first, so we have write it into buffer in reverse order) */
//     int idx = OFstatic_cast(int, sizeof(buffer) - 1);

//     // First we convert the 128-bit integer into four 32-bit integers
//     getBinaryRepresentation(representation);
//     for (int i = 0; i < 4; i++) {
//         data[i]  = representation.value[0 + 4*i] << 24;
//         data[i] |= representation.value[1 + 4*i] << 16;
//         data[i] |= representation.value[2 + 4*i] <<  8;
//         data[i] |= representation.value[3 + 4*i] <<  0;
//     }

//     if (data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 0) {
//         // This should never happen, but we can still handle it! :-)
//         stream << "0";
//         return;
//     }

//     // As long as the result isn't 0, divide by 10 and print the remainder
//     while (data[0] != 0 || data[1] != 0 || data[2] != 0 || data[3] != 0) {
//         Uint32 rem = 0;
//         divide_by_10(data[0], data[0], rem);
//         divide_by_10(data[1], data[1], rem);
//         divide_by_10(data[2], data[2], rem);
//         divide_by_10(data[3], data[3], rem);

//         assert(rem <= 9);
//         buffer[--idx] = OFstatic_cast(char, rem + '0');
//     }
//     assert(idx >= 0);
//     buffer[sizeof(buffer) - 1] = '\0';
//     stream << &buffer[idx];
// }

// OFBool OFUUID::operator==(const OFUUID& o) const
// {
//     struct BinaryRepresentation own;
//     struct BinaryRepresentation other;
//     getBinaryRepresentation(own);
//     o.getBinaryRepresentation(other);

//     return memcmp(&own, &other, sizeof(struct BinaryRepresentation)) == 0;
// }
