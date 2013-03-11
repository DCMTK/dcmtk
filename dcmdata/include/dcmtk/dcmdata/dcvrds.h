/*
 *
 *  Copyright (C) 1994-2013, OFFIS e.V.
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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmDecimalString
 *
 */


#ifndef DCVRDS_H
#define DCVRDS_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/dcmdata/dcbytstr.h"


/** a class representing the DICOM value representation 'Decimal String' (DS)
 */
class DCMTK_DCMDATA_EXPORT DcmDecimalString
  : public DcmByteString
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmDecimalString(const DcmTag &tag,
                     const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmDecimalString(const DcmDecimalString &old);

    /** destructor
     */
    virtual ~DcmDecimalString();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmDecimalString &operator=(const DcmDecimalString &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmDecimalString(*this);
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
     *  @return type identifier of this class (EVR_DS)
     */
    virtual DcmEVR ident() const;

    /** check whether stored value conforms to the VR and to the specified VM
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (See DcmElement::checkVM() for a list of valid values.)
     *  @param oldFormat parameter not used for this VR (only for DA, TM)
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    virtual OFCondition checkValue(const OFString &vm = "1-n",
                                   const OFBool oldFormat = OFFalse);

    /** get particular float value
     *  @param doubleVal reference to result variable
     *  @param pos index of the value to be retrieved (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFloat64(Float64 &doubleVal,
                                   const unsigned long pos = 0);

    /** get stored float values as a vector.
     *  Please note that only an element value consisting of zero or more spaces is considered
     *  as being empty and, therefore, results in an empty vector with status ".good()"; use
     *  isEmpty() before calling this method if you also want to check for other non-significant
     *  characters (e.g. the backslash).
     *  @param doubleVals reference to result variable
     *    (cleared automatically before entries are added)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFloat64Vector(OFVector<Float64> &doubleVals);

    /** get a particular value as a character string
     *  @param stringVal variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize delete leading and trailing spaces if OFTrue
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** write object in XML format
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &out,
                                 const size_t flags = 0);

    /* --- static helper functions --- */

    /** check whether given string value conforms to the VR "DS" (Decimal String)
     *  and to the specified VM.
     *  @param value string value to be checked (possibly multi-valued)
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (See DcmElement::checkVM() for a list of valid values.)
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    static OFCondition checkStringValue(const OFString &value,
                                        const OFString &vm = "1-n");
};


#endif // DCVRDS_H
