/*
**
** Author:  Joerg Riesmeier
** Created: 22.12.96
**
** Module:  diluptab.h
**
** Purpose: DicomLookupTable (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:18 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diluptab.h,v $
** CVS/RCS Revision: $Revision: 1.2 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DILUPTAB_H
#define __DILUPTAB_H

#include "osconfig.h"
#include "dctypes.h"

#include "diutils.h"

#include <stddef.h>


/*---------------------*
 *  macro definitions  *
 *---------------------*/

#define MIN_TABLE_ENTRY_SIZE 8
#define MAX_TABLE_ENTRY_SIZE 16
#define MAX_TABLE_ENTRY_COUNT 65536


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmObject;
class DcmTagKey;
class DiDocument;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiLookupTable
{
 public:
    DiLookupTable(const DiDocument *, const DcmTagKey &, const DcmTagKey &, EI_Status * = NULL);
    DiLookupTable(const DiDocument *, const DcmTagKey &, const DcmTagKey &, const DcmTagKey &, const unsigned long = 0,
        unsigned long * = NULL);
    virtual ~DiLookupTable();

    inline Uint32 getCount() const
        { return Count; }
    
    inline Uint16 getBits() const
        { return Bits; }
    inline Sint32 getFirstEntry() const
        { return FirstEntry; }
    inline Sint32 getLastEntry() const
        { return FirstEntry + Count - 1; }

    inline Uint16 getValue(Sint32 pos) const
        { return Data[pos - FirstEntry] & Mask; }
    inline Uint16 getFirstValue() const
        { return Data[0] & Mask; }
    inline Uint16 getLastValue() const
        { return Data[Count - 1] & Mask; }
    inline Uint16 getMinValue() const
        { return MinValue; }
    inline Uint16 getMaxValue() const
        { return MaxValue; }

    inline int isValid() const
        { return Valid; }

 protected:
    void Init(const DiDocument *, DcmObject *, const DcmTagKey &, const DcmTagKey &, EI_Status * = NULL);
    void checkBits(const Uint16, const Uint16, const Uint16);

 private:
    Uint32 Count;
    Uint16 FirstEntry;
    Uint16 Bits;
    Uint16 Mask;
    
    Uint16 MinValue;
    Uint16 MaxValue;

    int Valid;

    const Uint16 *Data;             // points to lookup table data
};


#endif


/*
**
** CVS/RCS Log:
** $Log: diluptab.h,v $
** Revision 1.2  1998-05-11 14:53:18  joergr
** Added CVS/RCS header to each file.
**
**
*/
