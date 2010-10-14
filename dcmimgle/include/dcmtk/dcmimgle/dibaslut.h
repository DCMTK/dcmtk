/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:25 $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class DiBaseLUT
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
        return DicomImageClass::maxval(Bits, 0);
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


/*
 *
 * CVS/RCS Log:
 * $Log: dibaslut.h,v $
 * Revision 1.18  2010-10-14 13:16:25  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.17  2010-10-05 15:26:28  joergr
 * Fixed various Doxygen API documentation issues.
 *
 * Revision 1.16  2005/12/08 16:47:32  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.15  2003/12/08 18:40:32  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated CVS header.
 *
 * Revision 1.14  2003/06/12 15:08:34  joergr
 * Fixed inconsistent API documentation reported by Doxygen.
 *
 * Revision 1.13  2001/06/01 15:49:39  meichel
 * Updated copyright header
 *
 * Revision 1.12  2000/03/08 16:24:13  meichel
 * Updated copyright header.
 *
 * Revision 1.11  2000/03/06 18:17:59  joergr
 * Moved get-method to base class and made method virtual to avoid hiding of
 * methods (reported by Sun CC 4.2).
 *
 * Revision 1.10  1999/10/20 10:34:43  joergr
 * Enhanced method invertTable to distinguish between copy of LUT data and
 * original (referenced) LUT data.
 *
 * Revision 1.9  1999/09/30 11:37:08  joergr
 * Added methods to compare two lookup tables.
 *
 * Revision 1.8  1999/09/17 12:07:23  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.7  1999/09/08 15:19:24  joergr
 * Completed implementation of setting inverse presentation LUT as needed
 * e.g. for DICOM print (invert 8->12 bits PLUT).
 *
 * Revision 1.6  1999/07/23 13:51:44  joergr
 * Changed comments/formatting.
 *
 * Revision 1.5  1999/05/03 11:09:27  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.4  1999/03/24 17:19:58  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.3  1999/03/03 11:46:26  joergr
 * Added method to invert lookup table data (used for presentation state LUTs).
 *
 * Revision 1.2  1999/02/11 15:42:49  joergr
 * Removed unnecessary include statement.
 *
 * Revision 1.1  1999/02/03 17:45:38  joergr
 * Added base class for look-up tables (moved main functionality of class
 * DiLookupTable to DiBaseLUT).
 *
 *
 */
