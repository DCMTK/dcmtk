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
 *  Purpose: Interface of class DcmOverlayData
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-21 08:21:13 $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCOVLAY_H
#define DCOVLAY_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcvrpobw.h"

/** a class used to represent overlay data (60xx,3000)
 */
class DcmOverlayData : public DcmPolymorphOBOW
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

/*
** CVS/RCS Log:
** $Log: dcovlay.h,v $
** Revision 1.13  2010-10-21 08:21:13  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
** Added missing API documentation and slightly reformatted source code.
**
** Revision 1.12  2010-10-14 13:15:41  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.11  2009-11-04 09:58:07  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.10  2008-07-17 11:19:49  onken
** Updated copyFrom() documentation.
**
** Revision 1.9  2008-07-17 10:30:23  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.8  2007-11-29 14:30:35  meichel
** Updated doxygen API documentation
**
** Revision 1.7  2005/12/08 16:28:28  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.6  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.5  2001/06/01 15:48:42  meichel
** Updated copyright header
**
** Revision 1.4  2000/03/08 16:26:16  meichel
** Updated copyright header.
**
** Revision 1.3  1999/03/31 09:24:43  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.2  1998/11/12 16:47:41  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.1  1997/07/21 07:54:00  andreas
** - Support for CP 14. PixelData and OverlayData can have VR OW or OB
**   (depending on the transfer syntax). New internal value
**   representation (only for ident()) for OverlayData.
**
*/
