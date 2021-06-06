/*
 *
 *  Copyright (C) 1996-2021, OFFIS e.V.
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
 *  Purpose: DicomLookupTable (Source)
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcvrus.h"

#include "dcmtk/ofstd/ofbmanip.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/diluptab.h"
#include "dcmtk/dcmimgle/didocu.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiLookupTable::DiLookupTable(const DiDocument *docu,
                             const DcmTagKey &descriptor,
                             const DcmTagKey &data,
                             const DcmTagKey &explanation,
                             const EL_BitsPerTableEntry descripMode,
                             EI_Status *status)
  : DiBaseLUT(),
    OriginalBitsAllocated(16),
    OriginalData(NULL)
{
    if (docu != NULL)
        Init(docu, NULL, descriptor, data, explanation, descripMode, status);
}


DiLookupTable::DiLookupTable(const DiDocument *docu,
                             const DcmTagKey &sequence,
                             const DcmTagKey &descriptor,
                             const DcmTagKey &data,
                             const DcmTagKey &explanation,
                             const EL_BitsPerTableEntry descripMode,
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
        /* store number of items in the optional return variable */
        if (card != NULL)
            *card = count;
        if ((seq != NULL) && (pos < count))
        {
            DcmItem *item = seq->getItem(pos);
            Init(docu, item, descriptor, data, explanation, descripMode);
        }
    }
}


DiLookupTable::DiLookupTable(const DcmUnsignedShort &data,
                             const DcmUnsignedShort &descriptor,
                             const DcmLongString *explanation,
                             const EL_BitsPerTableEntry descripMode,
                             const signed long first,
                             EI_Status *status)
  : DiBaseLUT(),
    OriginalBitsAllocated(16),
    OriginalData(NULL)
{
    Uint16 us = 0;
    const DcmElement *descElem = OFreinterpret_cast(const DcmElement *, &descriptor);
    if (DiDocument::getElemValue(descElem, us, 0, OFTrue /*allowSigned*/) >= 3)         // number of LUT entries
    {
        Count = (us == 0) ? MAX_TABLE_ENTRY_COUNT : us;                                 // see DICOM supplement 5: "0" => 65536
        DiDocument::getElemValue(descElem, FirstEntry, 1, OFTrue /*allowSigned*/);      // can be SS or US (will be typecasted later)
        if ((first >= 0) && (FirstEntry != OFstatic_cast(Uint16, first)))
        {
            DCMIMGLE_WARN("invalid value for 'FirstInputValueMapped' in lookup table ("
                << FirstEntry << ") ... assuming " << first);
            FirstEntry = OFstatic_cast(Uint16, first);
        }
        DiDocument::getElemValue(descElem, us, 2, OFTrue /*allowSigned*/);              // bits per entry (only informational)
        unsigned long count = DiDocument::getElemValue(OFreinterpret_cast(const DcmElement *, &data), Data);
        OriginalData = OFstatic_cast(void *, OFconst_cast(Uint16 *, Data));             // store pointer to original data
        if (explanation != NULL)
            DiDocument::getElemValue(OFreinterpret_cast(const DcmElement *, explanation), Explanation);   // explanation (free form text)
        checkTable(count, us, descripMode, status);
     } else {
        if (status != NULL)
        {
            *status = EIS_MissingAttribute;
            DCMIMGLE_ERROR("incomplete or missing 'LookupTableDescriptor' " << descriptor.getTag());
        } else {
            DCMIMGLE_WARN("incomplete or missing 'LookupTableDescriptor' " << descriptor.getTag()
                << " ... ignoring LUT");
        }
     }
}


