/*
 *
 *  Copyright (C) 1996-2015, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomBaseLUT (Header)
 *
 */


#ifndef DIBASLUT_H
#define DIBASLUT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/diutils.h"


/*---------------------*
 *  macro definitions  *
 *---------------------*/

#define MIN_TABLE_ENTRY_SIZE 8
#define MAX_TABLE_ENTRY_SIZE 16
#define MAX_TABLE_ENTRY_COUNT 65536


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Base class to handle look-up tables
 */
class DCMTK_DCMIMGLE_EXPORT DiBaseLUT
{

 public:

    /** constructor
     *
     ** @param  count  number of LUT entries
     *  @param  bits   number of bits per entry
     */
    DiBaseLUT(const Uint32 count = 0,
              const Uint16 bits = 0);

    /** destructor
     */
    virtual ~DiBaseLUT();

    /** get number of LUT entries
     *
     ** @return number of LUT entries
     */
    inline Uint32 getCount() const
    {
        return Count;
    }

    /** get number of bits per entry
     *
     ** @return number of bits per entry
     */
    inline Uint16 getBits() const
    {
        return Bits;
    }

    /** get index of first LUT entry.
     *  First input value mapped (FIV) in LUT descriptor is US -> 16 bit unsigned.
     *  ... or the previous pixel transformation requires an unsigned LUT input value.
     *
     ** (#)param  dummy (used to distinguish between signed and unsigned methods)
     *
     ** @return index of first LUT entry
     */
    inline Uint32 getFirstEntry(const Uint32 = 0) const
    {
        return FirstEntry;
    }

    /** get index of first LUT entry.
     *  First input value mapped (FIV) in LUT descriptor is SS -> 16 bit signed.
     *  ... or the previous pixel transformation requires a signed LUT input value.
     *
     ** (#)param  dummy (used to distinguish between signed and unsigned methods)
     *
     ** @return index of first LUT entry
     */
    inline Sint32 getFirstEntry(const Sint32) const
    {
        return OFstatic_cast(Sint16, FirstEntry);
    }

    /** get index of last LUT entry.
     *  FIV in LUT descriptor is US -> 16 bit unsigned.
     *  ... or the previous pixel transformation requires an unsigned LUT input value.
     *
     ** (#)param  dummy (used to distinguish between signed and unsigned methods)
     *
     ** @return index of last LUT entry
     */
    inline Uint32 getLastEntry(const Uint32 = 0) const
    {
        return FirstEntry + Count - 1;
    }

    /** get index of last LUT entry.
     *  FIV in LUT descriptor is SS -> 16 bit signed.
     *  ... or the previous pixel transformation requires a signed LUT input value.
     *
     ** (#)param  dummy (used to distinguish between signed and unsigned methods)
     *
     ** @return index of first LUT entry
     */
    inline Sint32 getLastEntry(const Sint32) const
    {
        return OFstatic_cast(Sint32, OFstatic_cast(Sint16, FirstEntry)) + Count - 1;
    }

    /** get value of specified LUT entry
     *
     ** @param  pos  position in the LUT to be returned
     *
     ** @return value of specified LUT entry
     */
    inline Uint16 getValue(const Uint16 pos) const
    {
        return Data[pos];
    }

    /** get value of specified LUT entry.
     *  FIV in LUT descriptor is US -> 16 bit unsigned.
     *  ... or the previous pixel transformation requires an unsigned LUT input value.
     *
     ** @param  pos  position in the LUT to be returned
     *
     ** @return value of specified LUT entry
     */
    inline Uint16 getValue(const Uint32 pos) const
    {
        return Data[pos - FirstEntry];
    }

    /** get value of specified LUT entry.
     *  FIV in LUT descriptor is SS -> 16 bit signed.
     *  ... or the previous pixel transformation requires a signed LUT input value.
     *
     ** @param  pos  position in the LUT to be returned
     *
     ** @return value of specified LUT entry
     */
    inline Uint16 getValue(const Sint32 pos) const
    {
        return Data[pos - OFstatic_cast(Sint32, OFstatic_cast(Sint16, FirstEntry))];
    }

    /** get value of first LUT entry.
     *
     ** @return value of first LUT entry
     */
    inline Uint16 getFirstValue() const
    {
        return Data[0];
    }

    /** get value of last LUT entry.
     *
     ** @return value of last LUT entry
     */
    inline Uint16 getLastValue() const
    {
        return Data[Count - 1];
    }

    /** get pointer to LUT data
     *
     ** @return pointer to LUT data
     */
    inline const Uint16 *getData() const
    {
        return Data;
    }

    /** get minimum value of the LUT.
     *
     ** @return minimum value of the LUT
     */
    inline Uint16 getMinValue() const
    {
        return MinValue;
    }

    /** get maximum value of the LUT.
     *
     ** @return maximum value of the LUT
     */
    inline Uint16 getMaxValue() const
    {
        return MaxValue;
    }

    /** get absolute value range of the LUT entries.
     *  The maximum value which could be stored with the specified bit depth is calculated.
     *
     ** @return absolute range of LUT entries
     */
    inline Uint32 getAbsMaxRange() const
    {
        return OFstatic_cast(Uint32, DicomImageClass::maxval(Bits, 0));
    }

    /** check whether LUT is valid
     *
     ** @return status, true if valid, false otherwise
     */
    inline int isValid() const
    {
        return Valid;
    }

    /** get LUT explanation string
     *
     ** @return LUT explanation string if successful, NULL otherwise
     */
    inline const char *getExplanation() const
    {
        return (Explanation.empty()) ? OFstatic_cast(const char *, NULL) : Explanation.c_str();
    }

    /** compares current LUT with specified LUT
     *
     ** @param  lut  LUT to be compared with the current one
     *
     ** @return OFTrue if LUTs are equal, OFFalse otherwise
     */
    virtual OFBool operator==(const DiBaseLUT &lut);


 protected:

    /** constructor
     *
     ** @param  buffer  pointer to array with LUT entries
     *  @param  count   number of LUT entries
     *  @param  bits    number of bits per entry
     */
    DiBaseLUT(Uint16 *buffer,
              const Uint32 count = 0,
              const Uint16 bits = 0);

    /** compares current LUT with specified LUT
     *
     ** @param  lut  LUT to be compared with the current one
     *
     ** @return true if LUTs are not equal (1 = invalid LUT,
     *                                      2 = descriptor differs,
     *                                      3 = data differs)
     *          false (0) otherwise
     */
    int compare(const DiBaseLUT *lut);

    /// number of LUT entries
    Uint32 Count;
    /// first input value mapped (FIV)
    Uint16 FirstEntry;
    /// number of bits per entry
    Uint16 Bits;

    /// minimum LUT value
    Uint16 MinValue;
    /// maximum LUT value
    Uint16 MaxValue;

    /// status code, indicating whether LUT is valid
    int Valid;

    /// LUT explanation string
    OFString Explanation;

    /// pointer to lookup table data
    const Uint16 *Data;
    /// pointer to data buffer (will be deleted in the destructor)
    Uint16 *DataBuffer;


 private:

 // --- declarations to avoid compiler warnings

    DiBaseLUT(const DiBaseLUT &);
    DiBaseLUT &operator=(const DiBaseLUT &);
};


#endif
