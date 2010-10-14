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
 *  Purpose: DicomLookupTable (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:18 $
 *  CVS/RCS Revision: $Revision: 1.40 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
        DiDocument::getElemValue(descElem, FirstEntry, 1, OFTrue /*allowSigned*/);      // can be SS or US (will be type casted later)
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
                         DcmObject *obj,
                         const DcmTagKey &descriptor,
                         const DcmTagKey &data,
                         const DcmTagKey &explanation,
                         const EL_BitsPerTableEntry descripMode,
                         EI_Status *status)
{
    Uint16 us = 0;
    if (docu->getValue(descriptor, us, 0, obj, OFTrue /*allowSigned*/) >= 3)    // number of LUT entries
    {
        Count = (us == 0) ? MAX_TABLE_ENTRY_COUNT : us;                         // see DICOM supplement 5: "0" => 65536
        docu->getValue(descriptor, FirstEntry, 1, obj, OFTrue /*allowSigned*/); // can be SS or US (will be type casted later)
        docu->getValue(descriptor, us, 2, obj, OFTrue /*allowSigned*/);         // bits per entry (only informational)
        unsigned long count = docu->getValue(data, Data, obj);
        OriginalData = OFstatic_cast(void *, OFconst_cast(Uint16 *, Data));     // store pointer to original data
        if (explanation != DCM_UndefinedTagKey)
            docu->getValue(explanation, Explanation, 0 /*vm pos*/, obj);        // explanation (free form text)
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
        register unsigned long i;
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
                    register const Uint8 *p = OFreinterpret_cast(const Uint8 *, Data);
                    register Uint16 *q = DataBuffer;
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
        register const Uint16 *p = Data;
        register Uint16 value;
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
                register Uint16 *q = DataBuffer;
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
        Bits = (MaxValue > 0) ? DicomImageClass::tobits(MaxValue, 0) : bits;
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
        register Uint32 i;
        if (flag & 0x2)
        {
            if (OriginalData != NULL)
            {
                if (OriginalBitsAllocated == 8)
                {
                    if (Bits <= 8)
                    {
                        register const Uint8 *p = OFconst_cast(const Uint8 *, OFstatic_cast(Uint8 *, OriginalData));
                        register Uint8 *q = OFstatic_cast(Uint8 *, OriginalData);
                        const Uint8 max = OFstatic_cast(Uint8, DicomImageClass::maxval(Bits));
                        for (i = Count; i != 0; --i)
                            *(q++) = max - *(p++);
                        result |= 0x2;
                    }
                } else {
                    register const Uint16 *p = OFconst_cast(const Uint16 *, OFstatic_cast(Uint16 *, OriginalData));
                    register Uint16 *q = OFstatic_cast(Uint16 *, OriginalData);
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
                register const Uint16 *p = OFconst_cast(const Uint16 *, DataBuffer);
                register Uint16 *q = DataBuffer;
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
                    register const Uint16 *p = Data;
                    register Uint16 *q = DataBuffer;
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
        register Uint32 i;
        if (flag & 0x2)
        {
            if (OriginalData != NULL)
            {
                if (OriginalBitsAllocated == 8)
                {
                    if (Bits <= 8)
                    {
                        register Uint8 *p = OFstatic_cast(Uint8 *, OriginalData) + (Count - 1);
                        register Uint8 *q = OFstatic_cast(Uint8 *, OriginalData);
                        register Uint8 val;
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
                    register Uint16 *p = OFstatic_cast(Uint16 *, OriginalData) + (Count - 1);
                    register Uint16 *q = OFstatic_cast(Uint16 *, OriginalData);
                    register Uint16 val;
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
                register Uint16 *p = DataBuffer + (Count - 1);
                register Uint16 *q = DataBuffer;
                register Uint16 val;
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
                    register Uint16 *p = OFconst_cast(Uint16 *, Data) + (Count - 1);
                    register Uint16 *q = DataBuffer;
                    register Uint16 val;
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
        const Uint16 bits = DicomImageClass::tobits(Count + FirstEntry);
        Uint16 *data = new Uint16[count];
        Uint8 *valid = new Uint8[count];
        if ((data != NULL) && (valid != NULL))
        {
            OFBitmanipTemplate<Uint8>::zeroMem(valid, count);   // initialize array
            register Uint32 i;
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
                    register Uint32 j = i + 1;
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


/*
 *
 * CVS/RCS Log:
 * $Log: diluptab.cc,v $
 * Revision 1.40  2010-10-14 13:14:18  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.39  2010-10-06 14:48:47  joergr
 * Use DCM_UndefinedTagKey if no explanation tag can be specified for a LUT.
 *
 * Revision 1.38  2009-11-25 16:38:19  joergr
 * Fixed issue with LUT Descriptor elements using the value representation SS.
 * Revised logging messages.
 *
 * Revision 1.37  2009-10-28 14:26:02  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.36  2009-10-28 09:53:40  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.35  2009-02-26 10:08:10  joergr
 * Enhanced warning message text.
 *
 * Revision 1.34  2007/07/25 15:10:08  joergr
 * Fixed typo.
 *
 * Revision 1.33  2007/03/16 11:52:34  joergr
 * Introduced new flag that allows to select how to handle the BitsPerTableEntry
 * value in the LUT descriptor (use, ignore or check).
 * Fixed problem with wrong interpretation of the palette descriptor in certain
 * cases. Now the BitsPerTableEntry stored in the LUT descriptor is used by
 * default. The previous behavior can be enabled using the ELM_CheckValue mode.
 *
 * Revision 1.32  2006/08/15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.31  2005/12/08 15:42:53  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.30  2003/12/23 16:03:18  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.29  2003/12/17 16:18:34  joergr
 * Added new compatibility flag that allows to ignore the third value of LUT
 * descriptors and to determine the bits per table entry automatically.
 *
 * Revision 1.28  2003/12/08 18:10:20  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Added heuristics to determine the bit depth of a lookup table in case the
 * stored value is out of the expected range.
 *
 * Revision 1.27  2002/12/09 13:34:50  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.26  2002/07/18 12:36:02  joergr
 * Corrected typos.
 *
 * Revision 1.25  2002/06/26 16:12:01  joergr
 * Added new methods to get the explanation string of stored VOI windows and
 * LUTs (not only of the currently selected VOI transformation).
 *
 * Revision 1.24  2001/06/01 15:49:56  meichel
 * Updated copyright header
 *
 * Revision 1.23  2000/07/07 13:44:11  joergr
 * Added support for LIN OD presentation LUT shape.
 *
 * Revision 1.22  2000/05/03 09:47:24  joergr
 * Removed most informational and some warning messages from release built
 * (#ifndef DEBUG).
 *
 * Revision 1.21  2000/04/28 12:33:45  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.20  2000/04/27 13:10:29  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.19  2000/03/08 16:24:29  meichel
 * Updated copyright header.
 *
 * Revision 1.18  2000/03/06 18:20:35  joergr
 * Moved get-method to base class, renamed method and made method virtual to
 * avoid hiding of methods (reported by Sun CC 4.2).
 *
 * Revision 1.17  2000/03/03 14:09:19  meichel
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
