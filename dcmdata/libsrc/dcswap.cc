/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:09 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
    if(oldByteOrder != EBO_unknown && newByteOrder != EBO_unknown )
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
    /* use register (if available) to increase speed */
    register Uint8 save;

    /* in case valWidth equals 2, swap correspondingly */
    if (valWidth == 2)
    {
	register Uint8 * first = &OFstatic_cast(Uint8*, value)[0];
	register Uint8 * second = &OFstatic_cast(Uint8*, value)[1];
	register Uint32 times = byteLength/2;
	while(times--)
	{
	    save = *first;
	    *first = *second;
	    *second = save;
	    first +=2;
	    second +=2;
	}
    }
    /* if valWidth is greater than 2, swap correspondingly */
    else if (valWidth > 2)
    {
	register size_t i;
	const size_t halfWidth = valWidth/2;
	const size_t offset = valWidth-1;
	register Uint8 *start;
	register Uint8 *end;

	Uint32 times = byteLength/valWidth;
	Uint8  *base = OFstatic_cast(Uint8 *, value);

	while (times--)
	{
	    i = halfWidth;
	    start = base;
	    end = base+offset;
	    while (i--)
	    {
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



/*
 * CVS/RCS Log:
 * $Log: dcswap.cc,v $
 * Revision 1.17  2010-10-14 13:14:09  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.16  2009-11-04 09:58:10  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.15  2005-12-08 15:41:38  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.14  2004/02/04 16:45:00  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.13  2001/11/01 14:55:43  wilkens
 * Added lots of comments.
 *
 * Revision 1.12  2001/09/25 17:19:54  meichel
 * Adapted dcmdata to class OFCondition
 *
 * Revision 1.11  2001/06/01 15:49:10  meichel
 * Updated copyright header
 *
 * Revision 1.10  2000/03/08 16:26:42  meichel
 * Updated copyright header.
 *
 * Revision 1.9  1999/03/31 09:25:40  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
