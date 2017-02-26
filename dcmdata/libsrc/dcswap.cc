/*
 *
 *  Copyright (C) 1994-2017, OFFIS e.V.
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
 *  Author:  Andreas Barth
 *
 *  Purpose: byte order functions
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcswap.h"

OFCondition swapIfNecessary(const E_ByteOrder newByteOrder,
                            const E_ByteOrder oldByteOrder,
                            void * value, const Uint32 byteLength,
                            const size_t valWidth)
    /*
     * This function swaps byteLength bytes in value if newByteOrder and oldByteOrder
     * differ from each other. In case bytes have to be swapped, these bytes are seperated
     * in valWidth elements which will be swapped seperately.
     *
     * Parameters:
     *   newByteOrder - [in] The new byte ordering (little or big endian).
     *   oldByteOrder - [in] The current old byte ordering (little or big endian).
     *   value        - [in] Array that contains the actual bytes which might have to be swapped.
     *   byteLength   - [in] Length of the above array.
     *   valWidth     - [in] Specifies how many bytes shall be treated together as one element.
     */
{
    /* if the two byte orderings are unknown this is an illegal call */
    if (oldByteOrder != EBO_unknown && newByteOrder != EBO_unknown)
    {
        /* and if they differ from each other and valWidth is not 1 */
        if (oldByteOrder != newByteOrder && valWidth != 1)
        {
            /* in case the array length equals valWidth and only 2 or 4 bytes have to be swapped */
            /* we can swiftly swap these bytes by calling the corresponding functions. If this is */
            /* not the case we have to call a more sophisticated function. */
            if (byteLength == valWidth)
            {
                if (valWidth == 2)
                    swap2Bytes(OFstatic_cast(Uint8 *, value));
                else if (valWidth == 4)
                    swap4Bytes(OFstatic_cast(Uint8 *, value));
                else
                    swapBytes(value, byteLength, valWidth);
            }
            else
                swapBytes(value, byteLength, valWidth);
        }
        return EC_Normal;
    }
    return EC_IllegalCall;
}



void swapBytes(void * value, const Uint32 byteLength,
               const size_t valWidth)
    /*
     * This function swaps byteLength bytes in value. These bytes are seperated
     * in valWidth elements which will be swapped seperately.
     *
     * Parameters:
     *   value        - [in] Array that contains the actual bytes which might have to be swapped.
     *   byteLength   - [in] Length of the above array.
     *   valWidth     - [in] Specifies how many bytes shall be treated together as one element.
     */
{
    Uint8 save;

    /* in case valWidth equals 2, swap correspondingly */
    if (valWidth == 2)
    {
        Uint8 *first = &OFstatic_cast(Uint8*, value)[0];
        Uint8 *second = &OFstatic_cast(Uint8*, value)[1];
        Uint32 times = byteLength / 2;
        while(times)
        {
            --times;
            save = *first;
            *first = *second;
            *second = save;
            first += 2;
            second += 2;
        }
    }
    /* if valWidth is greater than 2, swap correspondingly */
    else if (valWidth > 2)
    {
        size_t i;
        const size_t halfWidth = valWidth / 2;
        const size_t offset = valWidth - 1;
        Uint8 *start;
        Uint8 *end;

        Uint32 times = OFstatic_cast(Uint32, byteLength / valWidth);
        Uint8  *base = OFstatic_cast(Uint8 *, value);

        while (times)
        {
            --times;
            i = halfWidth;
            start = base;
            end = base+offset;
            while (i)
            {
                --i;
                save = *start;
                *start++ = *end;
                *end-- = save;
            }
            base += valWidth;
        }
    }
}


Uint16 swapShort(const Uint16 toSwap)
{
    Uint8 *swapped = OFreinterpret_cast(Uint8 *, OFconst_cast(Uint16 *, &toSwap));
    swap2Bytes(swapped);
    return *OFreinterpret_cast(Uint16*, swapped);
}
