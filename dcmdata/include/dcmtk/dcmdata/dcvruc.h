/*
 *
 *  Copyright (C) 2015-2016, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Interface of class DcmUnlimitedCharacters
 *
 */


#ifndef DCVRUC_H
#define DCVRUC_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcchrstr.h"


/** a class representing the DICOM value representation 'Unlimited Characters' (UC)
 */
class DCMTK_DCMDATA_EXPORT DcmUnlimitedCharacters
  : public DcmCharString
{

  public:

    /** constructor
     *  @param tag attribute tag
     *  @param len length of the attribute value
     */
    DcmUnlimitedCharacters(const DcmTag &tag,
                           const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmUnlimitedCharacters(const DcmUnlimitedCharacters &old);

    /** destructor
     */
    virtual ~DcmUnlimitedCharacters();

    /** copy assignment operator
     *  @param obj element to be copied
     */
    DcmUnlimitedCharacters &operator=(const DcmUnlimitedCharacters &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmUnlimitedCharacters(*this);
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
     *                    class type as "this" object
     *  @return EC_Normal if copying was successful, error otherwise
     */
    virtual OFCondition copyFrom(const DcmObject& rhs);

    /** get element type identifier
     *  @return type identifier of this class (EVR_UC)
     */
    virtual DcmEVR ident() const;

    /** check whether stored value conforms to the VR and to the specified VM
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (See DcmElement::checkVM() for a list of valid values.)
     *  @param oldFormat parameter not used for this VR (only for DA, TM)
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    virtual OFCondition checkValue(const OFString &vm = "",
                                   const OFBool oldFormat = OFFalse);

    /** get a copy of a particular string component
     *  @param stringVal variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize delete leading and trailing spaces if OFTrue
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /* --- static helper functions --- */

    /** check whether given string value conforms to the VR "UC" (Unlimited Characters)
     *  and to the specified VM.
     *  @param value string value to be checked
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (See DcmElement::checkVM() for a list of valid values.)
     *  @param charset character set (according to the value of the SpecificCharacterSet
     *    element) to be used for checking the string value. The default is ASCII (7-bit).
     *    Currently, the VR checker only supports ASCII (ISO_IR 6) and Latin-1 (ISO_IR 100).
     *    All other values disable the check of the value representation, e.g. "UNKNOWN".
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    static OFCondition checkStringValue(const OFString &value,
                                        const OFString &vm = "1-n",
                                        const OFString &charset = "");
};


#endif // DCVRUC_H
