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
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-03 14:09:19 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/diluptab.cc,v $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dcdeftag.h"
#include "dcsequen.h"
#include "dcitem.h"
#include "ofbmanip.h"

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
  : DiBaseLUT(),
    OriginalBitsAllocated(16),
    OriginalData(NULL)
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
  : DiBaseLUT(),
    OriginalBitsAllocated(16),
    OriginalData(NULL)
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
                             const signed long first,
                             EI_Status *status)
  : DiBaseLUT(),
    OriginalBitsAllocated(16),
    OriginalData(NULL)
{
    Uint16 us = 0;
    if (DiDocument::getElemValue((const DcmElement *)&descriptor, us, 0) >= 3)           // number of LUT entries
    {
        Count = (us == 0) ? MAX_TABLE_ENTRY_COUNT : us;                                  // see DICOM supplement 5: "0" => 65536
        DiDocument::getElemValue((const DcmElement *)&descriptor, FirstEntry, 1);        // can be SS or US (will be type casted later)
        if ((first >= 0) && (FirstEntry != (Uint16)first))
        {
            if (DicomImageClass::DebugLevel & DicomImageClass::DL_Warnings)
            {
                CERR << "WARNING: invalid value for 'First input value mapped' (" << FirstEntry << ") "
                     << "... assuming " << first << " !" << endl;
            }
            FirstEntry = (Uint16)first;
        }
        DiDocument::getElemValue((const DcmElement *)&descriptor, us, 2);                // bits per entry (only informational)
        unsigned long count = DiDocument::getElemValue((const DcmElement *)&data, Data);
        OriginalData = (void *)Data;                                                     // store pointer to original data
        if (explanation != NULL)
            DiDocument::getElemValue((const DcmElement *)explanation, Explanation);      // explanation (free form text)
        checkTable(count, us, status);
     } else {
        if (status != NULL)
        {
            *status = EIS_MissingAttribute;
            if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
                CERR << "ERROR: incomplete or missing 'LookupTableDescriptor' !" << endl;
        } else {
            if (DicomImageClass::DebugLevel & DicomImageClass::DL_Warnings)
                CERR << "WARNING: incomplete or missing  'LookupTableDescriptor' ... ignoring LUT !" << endl;
        }
     }
}


DiLookupTable::DiLookupTable(Uint16 *buffer,
                             const Uint32 count,
                             const Uint16 bits)
  : DiBaseLUT(buffer),
    OriginalBitsAllocated(16),
    OriginalData(buffer)
{
    checkTable(count, bits);
}


/*--------------*
 *  destructor  *
 *--------------*/

DiLookupTable::~DiLookupTable()
{
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
    if (docu->getValue(descriptor, us, 0, obj) >= 3)                         // number of LUT entries
    {
        Count = (us == 0) ? MAX_TABLE_ENTRY_COUNT : us;                      // see DICOM supplement 5: "0" => 65536
        docu->getValue(descriptor, FirstEntry, 1, obj);                      // can be SS or US (will be type casted later)
        docu->getValue(descriptor, us, 2, obj);                              // bits per entry (only informational)
        unsigned long count = docu->getValue(data, Data, obj);
        OriginalData = (void *)Data;                                         // store pointer to original data
        if (explanation != DcmTagKey(0, 0))
            docu->getValue(explanation, Explanation);                        // explanation (free form text)
        checkTable(count, us, status);
    } else {
        if (status != NULL)
        {
            *status = EIS_MissingAttribute;
            if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
                CERR << "ERROR: incomplete or missing 'LookupTableDescriptor' !" << endl;
        } else {
            if (DicomImageClass::DebugLevel & DicomImageClass::DL_Warnings)
                CERR << "WARNING: incomplete or missing  'LookupTableDescriptor' ... ignoring LUT !" << endl;
        }
    }
}



