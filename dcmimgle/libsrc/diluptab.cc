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
 *  Purpose: DicomLookupTable (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-12-16 16:11:54 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/diluptab.cc,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctypes.h"
#include "dcdeftag.h"
#include "dcsequen.h"
#include "dcitem.h"

#include "diluptab.h"
#include "didocu.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiLookupTable::DiLookupTable(const DiDocument *docu,
                             const DcmTagKey &descriptor,
                             const DcmTagKey &data,
                             const DcmTagKey &explanation,
                             EI_Status *status)
  : Count(0),
    FirstEntry(0),
    Bits(0),
    MinValue(0),
    MaxValue(0),
    Valid(0),
    Explanation(),
    Data(NULL),
    DataBuffer(NULL)
{
    if (docu != NULL)
        Init(docu, NULL, descriptor, data, explanation, status);
} 


DiLookupTable::DiLookupTable(const DiDocument *docu,
                             const DcmTagKey &sequence,
                             const DcmTagKey &descriptor,
                             const DcmTagKey &data,
                             const DcmTagKey &explanation,
                             const unsigned long pos,
                             unsigned long *card)
  : Count(0),
    FirstEntry(0),
    Bits(0),
    MinValue(0),
    MaxValue(0),
    Valid(0),
    Explanation(),
    Data(NULL),
    DataBuffer(NULL)
{
    if (docu != NULL)
    {
        DcmSequenceOfItems *seq = NULL;
        const unsigned long count = docu->getSequence(sequence, seq);
        if (card != NULL)
            *card = count;
        if ((seq != NULL) && (pos < count))
        {
            DcmItem *item = seq->getItem(pos);
            Init(docu, item, descriptor, data, explanation);
        }
    }
}


DiLookupTable::DiLookupTable(const DcmUnsignedShort &data,
                             const DcmUnsignedShort &descriptor,
                             const DcmLongString *explanation,
                             const signed int first,
                             EI_Status *status)
  : Count(0),
    FirstEntry(0),
    Bits(0),
    MinValue(0),
    MaxValue(0),
    Valid(0),
    Explanation(),
    Data(NULL),
    DataBuffer(NULL)
{
    Uint16 us = 0;
    int ok = (DiDocument::getElemValue((const DcmElement *)&descriptor, us, 0) > 0);
    Count = (ok && (us == 0)) ? MAX_TABLE_ENTRY_COUNT : us;                                 // see DICOM supplement 5
    ok &= (DiDocument::getElemValue((const DcmElement *)&descriptor, FirstEntry, 1) > 0);   // can be SS or US (will be type casted later) !?
    if (ok && (first >= 0) && (FirstEntry != (Uint16)first))
    {
        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
        {
            cerr << "WARNING: invalid value for 'First input value mapped' (" << FirstEntry << ") ";
            cerr << "... assuming " << first << " !" << endl;
        }
        FirstEntry = (Uint16)first;
    }
    ok &= (DiDocument::getElemValue((const DcmElement *)&descriptor, us, 2) > 0);           // bits per entry (only informational)
    unsigned long count = DiDocument::getElemValue((const DcmElement *)&data, Data);
    if (explanation != NULL)
        DiDocument::getElemValue((const DcmElement *)explanation, Explanation);             // explanation (free form text)
    checkTable(ok, count, us, status);
}


/*--------------*
 *  destructor  *
 *--------------*/

DiLookupTable::~DiLookupTable()
{
    delete[] DataBuffer;
}


/********************************************************************/


void DiLookupTable::Init(const DiDocument *docu,
                         DcmObject *obj,
                         const DcmTagKey &descriptor,
                         const DcmTagKey &data,
                         const DcmTagKey &explanation,
                         EI_Status *status)
{
    Uint16 us = 0;
    int ok = (docu->getValue(descriptor, us, 0, obj) > 0);
    Count = (ok && (us == 0)) ? MAX_TABLE_ENTRY_COUNT : us;                 // see DICOM supplement 5
    ok &= (docu->getValue(descriptor, FirstEntry, 1, obj) > 0);             // can be SS or US (will be type casted later) !?
    ok &= (docu->getValue(descriptor, us, 2, obj) > 0);                     // bits per entry (only informational)
    unsigned long count = docu->getValue(data, Data, obj);
    if (explanation != DcmTagKey(0,0))
        docu->getValue(explanation, Explanation);                           // explanation (free form text)
    checkTable(ok, count, us, status);
}
    
    
    
