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
 *  Update Date:      $Date: 1999-07-23 13:51:44 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dibaslut.h,v $
 *  CVS/RCS Revision: $Revision: 1.6 $
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

    DiBaseLUT(const Uint32 count = 0,
              const Uint16 bits = 0);

    virtual ~DiBaseLUT();

    inline Uint32 getCount() const
        { return Count; }
    
    inline Uint16 getBits() const
        { return Bits; }

    inline Uint32 getFirstEntry(const Uint32 = 0) const
        { return FirstEntry; }

    inline Sint32 getFirstEntry(const Sint32) const
        { return (Sint16)FirstEntry; }

    inline Uint32 getLastEntry(const Uint32 = 0) const
        { return FirstEntry + Count - 1; }

    inline Sint32 getLastEntry(const Sint32) const
        { return (Sint32)((Sint16)FirstEntry) + Count - 1; }

    inline Uint16 getValue(const Uint32 pos) const
        { return Data[pos - FirstEntry]; }

    inline Uint16 getValue(const Sint32 pos) const
        { return Data[pos - (Sint32)((Sint16)FirstEntry)]; }

    inline Uint16 getFirstValue() const
        { return Data[0]; }

    inline Uint16 getLastValue() const
        { return Data[Count - 1]; }

    inline Uint16 getMinValue() const
        { return MinValue; }

    inline Uint16 getMaxValue() const
        { return MaxValue; }

    inline Uint32 getAbsMaxRange() const
        { return DicomImageClass::maxval(Bits, 0); }

    inline int isValid() const
        { return Valid; }

    inline const char *getExplanation() const
    {
        return (Explanation.empty()) ? (const char *)NULL : Explanation.c_str();
    }
    
    int invertTable();
    

 protected:

    Uint32 Count;
    Uint16 FirstEntry;
    Uint16 Bits;
    
    Uint16 MinValue;
    Uint16 MaxValue;

    int Valid;

    OFString Explanation;

    const Uint16 *Data;             // points to lookup table data
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
 * Revision 1.6  1999-07-23 13:51:44  joergr
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