void DiLookupTable::checkTable(unsigned long count,
                               Uint16 bits,
                               EI_Status *status)
{
    if (count > 0)                                                            // valid LUT
    {
        register unsigned long i;
        if (count > MAX_TABLE_ENTRY_COUNT)                                    // cut LUT length to maximum
            count = MAX_TABLE_ENTRY_COUNT;
        if (count != Count)                                                   // length of LUT differs from number of LUT entries
        {
            if (count == ((Count + 1) >> 1))                                  // bits allocated 8, ignore padding
            {
                OriginalBitsAllocated = 8;
                if (DicomImageClass::DebugLevel & DicomImageClass::DL_Informationals)
                    CERR << "INFO: lookup table uses 8 bits allocated ... converting to 16 bits." << endl;
                DataBuffer = new Uint16[Count];                               // create new LUT
                if (DataBuffer != NULL)
                {
                    register const Uint8 *p = (const Uint8 *)Data;
                    register Uint16 *q = DataBuffer;
                    if (gLocalByteOrder == EBO_BigEndian)                     // local machine has big endian byte ordering
                    {
                        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Informationals)
                            CERR << "INFO: local machine has big endian byte ordering ... swapping 8 bit LUT entries." << endl;
                        for (i = count; i != 0; i--)                          // copy 8 bit entries to new 16 bit LUT (swap hi/lo byte)
                        {
                            *(q++) = *(p + 1);                                // copy low byte ...
                            *(q++) = *p;                                      // ... and then high byte
                            p += 2;                                           // jump to next hi/lo byte pair
                        }
                    } else {                                                  // local machine has little endian byte ordering (or unknown)
                        for (i = Count; i != 0; i--)
                            *(q++) = *(p++);                                  // copy 8 bit entries to new 16 bit LUT
                    }
                }
                Data = DataBuffer;
            } else {
                if (DicomImageClass::DebugLevel & DicomImageClass::DL_Warnings)
                {
                    CERR << "WARNING: invalid value for 'NumberOfTableEntries' (" << Count << ") "
                         << "... assuming " << count << " !" << endl;
                }
                Count = count;
            }
        }
        MinValue = (Uint16)DicomImageClass::maxval(MAX_TABLE_ENTRY_SIZE);     // set minimum to maximum value
        register const Uint16 *p = Data;
        register Uint16 value;
        if (DataBuffer != NULL)                                               // LUT entries have been copied 8 -> 16 bits
        {
            for (i = Count; i != 0; i--)
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
            for (i = Count; i != 0; i--)
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
        Uint16 mask = (Uint16)DicomImageClass::maxval(Bits);                  // mask lo-byte (8) or full word (16)
        if (((MinValue & mask) != MinValue) || ((MaxValue & mask) != MaxValue))
        {                                                                     // mask table entries and copy them to new LUT
            MinValue &= mask;
            MaxValue &= mask;
            if (DataBuffer == NULL)
                DataBuffer = new Uint16[Count];                               // create new LUT
            if (DataBuffer != NULL)
            {
                p = Data;
                register Uint16 *q = DataBuffer;
                for (i = Count; i != 0; i--)
                    *(q++) = *(p++) & mask;
            }
            Data = DataBuffer;
        }
        Valid = (Data != NULL);                                               // lookup table is valid
    } else {
        if (status != NULL)
        {
            *status = EIS_InvalidValue;
            if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
                CERR << "ERROR: empty 'LookupTableData' attribute !" << endl;
        } else {
            if (DicomImageClass::DebugLevel & DicomImageClass::DL_Warnings)
                CERR << "WARNING: empty 'LookupTableData' attribute ... ignoring LUT !" << endl;
        }
    }
}



/********************************************************************/


void DiLookupTable::checkBits(const Uint16 bits,
                              const Uint16 right,
                              const Uint16 wrong)
{
    if ((bits < MIN_TABLE_ENTRY_SIZE) || (bits > MAX_TABLE_ENTRY_SIZE))
    {
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Warnings)
        {
            CERR << "WARNING: unsuitable value for 'BitsPerTableEntry' (" << bits << ") ... should be between "
                 << MIN_TABLE_ENTRY_SIZE << " and " << MAX_TABLE_ENTRY_SIZE << " inclusive !" << endl;
        }
        if (bits < MIN_TABLE_ENTRY_SIZE)
            Bits = MIN_TABLE_ENTRY_SIZE;
        else
            Bits = MAX_TABLE_ENTRY_SIZE;
    }
    else if (bits == wrong)
    {
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Warnings)
        {
            CERR << "WARNING: unsuitable value for 'BitsPerTableEntry' (" << bits << ") "
                 << "... assuming " << right << " !" << endl;
        }
        Bits = right;
    } else {

        /* do something heuristically in the future, e.g. create a 'Mask'? */

        Bits = bits;                                // assuming that descriptor value is correct !
    }
}