void DiLookupTable::checkTable(const int ok,
                               unsigned long count,
                               Uint16 bits,
                               EI_Status *status)
{
    if (ok && (count > 0))                                                    // valid LUT
    {
        register Uint16 i;
        if (count > MAX_TABLE_ENTRY_COUNT)                                    // cut LUT length to maximum
            count = MAX_TABLE_ENTRY_COUNT;
        if (count != Count)                                                   // length of LUT differs from number of LUT entries
        {
            if (count == ((Count + 1) >> 1))                                  // bits allocated 8, ignore padding
            {
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                    cerr << "INFO: lookup table uses 8 bits allocated ... converting to 16 bits." << endl;
                DataBuffer = new Uint16[Count];                               // create new LUT
                if (DataBuffer != NULL)
                {
                    register const Uint8 *p = (const Uint8 *)Data;
                    register Uint16 *q = DataBuffer;
                    if (gLocalByteOrder == EBO_BigEndian)                     // local machine has big endian byte ordering 
                    {
                        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                            cerr << "INFO: local machine has big endian byte ordering ... swapping 8 bit LUT entries." << endl;
                        for (i = 0; i < count; i++)                           // copy 8 bit entries to new 16 bit LUT (swap hi/lo byte)
                        {
                            *(q++) = *(p + 1);                                // copy low byte ...
                            *(q++) = *p;                                      // ... and then high byte
                            p += 2;                                           // jump to next hi/lo byte pair
                        }
                    } else {                                                  // local machine has little endian byte ordering (or unknown)
                        for (i = 0; i < Count; i++)
                            *(q++) = *(p++);                                  // copy 8 bit entries to new 16 bit LUT
                    }
                }
                Data = DataBuffer;
            } else {
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                {
                    cerr << "WARNING: invalid value for 'NumberOfTableEntries' (" << Count << ") ";
                    cerr << "... assuming " << count << " !" << endl;
                }
                Count = count;
            }
        }
        MinValue = (Uint16)maxval(MAX_TABLE_ENTRY_SIZE);                      // set minimum to maximum value
        register const Uint16 *p = Data;
        register Uint16 value;
        if (DataBuffer != NULL)                                               // LUT entries have been copied 8 -> 16 bits
        {
            for (i = 0; i < Count; i++)
            {
                value = *(p++);
                if (value < MinValue)                                         // get global minimum
                    MinValue = value;
                if (value > MaxValue)                                         // get global maximum
                    MaxValue = value;
            }
            checkBits(bits, 8);                                               // set 'Bits'
        } else {
            int cmp = 0;
            for (i = 0; i < Count; i++)
            {
                value = *(p++);
                if (((value >> 8) != 0) && (value & 0xff) != (value >> 8))    // lo-byte not equal to hi-byte and ...
                    cmp = 1;
                if (value < MinValue)                                         // get global minimum
                    MinValue = value;
                if (value > MaxValue)                                         // get global maximum
                    MaxValue = value;
            }
            if (cmp == 0)                                                     // lo-byte is always equal to hi-byte
                checkBits(bits, MIN_TABLE_ENTRY_SIZE, MAX_TABLE_ENTRY_SIZE);  // set 'Bits'
            else
                checkBits(bits, MAX_TABLE_ENTRY_SIZE, MIN_TABLE_ENTRY_SIZE);
        }
        Uint16 mask = (Uint16)maxval(Bits);                                   // mask lo-byte (8) or full word (16)
        if ((MinValue & mask != MinValue) || (MaxValue & mask != MaxValue))
        {                                                                     // mask table entries and copy them to new LUT
            MinValue &= mask;
            MaxValue &= mask;
            if (DataBuffer == NULL)
                DataBuffer = new Uint16[Count];                               // create new LUT
            if (DataBuffer != NULL)
            {
                p = Data;
                register Uint16 *q = DataBuffer;
                for (i = 0; i < Count; i++)
                    *(q++) = *(p++) & mask;
            }
            Data = DataBuffer;
        }
        Valid = (Data != NULL);                                               // lookup table is valid
    }
    else if (status != NULL)
    {
        *status = EIS_MissingAttribute;
        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Errors)
            cerr << "ERROR: one or more mandatory table attributes are missing !" << endl;
    }       
}



/********************************************************************/


void DiLookupTable::checkBits(const Uint16 bits,
                              const Uint16 right,
                              const Uint16 wrong)
{
    if ((bits < MIN_TABLE_ENTRY_SIZE) || (bits > MAX_TABLE_ENTRY_SIZE))
    {
        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
        {
            cerr << "WARNING: unsuitable value for 'BitsPerTableEntry' (" << bits << ") ... should be between ";
            cerr << MIN_TABLE_ENTRY_SIZE << " and " << MAX_TABLE_ENTRY_SIZE << " inclusive !" << endl;
        }
        if (bits < MIN_TABLE_ENTRY_SIZE)
            Bits = MIN_TABLE_ENTRY_SIZE;
        else
            Bits = MAX_TABLE_ENTRY_SIZE;
    }
    else if (bits == wrong)
    {
        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
        {
            cerr << "WARNING: unsuitable value for 'BitsPerTableEntry' (" << bits << ") ";
            cerr << "... assuming " << right << " !" << endl;
        }
        Bits = right;
    } else {
        
        /* do something heuristic in the future, e.g. create a 'Mask'? */
        
        Bits = bits;                                // assuming that descriptor value is correct !
    }
}


/*
**
** CVS/RCS Log:
** $Log: diluptab.cc,v $
** Revision 1.3  1998-12-16 16:11:54  joergr
** Added explanation string to LUT class (retrieved from dataset).
**
** Revision 1.2  1998/12/14 17:34:44  joergr
** Added support for signed values as second entry in look-up tables
** (= first value mapped).
**
** Revision 1.1  1998/11/27 16:04:33  joergr
** Added copyright message.
** Added constructors to use external modality transformations.
** Added methods to support presentation LUTs and shapes.
**
** Revision 1.3  1998/05/11 14:52:30  joergr
** Added CVS/RCS header to each file.
**
**
*/
