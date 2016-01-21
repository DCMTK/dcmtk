/*
 *
 *  Copyright (C) 2016, OFFIS e.V.
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
 *  Purpose: Interface of class DcmOtherLong
 *
 */


#ifndef DCVROL_H
#define DCVROL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrul.h"


/** a class representing the DICOM value representation 'Other Long' (OL)
 */
class DCMTK_DCMDATA_EXPORT DcmOtherLong
  : public DcmUnsignedLong
{

 public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmOtherLong(const DcmTag &tag,
                 const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmOtherLong(const DcmOtherLong &old);

    /** destructor
     */
    virtual ~DcmOtherLong();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmOtherLong &operator=(const DcmOtherLong &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmOtherLong *clone() const
    {
      return new DcmOtherLong(*this);
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
     *  @return type identifier of this class (EVR_OL)
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
    virtual OFCondition writeXML(STD_NAMESPACE ostream&out,
                                 const size_t flags = 0);
};


#endif // DCVROL_H
