/*
 *
 *  Copyright (C) 1994-2001, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmDate
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-10-10 15:16:40 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrda.h,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCVRDA_H
#define DCVRDA_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */


#include "dctypes.h"
#include "dcbytstr.h"



/** a class representing the DICOM value representation 'Date' (DA)
 */
class DcmDate : public DcmByteString 
{
  public:
    DcmDate(const DcmTag &tag, const Uint32 len = 0);
    DcmDate( const DcmDate &newDA );
    virtual ~DcmDate();

    DcmDate &operator=(const DcmDate &obj) { DcmByteString::operator=(obj); return *this; }

    virtual DcmEVR ident() const { return EVR_DA; }

	/** set the element value to the current system date.
	 *  The DICOM DA format supported by this function is "YYYYMMDD". If the current
	 *  system date is unavailable the date is set to "19000101" and an error code is
	 *  returned.
	 *  @return EC_Normal upon success, an error code otherwise
	 */
    OFCondition setCurrentDate();
	
	/** get the current element value in ISO date format.
	 *  The ISO date format supported by this function is "YYYY-MM-DD". Please note
	 *  that the element value is expected to be in valid DICOM DA format ("YYYYMMDD",
	 *  "YYYY.MM.DD" is also supported for reasons of backward compatibility).
	 *	If this function fails the result variable 'formattedDate' is cleared automatically.
	 *  @param formattedDate reference to string variable where the result is stored
	 *  @param pos index of the element component in case of value multiplicity (0..vm-1)
	 *  @param supportOldFormat if OFTrue support old (prior V3.0) date format (see above)
	 *  @return EC_Normal upon success, an error code otherwise
	 */
    OFCondition getISOFormattedDate(
        OFString &formattedDate,
        const unsigned long pos = 0,
        const OFBool supportOldFormat = OFTrue);
                                    
    /* --- static helper functions --- */

	/** get the current system date.
	 *  The DICOM DA format supported by this function is "YYYYMMDD". If the current
	 *  system date is unavailable the date is set to "19000101" and an error code is
	 *  returned.
	 *  @param dicomDate reference to string variable where the result is stored
	 *  @return EC_Normal upon success, an error code otherwise
	 */
    static OFCondition getCurrentDate(
        OFString &dicomDate);

	/** get the specified DICOM date value in ISO format.
	 *  The ISO date format supported by this function is "YYYY-MM-DD". Please note
	 *  that the specified value is expected to be in valid DICOM DA format ("YYYYMMDD",
	 *  "YYYY.MM.DD" is also supported for reasons of backward compatibility).
	 *  If this function fails the result variable 'formattedDate' is cleared automatically.
	 *  @param dicomDate string value in DICOM DA format to be converted to ISO format
	 *  @param formattedDate reference to string variable where the result is stored
	 *  @param supportOldFormat if OFTrue support old (prior V3.0) date format (see above)
	 *  @return EC_Normal upon success, an error code otherwise
	 */
    static OFCondition getISOFormattedDateFromString(
        const OFString &dicomDate,
        OFString &formattedDate,
        const OFBool supportOldFormat = OFTrue);

};


#endif // DCVRDA_H

/*
** CVS/RCS Log:
** $Log: dcvrda.h,v $
** Revision 1.9  2001-10-10 15:16:40  joergr
** Added new flag to date/time routines allowing to choose whether the old
** prior V3.0 format for the corresponding DICOM VRs is supported or not.
**
** Revision 1.8  2001/10/01 15:01:38  joergr
** Introduced new general purpose functions to get/set person names, date, time
** and date/time.
**
** Revision 1.7  2001/06/01 15:48:49  meichel
** Updated copyright header
**
** Revision 1.6  2000/03/08 16:26:22  meichel
** Updated copyright header.
**
** Revision 1.5  1999/03/31 09:24:58  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.4  1998/11/12 16:47:47  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.3  1996/01/05 13:23:04  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