int DiLookupTable::invertTable(const int flag)
{
    int result = 0;
    if ((Data != NULL) && (Count > 0) && (flag & 0x3))
    {
        register Uint32 i;
        if (flag & 0x2)
        {
            if (OriginalData != NULL)
            {
                if (OriginalBitsAllocated == 8)
                {
                    if (Bits <= 8)
                    {
                        register const Uint8 *p = (const Uint8 *)OriginalData;
                        register Uint8 *q = (Uint8 *)OriginalData;
                        const Uint8 max = (Uint8)DicomImageClass::maxval(Bits);
                        for (i = Count; i != 0; i--)
                            *(q++) = max - *(p++);
                        result |= 0x2;
                    }
                } else {
                    register const Uint16 *p = (const Uint16 *)OriginalData;
                    register Uint16 *q = (Uint16 *)OriginalData;
                    const Uint16 max = (Uint16)DicomImageClass::maxval(Bits);
                    for (i = Count; i != 0; i--)
                        *(q++) = max - *(p++);
                    result |= 0x2;
                }
            }
        }
        if (flag & 0x1)
        {
            if (DataBuffer != NULL)
            {
                register const Uint16 *p = (const Uint16 *)DataBuffer;
                register Uint16 *q = DataBuffer;
                const Uint16 max = (Uint16)DicomImageClass::maxval(Bits);
                for (i = Count; i != 0; i--)
                    *(q++) = max - *(p++);
                result |= 0x1;
            }
            else if (!(flag & 0x2))
            {
                DataBuffer = new Uint16[Count];
                if (DataBuffer != NULL)
                {
                    register const Uint16 *p = Data;
                    register Uint16 *q = DataBuffer;
                    const Uint16 max = (Uint16)DicomImageClass::maxval(Bits);
                    for (i = Count; i != 0; i--)
                        *(q++) = max - *(p++);
                    Data = DataBuffer;
                    result |= 0x1;
                }
            }
        }
    }
    return result;
}


int DiLookupTable::mirrorTable(const int flag)
{
    int result = 0;
    if ((Data != NULL) && (Count > 0) && (flag & 0x3))
    {
        register Uint32 i;
        if (flag & 0x2)
        {
            if (OriginalData != NULL)
            {
                if (OriginalBitsAllocated == 8)
                {
                    if (Bits <= 8)
                    {
                        register Uint8 *p = (Uint8 *)OriginalData + (Count - 1);
                        register Uint8 *q = (Uint8 *)OriginalData;
                        register Uint8 val;
                        const unsigned long mid = Count / 2;
                        for (i = mid; i != 0; i--)
                        {
                            val = *q;
                            *(q++) = *p;
                            *(p--) = val;
                        }
                        result |= 0x2;
                    }
                } else {
                    register Uint16 *p = (Uint16 *)OriginalData + (Count - 1);
                    register Uint16 *q = (Uint16 *)OriginalData;
                    register Uint16 val;
                    const unsigned long mid = Count / 2;
                    for (i = mid; i != 0; i--)
                    {
                        val = *q;
                        *(q++) = *p;
                        *(p--) = val;
                    }
                    result |= 0x2;
                }
            }
        }
        if (flag & 0x1)
        {
            if (DataBuffer != NULL)
            {
                register Uint16 *p = DataBuffer + (Count - 1);
                register Uint16 *q = DataBuffer;
                register Uint16 val;
                const unsigned long mid = Count / 2;
                for (i = mid; i != 0; i--)
                {
                    val = *q;
                    *(q++) = *p;
                    *(p--) = val;
                }
                result |= 0x1;
            }
            else if (!(flag & 0x2))
            {
                DataBuffer = new Uint16[Count];
                if (DataBuffer != NULL)
                {
                    register Uint16 *p = (Uint16 *)Data + (Count - 1);
                    register Uint16 *q = DataBuffer;
                    register Uint16 val;
                    const unsigned long mid = Count / 2;
                    for (i = mid; i != 0; i--)
                    {
                        val = *q;
                        *(q++) = *p;
                        *(p--) = val;
                    }
                    Data = DataBuffer;
                    result |= 0x1;
                }
            }
        }
    }
    return result;
}


