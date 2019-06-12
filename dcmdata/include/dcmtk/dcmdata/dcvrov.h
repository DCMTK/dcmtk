/*
 *
 *  Copyright (C) 2019, OFFIS e.V.
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
 *  Purpose: Interface of class DcmOther64bitVeryLong
 *
 */


#ifndef DCVROV_H
#define DCVROV_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvruv.h"


// forward declarations
class DcmJsonFormat;


/** a class representing the DICOM value representation 'Other 64-bit Very Long' (OV)
 */
class DCMTK_DCMDATA_EXPORT DcmOther64bitVeryLong
  : public DcmUnsigned64bitVeryLong
{

 public:

    // Make friend with DcmItem which requires access to protected
    // constructor allowing construction using an explicit value length.
    friend class DcmItem;

    /** constructor.
     *  Create new element from given tag.
     *  @param tag DICOM tag for the new element
     */
    DcmOther64bitVeryLong(const DcmTag &tag);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmOther64bitVeryLong(const DcmOther64bitVeryLong &old);

    /** destructor
     */
    virtual ~DcmOther64bitVeryLong();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmOther64bitVeryLong &operator=(const DcmOther64bitVeryLong &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmOther64bitVeryLong *clone() const
    {
      return new DcmOther64bitVeryLong(*this);
    }

    /** virtual object copying. This method can be used for DcmObject
     *  and derived classes to get a deep copy of an object. Internally,
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
     *  @return type identifier of this class (EVR_OV)
     */
    virtual DcmEVR ident() const;

    /** check whether stored value conforms to the VR and to the specified VM
     *  @param vm parameter not used for this VR
     *  @param oldFormat parameter not used for this VR (only for DA, TM)
     *  @return always returns EC_Normal, i.e. currently no checks are performed
     */
    virtual OFCondition checkValue(const OFString &vm = "",
                                   const OFBool oldFormat = OFFalse);

    /** get value multiplicity
     *  @return always returns 1 (according to the DICOM standard)
     */
    virtual unsigned long getVM();

    /** write object in XML format to a stream
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &out,
                                 const size_t flags = 0);

    /** write object in JSON format
     *  @param out output stream to which the JSON document is written
     *  @param format used to format and customize the output
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeJson(STD_NAMESPACE ostream &out,
                                  DcmJsonFormat &format);

  protected:

    /** constructor. Create new element from given tag and length.
     *  Only reachable from friend classes since construction with
     *  length different from 0 leads to a state with length being set but
     *  the element's value still being uninitialized. This can lead to crashes
     *  when the value is read or written. Thus the method calling this
     *  constructor with length > 0 must ensure that the element's value is
     *  explicitly initialized, too.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmOther64bitVeryLong(const DcmTag &tag,
                          const Uint32 len);

};


#endif // DCVROV_H
