/*
 *
 *  Copyright (C) 1994-2016, OFFIS e.V.
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
 *  Author:  Andrew Hewett
 *
 *  Purpose: Interface of class DcmUnlimitedText
 *
 */


#ifndef DCVRUT_H
#define DCVRUT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcchrstr.h"


/** a class representing the DICOM value representation 'Unlimited Text' (UT)
 */
class DCMTK_DCMDATA_EXPORT DcmUnlimitedText
  : public DcmCharString
{

  public:

    /** constructor
     *  @param tag attribute tag
     *  @param len length of the attribute value
     */
    DcmUnlimitedText(const DcmTag &tag,
                     const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmUnlimitedText(const DcmUnlimitedText &old);

    /** destructor
     */
    virtual ~DcmUnlimitedText();

    /** copy assignment operator
     *  @param obj element to be copied
     */
    DcmUnlimitedText &operator=(const DcmUnlimitedText &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmUnlimitedText(*this);
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
     *  @return type identifier of this class (EVR_UT)
     */
    virtual DcmEVR ident() const;

    /** check whether stored value conforms to the VR and to the specified VM
     *  @param vm parameter not used for this VR
     *  @param oldFormat parameter not used for this VR (only for DA, TM)
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    virtual OFCondition checkValue(const OFString &vm = "",
                                   const OFBool oldFormat = OFFalse);

    /** get the value multiplicity.
     *  Since the backslash "\" is not regarded as a separator the value
     *  multiplicity is always 1.
     *  @return value multiplicity of the currently stored value
     */
    virtual unsigned long getVM();

    /** get a copy of a particular string component
     *  @param stringVal variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize delete leading and trailing spaces if OFTrue
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** get the string value (all components)
     *  @param stringVal string variable in which the result value is stored
     *  @param normalize remove trailing spaces if OFTrue
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFStringArray(OFString &stringVal,
                                         OFBool normalize = OFTrue);

    /* --- static helper functions --- */

    /** check whether given string value conforms to the VR "UT" (Unlimited Text)
     *  @param value string value to be checked
     *  @param charset character set (according to the value of the SpecificCharacterSet
     *    element) to be used for checking the string value. The default is ASCII (7-bit).
     *    Currently, the VR checker only supports ASCII (ISO_IR 6) and Latin-1 (ISO_IR 100).
     *    All other values disable the check of the value representation, e.g. "UNKNOWN".
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    static OFCondition checkStringValue(const OFString &value,
                                        const OFString &charset = "");
};


#endif // DCVRUT_H
