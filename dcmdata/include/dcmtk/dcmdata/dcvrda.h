/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
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
 */

#ifndef DCVRDA_H
#define DCVRDA_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcbytstr.h"
#include "dcmtk/ofstd/ofdate.h"


/** a class representing the DICOM value representation 'Date' (DA)
 */
class DCMTK_DCMDATA_EXPORT DcmDate
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
     *    (See DcmElement::checkVM() for a list of valid values.)
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
     *  If this function fails or the current element value is empty, the result
     *  variable 'formattedDate' is cleared automatically.
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
     *  If this function fails, the result variable 'dateValue' is cleared automatically.
     *  @param dicomDate string value in DICOM DA format to be converted to ISO format.
     *    An empty string is not regarded as valid input, since the date would be unknown.
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
     *  If this function fails or the specified DICOM date value is empty, the result
     *  variable 'formattedDate' is cleared automatically.
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
     *    (See DcmElement::checkVM() for a list of valid values.)
     *  @param oldFormat support old ACR/NEMA date format if OFTrue (i.e. with "." delimiters)
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    static OFCondition checkStringValue(const OFString &value,
                                        const OFString &vm = "1-n",
                                        const OFBool oldFormat = OFFalse);
};


#endif // DCVRDA_H
