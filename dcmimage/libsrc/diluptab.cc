/*
**
** Author:  Joerg Riesmeier
** Created: 24.12.96
**
** Module:  diluptab.cc
**
** Purpose: DicomLookupTable (Source)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:52:30 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/Attic/diluptab.cc,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
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

DiLookupTable::DiLookupTable(const DiDocument *docu, const DcmTagKey &descriptor, const DcmTagKey &data,
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


DiLookupTable::DiLookupTable(const DiDocument *docu, const DcmTagKey &sequence, const DcmTagKey &descriptor,
    const DcmTagKey &data, const unsigned long pos, unsigned long *card)
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


/********************************************************************/


void DiLookupTable::Init(const DiDocument *docu, DcmObject *obj, const DcmTagKey &descriptor, const DcmTagKey &data,
    EI_Status *status)
{
    Uint16 us = 0;
    int ok = (docu->getValue(descriptor, us, 0, obj) > 0);
    Count = (ok && (us == 0)) ? MAX_TABLE_ENTRY_COUNT : us;                 // see DICOM supplement 5
    ok &= (docu->getValue(descriptor, FirstEntry, 1, obj) > 0);
    ok &= (docu->getValue(descriptor, us, 2, obj) > 0);                     // bits per entry (only informational)
    unsigned long count = docu->getValue(data, Data, obj);
    if (ok && count)                                                        // valid LUT
    {
        if (count > MAX_TABLE_ENTRY_COUNT)
            count = MAX_TABLE_ENTRY_COUNT;
        if (count != Count)
        {
            cerr << "WARNING: invalid value for 'NumberOfTableEntries' (" << Count << ") ";
            cerr << "... assuming " << (Count = count) << " !" << endl;
        }
/*
        register Uint16 i;
        register Uint16 test = 0;
        for (i = 0; i < Count; i++) 
            test |= (Data[i] & 0xff00);
        Bits = (test == 0) ? MIN_TABLE_ENTRY_SIZE : MAX_TABLE_ENTRY_SIZE;
        if (us != Bits)
        {
            cerr << "WARNING: invalid value for 'BitsPerTableEntry' (" << us << ") ";
            cerr << "... assuming " << Bits << " !" << endl;
        }
*/
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
            checkBits(us, MIN_TABLE_ENTRY_SIZE, MAX_TABLE_ENTRY_SIZE);      // set 'Bits'
        else
            checkBits(us, MAX_TABLE_ENTRY_SIZE, MIN_TABLE_ENTRY_SIZE);
        Mask = (Uint16)maxval(Bits);                                        // mask lo-byte (8) or full word (16)
        MinValue &= Mask;
        MaxValue &= Mask;
        Valid = 1;                                                          // lookup table is valid
    }
    else if (status != NULL)
    {
        *status = EIS_MissingAttribute;
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


void DiLookupTable::checkBits(const Uint16 us, const Uint16 right, const Uint16 wrong)
{
    if (us == wrong)
    {
        cerr << "WARNING: unsuitable value for 'BitsPerTableEntry' (" << us << ") ";
        cerr << "... assuming " << right << " !" << endl;
    }
    else if (us != right)
    {
        cerr << "WARNING: invalid value for 'BitsPerTableEntry' (" << us << ") ";
        cerr << "... assuming " << right << " !" << endl;
    }
    Bits = right;
}


/*
**
** CVS/RCS Log:
** $Log: diluptab.cc,v $
** Revision 1.3  1998-05-11 14:52:30  joergr
** Added CVS/RCS header to each file.
**
**
*/