DiLookupTable::DiLookupTable(Uint16 *buffer,
                             const Uint32 count,
                             const Uint16 bits)
  : DiBaseLUT(buffer, count, bits),
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
                         DcmItem *item,
                         const DcmTagKey &descriptor,
                         const DcmTagKey &data,
                         const DcmTagKey &explanation,
                         const EL_BitsPerTableEntry descripMode,
                         EI_Status *status)
{
    Uint16 us = 0;
    if (docu->getValue(descriptor, us, 0, item, OFTrue /*allowSigned*/) >= 3)    // number of LUT entries
    {
        Count = (us == 0) ? MAX_TABLE_ENTRY_COUNT : us;                          // see DICOM supplement 5: "0" => 65536
        docu->getValue(descriptor, FirstEntry, 1, item, OFTrue /*allowSigned*/); // can be SS or US (will be typecasted later)
        docu->getValue(descriptor, us, 2, item, OFTrue /*allowSigned*/);         // bits per entry (only informational)
        unsigned long count = docu->getValue(data, Data, item);
        OriginalData = OFstatic_cast(void *, OFconst_cast(Uint16 *, Data));      // store pointer to original data
        if (explanation != DCM_UndefinedTagKey)
            docu->getValue(explanation, Explanation, 0 /*vm pos*/, item);        // explanation (free form text)
        checkTable(count, us, descripMode, status);
    } else {
        if (status != NULL)
        {
            *status = EIS_MissingAttribute;
            DCMIMGLE_ERROR("incomplete or missing 'LookupTableDescriptor' " << descriptor);
        } else {
            DCMIMGLE_WARN("incomplete or missing 'LookupTableDescriptor' " << descriptor
                << " ... ignoring LUT");
        }
    }
}


void DiLookupTable::checkTable(unsigned long count,
                               Uint16 bits,
                               const EL_BitsPerTableEntry descripMode,
                               EI_Status *status)
{
    if (count > 0)                                                            // valid LUT
    {
        unsigned long i;
        if (count > MAX_TABLE_ENTRY_COUNT)                                    // cut LUT length to maximum
            count = MAX_TABLE_ENTRY_COUNT;
        if (count != Count)                                                   // length of LUT differs from number of LUT entries
        {
            if (count == ((Count + 1) >> 1))                                  // bits allocated 8, ignore padding
            {
                OriginalBitsAllocated = 8;
                DCMIMGLE_DEBUG("lookup table uses 8 bits allocated ... converting to 16 bits");
                DataBuffer = new Uint16[Count];                               // create new LUT
                if ((DataBuffer != NULL) && (Data != NULL))
                {
                    const Uint8 *p = OFreinterpret_cast(const Uint8 *, Data);
                    Uint16 *q = DataBuffer;
                    if (gLocalByteOrder == EBO_BigEndian)                     // local machine has big endian byte ordering
                    {
                        DCMIMGLE_DEBUG("local machine has big endian byte ordering ... swapping 8 bit LUT entries");
                        for (i = count; i != 0; --i)                          // copy 8 bit entries to new 16 bit LUT (swap hi/lo byte)
                        {
                            *(q++) = *(p + 1);                                // copy low byte ...
                            *(q++) = *p;                                      // ... and then high byte
                            p += 2;                                           // jump to next hi/lo byte pair
                        }
                    } else {                                                  // local machine has little endian byte ordering (or unknown)
                        for (i = Count; i != 0; --i)
                            *(q++) = *(p++);                                  // copy 8 bit entries to new 16 bit LUT
                    }
                }
                Data = DataBuffer;
            } else {
                DCMIMGLE_WARN("invalid value for 'NumberOfTableEntries' (" << Count << ") ... assuming " << count);
                Count = count;
            }
        }
        MinValue = OFstatic_cast(Uint16, DicomImageClass::maxval(MAX_TABLE_ENTRY_SIZE));  // set minimum to maximum value
        const Uint16 *p = Data;
        Uint16 value;
        if (DataBuffer != NULL)                                               // LUT entries have been copied 8 -> 16 bits
        {
            for (i = Count; i != 0; --i)
            {
                value = *(p++);
                if (value < MinValue)                                         // get global minimum
                    MinValue = value;
                if (value > MaxValue)                                         // get global maximum
                    MaxValue = value;
            }
            checkBits(bits, 8, 0, descripMode);                               // set 'Bits'
        } else {
            int cmp = 0;
            for (i = Count; i != 0; --i)
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
                checkBits(bits, MIN_TABLE_ENTRY_SIZE, MAX_TABLE_ENTRY_SIZE, descripMode);  // set 'Bits'
            else
                checkBits(bits, MAX_TABLE_ENTRY_SIZE, MIN_TABLE_ENTRY_SIZE, descripMode);
        }
        Uint16 mask = OFstatic_cast(Uint16, DicomImageClass::maxval(Bits));   // mask lo-byte (8) or full word (16)
        if (((MinValue & mask) != MinValue) || ((MaxValue & mask) != MaxValue))
        {                                                                     // mask table entries and copy them to new LUT
            MinValue &= mask;
            MaxValue &= mask;
            if (DataBuffer == NULL)
                DataBuffer = new Uint16[Count];                               // create new LUT
            if (DataBuffer != NULL)
            {
                p = Data;
                Uint16 *q = DataBuffer;
                for (i = Count; i != 0; --i)
                    *(q++) = *(p++) & mask;
            }
            Data = DataBuffer;
        }
        Valid = (Data != NULL);                                               // lookup table is valid
    } else {
        if (status != NULL)
        {
            *status = EIS_InvalidValue;
            DCMIMGLE_ERROR("empty 'LookupTableData' attribute");
        } else {
            DCMIMGLE_WARN("empty 'LookupTableData' attribute ... ignoring LUT");
        }
    }
}


