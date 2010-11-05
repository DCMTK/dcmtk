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
 *  Author:  Gerd Ehlers, Andreas Barth, Joerg Riesmeier
 *
 *  Purpose: Interface of class DcmDate
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-11-05 09:34:11 $
 *  CVS/RCS Revision: $Revision: 1.23 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCVRDA_H
#define DCVRDA_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcbytstr.h"
#include "dcmtk/ofstd/ofdate.h"


/** a class representing the DICOM value representation 'Date' (DA)
 */
class DcmDate
  : public DcmByteString
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmDate(const DcmTag &tag,
            const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmDate(const DcmDate &old);

    /** destructor
     */
    virtual ~DcmDate();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmDate &operator=(const DcmDate &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmDate(*this);
    }

    /** Virtual object copying. This method can be used for DcmObject
     *  and derived classes to get a deep copy of an object. Internally
     *  the assignment operator is called if the given DcmObject parameter
     *  is of the same type as "this" object instance. If not, an error
     *  is returned. This function permits copying an object by value
     *  in a virtual way which therefore is different to just calling the
     *  assignment operator of DcmElement which could result in slicing
     *  the object.
     *  @param rhs - [in] The instance to copy from. Has to be of the same
     *                class type as "this" object
     *  @return EC_Normal if copying was successful, error otherwise
     */
    virtual OFCondition copyFrom(const DcmObject& rhs);

    /** get element type identifier
     *  @return type identifier of this class (EVR_DA)
     */
    virtual DcmEVR ident() const;

    /** check whether stored value conforms to the VR and to the specified VM
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (valid values: "1", "1-2", "1-3", "1-8", "1-99", "1-n", "2", "2-n", "2-2n",
     *                   "3", "3-n", "3-3n", "4", "6", "9", "16", "32")
     *  @param oldFormat support old ACR/NEMA format if OFTrue ('.' as a separator)
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    virtual OFCondition checkValue(const OFString &vm = "1-n",
                                   const OFBool oldFormat = OFFalse);

    /** get a copy of a particular string component
     *  @param stringVal variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize delete trailing spaces if OFTrue
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** set the element value to the current system date.
     *  The DICOM DA format supported by this function is "YYYYMMDD". If the current
     *  system date is unavailable the date is set to "19000101" and an error code is
     *  returned.
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition setCurrentDate();

    /** set the element value to the given date
     *  @param dateValue date to be set (should be a valid date)
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition setOFDate(const OFDate &dateValue);

    /** get the current element value in OFDate format.
     *  Please note that the element value is expected to be in valid DICOM DA format
     *  ("YYYYMMDD", "YYYY.MM.DD" is also supported for reasons of backward compatibility).
     *  If this function fails the result variable 'dateValue' is cleared automatically.
     *  @param dateValue reference to OFDate variable where the result is stored
     *  @param pos index of the element component in case of value multiplicity (0..vm-1)
     *  @param supportOldFormat if OFTrue support old (prior V3.0) date format (see above)
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition getOFDate(OFDate &dateValue,
                          const unsigned long pos = 0,
                          const OFBool supportOldFormat = OFTrue);

    /** get the current element value in ISO date format.
     *  The ISO date format supported by this function is "YYYY-MM-DD". Please note
     *  that the element value is expected to be in valid DICOM DA format ("YYYYMMDD",
     *  "YYYY.MM.DD" is also supported for reasons of backward compatibility).
     *  If this function fails the result variable 'formattedDate' is cleared automatically.
     *  @param formattedDate reference to string variable where the result is stored
     *  @param pos index of the element component in case of value multiplicity (0..vm-1)
     *  @param supportOldFormat if OFTrue support old (prior V3.0) date format (see above)
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition getISOFormattedDate(OFString &formattedDate,
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
    static OFCondition getCurrentDate(OFString &dicomDate);

    /** get the specified OFDate value in DICOM format.
     *  The DICOM DA format supported by this function is "YYYYMMDD". If the specified
     *  date is invalid the date is set to "19000101" and an error code is returned.
     *  @param dateValue date to be converted to DICOM format
     *  @param dicomDate reference to string variable where the result is stored
     *  @return EC_Normal upon success, an error code otherwise
     */
    static OFCondition getDicomDateFromOFDate(const OFDate &dateValue,
                                              OFString &dicomDate);

    /** get the specified DICOM date value in OFDate format.
     *  Please note that the specified value is expected to be in valid DICOM DA format
     *  ("YYYYMMDD", "YYYY.MM.DD" is also supported for reasons of backward compatibility).
     *  If this function fails the result variable 'dateValue' is cleared automatically.
     *  @param dicomDate string value in DICOM DA format to be converted to ISO format
     *  @param dateValue reference to OFDate variable where the result is stored
     *  @param supportOldFormat if OFTrue support old (prior V3.0) date format (see above)
     *  @return EC_Normal upon success, an error code otherwise
     */
    static OFCondition getOFDateFromString(const OFString &dicomDate,
                                           OFDate &dateValue,
                                           const OFBool supportOldFormat = OFTrue);

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
    static OFCondition getISOFormattedDateFromString(const OFString &dicomDate,
                                                     OFString &formattedDate,
                                                     const OFBool supportOldFormat = OFTrue);

    /** check whether given string value conforms to the VR "DA" (Date)
     *  and to the specified VM.
     *  @param value string value to be checked (possibly multi-valued)
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (valid values: "1", "1-2", "1-3", "1-8", "1-99", "1-n", "2", "2-n", "2-2n",
     *                   "3", "3-n", "3-3n", "4", "6", "9", "16", "32")
     *  @param oldFormat support old ACR/NEMA date format if OFTrue (i.e. with "." delimiters)
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    static OFCondition checkStringValue(const OFString &value,
                                        const OFString &vm = "1-n",
                                        const OFBool oldFormat = OFFalse);
};


#endif // DCVRDA_H


/*
** CVS/RCS Log:
** $Log: dcvrda.h,v $
** Revision 1.23  2010-11-05 09:34:11  joergr
** Added support for checking the value multiplicity "9" (see Supplement 131).
**
** Revision 1.22  2010-10-14 13:15:42  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.21  2010-04-23 15:26:13  joergr
** Specify an appropriate default value for the "vm" parameter of checkValue().
**
** Revision 1.20  2010-04-23 14:25:27  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.19  2010-04-22 09:31:30  joergr
** Revised misleading parameter documentation for the checkValue() method.
**
** Revision 1.18  2010-04-22 08:59:10  joergr
** Added support for further VM values ("1-8", "1-99", "16", "32") to be checked.
**
** Revision 1.17  2010-03-01 09:08:45  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.16  2009-08-03 09:05:30  joergr
** Added methods that check whether a given string value conforms to the VR and
** VM definitions of the DICOM standards.
**
** Revision 1.15  2008-07-17 11:19:49  onken
** Updated copyFrom() documentation.
**
** Revision 1.14  2008-07-17 10:30:23  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.13  2005-12-08 16:28:55  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.12  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.11  2002/12/06 12:49:14  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.10  2002/04/11 12:25:09  joergr
** Enhanced DICOM date, time and date/time classes. Added support for new
** standard date and time functions.
**
** Revision 1.9  2001/10/10 15:16:40  joergr
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
