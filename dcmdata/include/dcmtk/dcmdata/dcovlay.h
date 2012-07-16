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
 *  Author:  Andreas Barth
 *
 *  Purpose: Interface of class DcmOverlayData
 *
 */

#ifndef DCOVLAY_H
#define DCOVLAY_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcvrpobw.h"

/** a class used to represent overlay data (60xx,3000)
 */
class DCMTK_DCMDATA_EXPORT DcmOverlayData : public DcmPolymorphOBOW
{
public:

    /** constructor
     *  @param tag attribute tag
     *  @param len length of the attribute value
     */
    DcmOverlayData(const DcmTag &tag,
                   const Uint32 len = 0)
      : DcmPolymorphOBOW(tag, len)
    {
    }

    /** copy constructor
     *  @param oldObj element to be copied
     */
    DcmOverlayData(const DcmOverlayData &oldObj)
      : DcmPolymorphOBOW(oldObj)
    {
    }

    /** destructor
     */
    virtual ~DcmOverlayData()
    {
    }

    /** copy assignment operator
     *  @param obj element to be copied
     */
    DcmOverlayData &operator=(const DcmOverlayData &obj)
    {
      DcmPolymorphOBOW::operator=(obj);
      return *this;
    }

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmOverlayData(*this);
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
    virtual OFCondition copyFrom(const DcmObject &rhs)
    {
      if (this != &rhs)
      {
        if (rhs.ident() != ident()) return EC_IllegalCall;
        *this = OFstatic_cast(const DcmOverlayData &, rhs);
      }
      return EC_Normal;
    }

    /** return identifier for this class. Every class derived from this class
     *  returns a unique value of type enum DcmEVR for this call. This is used
     *  as a "poor man's RTTI" to correctly identify instances derived from
     *  this class even on compilers not supporting RTTI.
     *  @return type identifier of this class
     */
    virtual DcmEVR ident() const
    {
      return EVR_OverlayData;
    }
};

#endif