/********************************************************************/


void DiLookupTable::checkBits(const Uint16 bits,
                              const Uint16 rightBits,
                              const Uint16 wrongBits,
                              const EL_BitsPerTableEntry descripMode)
{
    /* is stored bit depth out of range? */
    if ((descripMode == ELM_IgnoreValue) || (bits < MIN_TABLE_ENTRY_SIZE) || (bits > MAX_TABLE_ENTRY_SIZE))
    {
        /* check whether correct bit depth can be determined automatically */
        Bits = (MaxValue > 0) ? OFstatic_cast(Uint16, DicomImageClass::tobits(MaxValue, 0)) : bits;
        /* check bit depth (again) for valid range */
        if (Bits < MIN_TABLE_ENTRY_SIZE)
            Bits = MIN_TABLE_ENTRY_SIZE;
        else if (Bits > MAX_TABLE_ENTRY_SIZE)
            Bits = MAX_TABLE_ENTRY_SIZE;
        /* check whether value has changed? */
        if (bits != Bits)
        {
            if (descripMode == ELM_IgnoreValue)
            {
                DCMIMGLE_INFO("ignoring value for 'BitsPerTableEntry' (" << bits << ") ... using " << Bits << " instead");
            } else {
                DCMIMGLE_WARN("unsuitable value for 'BitsPerTableEntry' (" << bits << ") ... valid range "
                    << MIN_TABLE_ENTRY_SIZE << "-" << MAX_TABLE_ENTRY_SIZE << ", using " << Bits);
            }
        }
    }
    else if ((descripMode == ELM_CheckValue) && (bits == wrongBits))
    {
        DCMIMGLE_WARN("unsuitable value for 'BitsPerTableEntry' (" << bits << ") ... assuming " << rightBits);
        Bits = rightBits;
    } else {
        /* assuming that the descriptor value is correct! */
        Bits = bits;
    }
}


