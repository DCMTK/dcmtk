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
 *  Update Date:      $Date: 1998-11-27 15:10:21 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diluptab.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 * 
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DILUPTAB_H
#define __DILUPTAB_H

#include "osconfig.h"
#include "dctypes.h"

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
class DcmTagKey;
class DcmUnsignedShort;
class DiDocument;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiLookupTable : public DiObjectCounter
{

 public:

    DiLookupTable(const DiDocument *,
                  const DcmTagKey &,
                  const DcmTagKey &,
                  EI_Status * = NULL);

    DiLookupTable(const DiDocument *,
                  const DcmTagKey &,
                  const DcmTagKey &,
                  const DcmTagKey &,
                  const unsigned long = 0,
                  unsigned long * = NULL);

    DiLookupTable(const DcmUnsignedShort &data,
                  const DcmUnsignedShort &descriptor,
                  EI_Status * = NULL);

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

    void Init(const DiDocument *,
              DcmObject *,
              const DcmTagKey &,
              const DcmTagKey &,
              EI_Status * = NULL);

    void checkTable(const int ok,
                    unsigned long count,
                    Uint16 bits,
                    EI_Status *status);

    void checkBits(const Uint16,
                   const Uint16,
                   const Uint16);


 private:

    Uint32 Count;
    Uint16 FirstEntry;
    Uint16 Bits;
    Uint16 Mask;
    
    Uint16 MinValue;
    Uint16 MaxValue;

    int Valid;

    const Uint16 *Data;             // points to lookup table data

 // --- declarations to avoid compiler warnings
 
    DiLookupTable(const DiLookupTable &);
    DiLookupTable &operator=(const DiLookupTable &);
};


#endif


/*
**
** CVS/RCS Log:
** $Log: diluptab.h,v $
** Revision 1.1  1998-11-27 15:10:21  joergr
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
