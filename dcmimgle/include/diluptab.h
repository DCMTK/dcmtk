/*
 *
 *  Copyright (C) 1996-99, OFFIS
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
 *  Purpose: DicomLookupTable (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-12-16 16:31:30 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diluptab.h,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 * 
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DILUPTAB_H
#define __DILUPTAB_H

#include "osconfig.h"
#include "dctypes.h"
#include "dctagkey.h"
#include "ofstring.h"

#include "diobjcou.h"
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
class DcmUnsignedShort;
class DcmLongString;
class DiDocument;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiLookupTable : public DiObjectCounter
{

 public:

    DiLookupTable(const DiDocument *docu,
                  const DcmTagKey &descriptor,
                  const DcmTagKey &data,
                  const DcmTagKey &explanation = DcmTagKey(0,0),
                  EI_Status *status = NULL);

    DiLookupTable(const DiDocument *docu,
                  const DcmTagKey &sequence,
                  const DcmTagKey &decriptor,
                  const DcmTagKey &data,
                  const DcmTagKey &explanation = DcmTagKey(0,0),
                  const unsigned long pos = 0,
                  unsigned long *card = NULL);

    DiLookupTable(const DcmUnsignedShort &data,
                  const DcmUnsignedShort &descriptor,
                  const DcmLongString *explanation = NULL,
                  const signed int first = -1,
                  EI_Status *status = NULL);

    virtual ~DiLookupTable();

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

    inline int isValid() const
        { return Valid; }

    inline const char *getExplanation() const
    {
        return Explanation.c_str();
    }


 protected:

    void Init(const DiDocument *docu,
              DcmObject *obj,
              const DcmTagKey &descriptor,
              const DcmTagKey &data,
              const DcmTagKey &explanation,
              EI_Status *status = NULL);

    void checkTable(const int ok,
                    unsigned long count,
                    Uint16 bits,
                    EI_Status *status);

    void checkBits(const Uint16 bits,
                   const Uint16 right,
                   const Uint16 wrong = 0);


 private:

    Uint32 Count;
    Uint16 FirstEntry;
    Uint16 Bits;
    
    Uint16 MinValue;
    Uint16 MaxValue;

    int Valid;

    OFString Explanation;

    const Uint16 *Data;             // points to lookup table data
    Uint16 *DataBuffer;

 // --- declarations to avoid compiler warnings
 
    DiLookupTable(const DiLookupTable &);
    DiLookupTable &operator=(const DiLookupTable &);
};


#endif


/*
**
** CVS/RCS Log:
** $Log: diluptab.h,v $
** Revision 1.3  1998-12-16 16:31:30  joergr
** Added explanation string to LUT class (retrieved from dataset).
**
** Revision 1.2  1998/12/14 17:19:19  joergr
** Added support for signed values as second entry in look-up tables
** (= first value mapped).
**
** Revision 1.1  1998/11/27 15:10:21  joergr
** Added copyright message.
** Added support of object counter class.
** Added constructors to use external modality transformations.
** Added methods to support presentation LUTs and shapes.
**
** Revision 1.3  1998/07/01 08:39:22  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.2  1998/05/11 14:53:18  joergr
** Added CVS/RCS header to each file.
**
**
*/