DiLookupTable *DiLookupTable::createInverseLUT() const
{
    DiLookupTable *lut = NULL;
    if (Valid)
    {
        const Uint32 count = DicomImageClass::maxval(Bits, 0);
        const Uint16 bits = DicomImageClass::tobits(Count + FirstEntry);
        Uint16 *data = new Uint16[count];
        Uint8 *valid = new Uint8[count];
        if ((data != NULL) && (valid != NULL))
        {
            OFBitmanipTemplate<Uint8>::zeroMem(valid, count);   // initialize array
            register Uint32 i;
            for (i = 0; i < Count; i++)                         // 'copy' values to new array
            {
                if (!valid[Data[i]])
                    data[Data[i]] = (Uint16)(i + FirstEntry);
                valid[Data[i]] = 1;
            }
            Uint32 last = 0;
            i = 0;
            while (i < count)                                   // fill gaps with valid values
            {
                if (valid[i])                                   // skip valid values
                    last = i;
                else
                {
                    register Uint32 j = i + 1;
                    while ((j < count) && !valid[j])            // find next valid value
                        j++;
                    if (valid[last])                            // check for starting conditions
                    {
                        const Uint32 mid = (j < count) ? (i + j) / 2 : count;
                        while (i < mid)
                        {                                   // fill first half with 'left' value
                            data[i] = data[last];
                            i++;
                        }
                    }
                    if ((j < count) && valid[j])
                    {
                        while (i < j)                           // fill second half with 'right' value
                        {
                            data[i] = data[j];
                            i++;
                        }
                        last = j;
                    }
                }
                i++;
            }
            lut = new DiLookupTable(data, count, bits);         // create new LUT
        }
        delete[] valid;
    }
    return lut;
}


int DiLookupTable::compareLUT(const DcmUnsignedShort &data,
                              const DcmUnsignedShort &descriptor)
{
    int result = 1;
    DiBaseLUT *lut = new DiLookupTable(data, descriptor);
    if (lut != NULL)
        result = compare(lut);
    delete lut;
    return result;
}


OFBool DiLookupTable::operator==(const DiLookupTable &lut)
{
    return (compare((const DiBaseLUT *)&lut) == 0);
}


/*
 *
 * CVS/RCS Log:
 * $Log: diluptab.cc,v $
 * Revision 1.17  2000-03-03 14:09:19  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.16  1999/11/24 11:14:44  joergr
 * Added method to mirror order of entries in look-up tables.
 *
 * Revision 1.15  1999/10/20 18:40:26  joergr
 * Removed const from pointer declaration (problem reported by MSVC).
 *
 * Revision 1.14  1999/10/20 10:36:37  joergr
 * Enhanced method invertTable to distinguish between copy of LUT data and
 * original (referenced) LUT data.
 *
 * Revision 1.13  1999/09/30 11:37:55  joergr
 * Added methods to compare two lookup tables.
 *
 * Revision 1.12  1999/09/17 17:27:43  joergr
 * Modified error/warning messages for corrupt lookup table attributes.
 * Changed integer type for loop variable to avoid compiler warnings reported
 * by MSVC.
 *
 * Revision 1.11  1999/09/17 13:16:56  joergr
 * Removed bug: check pointer variable before dereferencing it.
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.10  1999/09/08 16:58:36  joergr
 * Changed some integer types to avoid compiler warnings repoted by MSVC.
 *
 * Revision 1.9  1999/09/08 15:20:32  joergr
 * Completed implementation of setting inverse presentation LUT as needed
 * e.g. for DICOM print (invert 8->12 bits PLUT).
 *
 * Revision 1.8  1999/07/23 13:42:20  joergr
 * Corrected bug occurred when reading corrupted LUT descriptors.
 * Added dummy method (no implementation yet) to create inverse LUTs.
 *
 * Revision 1.7  1999/05/03 11:05:29  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.6  1999/04/28 15:01:42  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.5  1999/02/03 17:40:08  joergr
 * Added base class for look-up tables (moved main functionality of class
 * DiLookupTable to DiBaseLUT).
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 *
 * Revision 1.4  1998/12/22 13:29:16  joergr
 * Changed parameter type.
 *
 * Revision 1.3  1998/12/16 16:11:54  joergr
 * Added explanation string to LUT class (retrieved from dataset).
 *
 * Revision 1.2  1998/12/14 17:34:44  joergr
 * Added support for signed values as second entry in look-up tables
 * (= first value mapped).
 *
 * Revision 1.1  1998/11/27 16:04:33  joergr
 * Added copyright message.
 * Added constructors to use external modality transformations.
 * Added methods to support presentation LUTs and shapes.
 *
 * Revision 1.3  1998/05/11 14:52:30  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
