/*
 *
 *  Copyright (C) 1999, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomBaseLUT (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-09-17 12:07:23 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dibaslut.h,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 * 
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIBASLUT_H
#define __DIBASLUT_H

#include "osconfig.h"
#include "ofstring.h"

#include "diutils.h"


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
     ** @param  dummy (used to distinguish between signed and unsigned methods)
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
     ** @param  dummy (used to distinguish between signed and unsigned methods)
     *
     ** @return index of first LUT entry
     */
    inline Sint32 getFirstEntry(const Sint32) const
    {
        return (Sint16)FirstEntry;
    }

    /** get index of last LUT entry.
     *  FIV in LUT descriptor is US -> 16 bit unsigned.
     *  ... or the previous pixel transformation requires an unsigned LUT input value.
     *
     ** @param  dummy (used to distinguish between signed and unsigned methods)
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
     ** @param  dummy (used to distinguish between signed and unsigned methods)
     *
     ** @return index of first LUT entry
     */
    inline Sint32 getLastEntry(const Sint32) const
    {
        return (Sint32)((Sint16)FirstEntry) + Count - 1;
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
        return Data[pos - (Sint32)((Sint16)FirstEntry)];
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
        return (Explanation.empty()) ? (const char *)NULL : Explanation.c_str();
    }
    
    /** invert all LUT values.
     *  (e.g. used for presentation LUTs)
     *
     ** @return status, true if successful, false otherwise
     */
    int invertTable();
    

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
 * Revision 1.8  1999-09-17 12:07:23  joergr
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
