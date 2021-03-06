/*
 *
 *  Copyright (C) 2011-2021, OFFIS e.V.
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
 */


#ifndef DCTMACRO_H
#define DCTMACRO_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcistrmb.h"
#include "dcmtk/dcmdata/dcostrmb.h"
#include "dcmtk/dcmdata/dcxfer.h"


/* Macros for handling the endianness */
#define LITTLE_ENDIAN_UINT16(w) OFstatic_cast(Uint8, (w) & 0xff), OFstatic_cast(Uint8, (w) >> 8)
#define LITTLE_ENDIAN_UINT32(w) LITTLE_ENDIAN_UINT16((w) & 0xffff), LITTLE_ENDIAN_UINT16((w) >> 16)

#define BIG_ENDIAN_UINT16(w) OFstatic_cast(Uint8, (w) >> 8), OFstatic_cast(Uint8, (w) & 0xff)
#define BIG_ENDIAN_UINT32(w) BIG_ENDIAN_UINT16((w) >> 16), BIG_ENDIAN_UINT16((w) & 0xffff)

/* Various definitions for "fake generating" DICOM datasets, explicit transfer syntaxes */
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

/* Various definitions for "fake generating" DICOM datasets, implicit transfer syntaxes */
#define IMPLICIT_TAG_AND_LENGTH(tag, length) \
    TAG(tag), ENDIAN_UINT32(length)

#define UNDEFINED_LENGTH  0xffffffff
#define UNDEFINED_LENGTH_SHORT 0xffff
#define VALUE 0x2a


/**
 * Read the dataset from the buffer into a dataset object.
 * @param dset the dataset to read into
 * @param buffer the buffer to read from
 * @param length the length of the buffer
 * @param ts the transfer syntax that is used
 * @return EC_Normal if reading worked, else an error
 */
static inline OFCondition readDataset(DcmDataset& dset, const Uint8* buffer, size_t length, E_TransferSyntax ts)
{
    DcmInputBufferStream stream;
    stream.setBuffer(buffer, OFstatic_cast(offile_off_t, length));
    stream.setEos();

    dset.clear();
    dset.transferInit();
    const OFCondition cond = dset.read(stream, ts);
    dset.transferEnd();

    return cond;
}

/**
 * Read the dataset from the buffer into a dataset object.
 * The difference to readDataset() is that this function then writes it out into
 * a buffer again which is read again. So this also tests the write functions.
 * @param dset the dataset to read itno
 * @param buffer the buffer to read from
 * @param length the length of the buffer
 * @param ts1 the transfer syntax that is used in buffer
 * @param ts2 the transfer syntax that should be used for the write/read cycle
 * @return EC_Normal if reading worked, else an error
 */
static inline OFCondition readDatasetTwice(DcmDataset& dset, const Uint8* buffer,
        size_t length, E_TransferSyntax ts1, E_TransferSyntax ts2, E_EncodingType encType = EET_ExplicitLength)
{
    OFCondition cond = readDataset(dset, buffer, length, ts1);
    if (cond.bad())
        return cond;

    // Write the dataset into a temporary buffer
    OFVector<Uint8> writeBuffer(length * 2);
    DcmOutputBufferStream out(&writeBuffer[0], OFstatic_cast(offile_off_t, writeBuffer.size()));
    dset.transferInit();
    cond = dset.write(out, ts1, encType, NULL);
    dset.transferEnd();

    // To avoid error messages, we must always flush the buffer
    void *outBuffer;
    offile_off_t outLength;
    out.flushBuffer(outBuffer, outLength);

    if (cond.bad())
        // Writing failed
        return cond;

    // Now read the dataset again
    return readDataset(dset, OFstatic_cast(const Uint8 *, outBuffer),
                    OFstatic_cast(size_t, outLength), ts2);
}


#endif // DCTMACRO_H
