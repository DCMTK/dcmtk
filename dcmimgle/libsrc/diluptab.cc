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
 *  Update Date:      $Date: 1998-11-27 16:04:33 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/diluptab.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
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
                             EI_Status *status)
  : Count(0),
    FirstEntry(0),
    Bits(0),
    Mask(0),
    MinValue(0),
    MaxValue(0),
    Valid(0),
    Data(NULL)
{
    if (docu != NULL)
        Init(docu, NULL, descriptor, data, status);
} 


DiLookupTable::DiLookupTable(const DiDocument *docu,
                             const DcmTagKey &sequence,
                             const DcmTagKey &descriptor,
                             const DcmTagKey &data,
                             const unsigned long pos,
                             unsigned long *card)
  : Count(0),
    FirstEntry(0),
    Bits(0),
    Mask(0),
    MinValue(0),
    MaxValue(0),
    Valid(0),
    Data(NULL)
{                                                                           // UNTESTED !!!
    if (docu != NULL)
    {
        DcmSequenceOfItems *seq = NULL;
        const unsigned long count = docu->getSequence(sequence, seq);
        if (card != NULL)
            *card = count;
        if ((seq != NULL) && (pos < count))
        {
            DcmItem *item = seq->getItem(pos);
            Init(docu, item, descriptor, data);
        }
    }
}


DiLookupTable::DiLookupTable(const DcmUnsignedShort &data,
                             const DcmUnsignedShort &descriptor,
                             EI_Status *status)
  : Count(0),
    FirstEntry(0),
    Bits(0),
    Mask(0),
    MinValue(0),
    MaxValue(0),
    Valid(0),
    Data(NULL)
{
    Uint16 us = 0;
    int ok = (DiDocument::getElemValue(descriptor, us, 0) > 0);
    Count = (ok && (us == 0)) ? MAX_TABLE_ENTRY_COUNT : us;                  // see DICOM supplement 5
    ok &= (DiDocument::getElemValue(descriptor, FirstEntry, 1) > 0);         // can be SS or US !?
    ok &= (DiDocument::getElemValue(descriptor, us, 2) > 0);                 // bits per entry (only informational)
    unsigned long count = DiDocument::getElemValue(data, Data);
    checkTable(ok, count, us, status);
}


/********************************************************************/


void DiLookupTable::Init(const DiDocument *docu, DcmObject *obj, const DcmTagKey &descriptor, const DcmTagKey &data,
    EI_Status *status)
{
    Uint16 us = 0;
    int ok = (docu->getValue(descriptor, us, 0, obj) > 0);
    Count = (ok && (us == 0)) ? MAX_TABLE_ENTRY_COUNT : us;                 // see DICOM supplement 5
    ok &= (docu->getValue(descriptor, FirstEntry, 1, obj) > 0);             // can be SS or US !?
    ok &= (docu->getValue(descriptor, us, 2, obj) > 0);                     // bits per entry (only informational)
    unsigned long count = docu->getValue(data, Data, obj);
    checkTable(ok, count, us, status);
}
    
    
    
void DiLookupTable::checkTable(const int ok,
                               unsigned long count,
                               Uint16 bits,
                               EI_Status *status)
{
    if (ok && count)                                                        // valid LUT
    {
        if (count > MAX_TABLE_ENTRY_COUNT)
            count = MAX_TABLE_ENTRY_COUNT;
        if (count != Count)
        {
            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
            {
                cerr << "WARNING: invalid value for 'NumberOfTableEntries' (" << Count << ") ";
                cerr << "... assuming " << count << " !" << endl;
            }
            Count = count;
        }
        register Uint16 i;
        register Uint16 value;
        int cmp = 0;
        MinValue = (Uint16)maxval(MAX_TABLE_ENTRY_SIZE);                    // set minimum to maximum value
        for (i = 0; i < Count; i++)
        {
            value = Data[i];
            if ((value & 0xff) != (value >> 8))                             // lo-byte not equal to hi-byte
                cmp = 1;
            if (value < MinValue)                                           // get global minimum
                MinValue = value;
            if (value > MaxValue)                                           // get global maximum
                MaxValue = value;
        }
        if (cmp == 0)                                                       // lo-byte is always equal to hi-byte
            checkBits(bits, MIN_TABLE_ENTRY_SIZE, MAX_TABLE_ENTRY_SIZE);    // set 'Bits'
        else
            checkBits(bits, MAX_TABLE_ENTRY_SIZE, MIN_TABLE_ENTRY_SIZE);
        Mask = (Uint16)maxval(Bits);                                        // mask lo-byte (8) or full word (16)
        MinValue &= Mask;
        MaxValue &= Mask;
        Valid = 1;                                                          // lookup table is valid
    }
    else if (status != NULL)
    {
        *status = EIS_MissingAttribute;
        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Errors)
            cerr << "ERROR: one or more mandatory table attributes are missing !" << endl;
    }       
}


/*--------------*
 *  destructor  *
 *--------------*/

DiLookupTable::~DiLookupTable()
{
}


/********************************************************************/


void DiLookupTable::checkBits(const Uint16 bits, const Uint16 right, const Uint16 wrong)
{
    if (bits == wrong)
    {
        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
        {
            cerr << "WARNING: unsuitable value for 'BitsPerTableEntry' (" << bits << ") ";
            cerr << "... assuming " << right << " !" << endl;
        }
    }
    else if (bits != right)
    {
        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
        {
            cerr << "WARNING: invalid value for 'BitsPerTableEntry' (" << bits << ") ";
            cerr << "... assuming " << right << " !" << endl;
        }
    }
    Bits = right;
}


/*
**
** CVS/RCS Log:
** $Log: diluptab.cc,v $
** Revision 1.1  1998-11-27 16:04:33  joergr
** Added copyright message.
** Added constructors to use external modality transformations.
** Added methods to support presentation LUTs and shapes.
**
** Revision 1.3  1998/05/11 14:52:30  joergr
** Added CVS/RCS header to each file.
**
**
*/
