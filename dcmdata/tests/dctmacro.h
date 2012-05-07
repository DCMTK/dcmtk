/*
 *
 *  Copyright (C) 2011-2012, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: test program for reading DICOM datasets
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2012-05-07 10:06:54 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCTMACRO_H
#define DCTMACRO_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcistrmb.h"
#include "dcmtk/dcmdata/dcxfer.h"


/* Macros for handling the endianness */
#define LITTLE_ENDIAN_UINT16(w) OFstatic_cast(Uint8, (w) & 0xff), OFstatic_cast(Uint8, (w) >> 8)
#define LITTLE_ENDIAN_UINT32(w) LITTLE_ENDIAN_UINT16((w) & 0xffff), LITTLE_ENDIAN_UINT16((w) >> 16)

#define BIG_ENDIAN_UINT16(w) OFstatic_cast(Uint8, (w) >> 8), OFstatic_cast(Uint8, (w) & 0xff)
#define BIG_ENDIAN_UINT32(w) BIG_ENDIAN_UINT16((w) >> 16), BIG_ENDIAN_UINT16((w) & 0xffff)

/* Various definitions for "fake generating" DICOM datasets */
#define RESERVED_BYTES 0, 0
#define TAG(tag) ENDIAN_UINT16((tag).getGroup()), ENDIAN_UINT16((tag).getElement())
#define TAG_AND_LENGTH(tag, v, r, length) \
    TAG(tag), v, r, RESERVED_BYTES, ENDIAN_UINT32(length)
#define TAG_AND_LENGTH_SHORT(tag, v, r, length) \
    TAG(tag), v, r, ENDIAN_UINT16(length)
#define ITEM(length) \
    TAG(DCM_Item), ENDIAN_UINT32(length)
#define ITEM_END \
    TAG(DCM_ItemDelimitationItem), ENDIAN_UINT32(0)
#define SEQUENCE_END \
    TAG(DCM_SequenceDelimitationItem), ENDIAN_UINT32(0)

#define UNDEFINED_LENGTH  0xffffffff
#define UNDEFINED_LENGTH_SHORT 0xffff
#define VALUE 0x2a


static inline OFCondition readDataset(DcmDataset& dset, const Uint8* buffer, size_t length, E_TransferSyntax ts)
{
    DcmInputBufferStream stream;
    stream.setBuffer(buffer, length);
    stream.setEos();

    dset.clear();
    dset.transferInit();
    const OFCondition cond = dset.read(stream, ts);
    dset.transferEnd();

    return cond;
}


#endif // DCTMACRO_H

/*
 *
 * CVS/RCS Log:
 * $Log: dctmacro.h,v $
 * Revision 1.1  2012-05-07 10:06:54  uli
 * Added a test for the new functions for accessing parent objects and items.
 *
 *
 */