int DiLookupTable::invertTable(const int flag)
{
    int result = 0;
    if ((Data != NULL) && (Count > 0) && (flag & 0x3))
    {
        Uint32 i;
        if (flag & 0x2)
        {
            if (OriginalData != NULL)
            {
                if (OriginalBitsAllocated == 8)
                {
                    if (Bits <= 8)
                    {
                        const Uint8 *p = OFconst_cast(const Uint8 *, OFstatic_cast(Uint8 *, OriginalData));
                        Uint8 *q = OFstatic_cast(Uint8 *, OriginalData);
                        const Uint8 max = OFstatic_cast(Uint8, DicomImageClass::maxval(Bits));
                        for (i = Count; i != 0; --i)
                            *(q++) = max - *(p++);
                        result |= 0x2;
                    }
                } else {
                    const Uint16 *p = OFconst_cast(const Uint16 *, OFstatic_cast(Uint16 *, OriginalData));
                    Uint16 *q = OFstatic_cast(Uint16 *, OriginalData);
                    const Uint16 max = OFstatic_cast(Uint16, DicomImageClass::maxval(Bits));
                    for (i = Count; i != 0; --i)
                        *(q++) = max - *(p++);
                    result |= 0x2;
                }
            }
        }
        if (flag & 0x1)
        {
            if (DataBuffer != NULL)
            {
                const Uint16 *p = OFconst_cast(const Uint16 *, DataBuffer);
                Uint16 *q = DataBuffer;
                const Uint16 max = OFstatic_cast(Uint16, DicomImageClass::maxval(Bits));
                for (i = Count; i != 0; --i)
                    *(q++) = max - *(p++);
                result |= 0x1;
            }
            else if (!(flag & 0x2))
            {
                DataBuffer = new Uint16[Count];
                if (DataBuffer != NULL)
                {
                    const Uint16 *p = Data;
                    Uint16 *q = DataBuffer;
                    const Uint16 max = OFstatic_cast(Uint16, DicomImageClass::maxval(Bits));
                    for (i = Count; i != 0; --i)
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
        Uint32 i;
        if (flag & 0x2)
        {
            if (OriginalData != NULL)
            {
                if (OriginalBitsAllocated == 8)
                {
                    if (Bits <= 8)
                    {
                        Uint8 *p = OFstatic_cast(Uint8 *, OriginalData) + (Count - 1);
                        Uint8 *q = OFstatic_cast(Uint8 *, OriginalData);
                        Uint8 val;
                        const unsigned long mid = Count / 2;
                        for (i = mid; i != 0; --i)
                        {
                            val = *q;
                            *(q++) = *p;
                            *(p--) = val;
                        }
                        result |= 0x2;
                    }
                } else {
                    Uint16 *p = OFstatic_cast(Uint16 *, OriginalData) + (Count - 1);
                    Uint16 *q = OFstatic_cast(Uint16 *, OriginalData);
                    Uint16 val;
                    const unsigned long mid = Count / 2;
                    for (i = mid; i != 0; --i)
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
                Uint16 *p = DataBuffer + (Count - 1);
                Uint16 *q = DataBuffer;
                Uint16 val;
                const unsigned long mid = Count / 2;
                for (i = mid; i != 0; --i)
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
                    Uint16 *p = OFconst_cast(Uint16 *, Data) + (Count - 1);
                    Uint16 *q = DataBuffer;
                    Uint16 val;
                    const unsigned long mid = Count / 2;
                    for (i = mid; i != 0; --i)
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
        const Uint16 bits = OFstatic_cast(Uint16, DicomImageClass::tobits(Count + FirstEntry));
        Uint16 *data = new Uint16[count];
        Uint8 *valid = new Uint8[count];
        if ((data != NULL) && (valid != NULL))
        {
            OFBitmanipTemplate<Uint8>::zeroMem(valid, count);   // initialize array
            Uint32 i;
            for (i = 0; i < Count; ++i)                         // 'copy' values to new array
            {
                if (!valid[Data[i]])
                    data[Data[i]] = OFstatic_cast(Uint16, i + FirstEntry);
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
                    Uint32 j = i + 1;
                    while ((j < count) && !valid[j])            // find next valid value
                        ++j;
                    if (valid[last])                            // check for starting conditions
                    {
                        const Uint32 mid = (j < count) ? (i + j) / 2 : count;
                        while (i < mid)
                        {                                       // fill first half with 'left' value
                            data[i] = data[last];
                            ++i;
                        }
                    }
                    if ((j < count) && valid[j])
                    {
                        while (i < j)                           // fill second half with 'right' value
                        {
                            data[i] = data[j];
                            ++i;
                        }
                        last = j;
                    }
                }
                ++i;
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


OFBool DiLookupTable::operator==(const DiBaseLUT &lut)
{
    return (compare(&lut) == 0);
}


OFBool DiLookupTable::operator==(const DiLookupTable &lut)
{
    return (compare(OFstatic_cast(const DiBaseLUT *, &lut)) == 0);
}
