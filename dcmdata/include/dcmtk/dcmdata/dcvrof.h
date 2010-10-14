/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Purpose: Interface of class DcmOtherFloat
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:43 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCVROF_H
#define DCVROF_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrfl.h"


/** a class representing the DICOM value representation 'Other Float String' (OF)
 */
class DcmOtherFloat
  : public DcmFloatingPointSingle
{

 public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmOtherFloat(const DcmTag &tag,
                  const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmOtherFloat(const DcmOtherFloat &old);

    /** destructor
     */
    virtual ~DcmOtherFloat();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmOtherFloat &operator=(const DcmOtherFloat &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmOtherFloat(*this);
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
     *  @return type identifier of this class (EVR_OF)
     */
    virtual DcmEVR ident() const;

    /** check whether stored value conforms to the VR and to the specified VM
     *  @param vm parameter not used for this VR
     *  @param oldFormat parameter not used for this VR (only for DA, TM, PN)
     *  @return always returns EC_Normal, i.e. currently no checks are performed
     */
    virtual OFCondition checkValue(const OFString &vm = "",
                                   const OFBool oldFormat = OFFalse);

    /** get value multiplicity
     *  @return always returns 1 (according to the DICOM standard)
     */
    virtual unsigned long getVM();
};


#endif // DCVROF_H


/*
 * CVS/RCS Log:
 * $Log: dcvrof.h,v $
 * Revision 1.9  2010-10-14 13:15:43  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.8  2010-04-23 15:26:13  joergr
 * Specify an appropriate default value for the "vm" parameter of checkValue().
 *
 * Revision 1.7  2010-04-23 14:25:27  joergr
 * Added new method to all VR classes which checks whether the stored value
 * conforms to the VR definition and to the specified VM.
 *
 * Revision 1.6  2009-11-04 09:58:07  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.5  2008-07-17 11:19:49  onken
 * Updated copyFrom() documentation.
 *
 * Revision 1.4  2008-07-17 10:30:23  onken
 * Implemented copyFrom() method for complete DcmObject class hierarchy, which
 * permits setting an instance's value from an existing object. Implemented
 * assignment operator where necessary.
 *
 * Revision 1.3  2005-12-08 16:29:04  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2004/07/01 12:28:25  meichel
 * Introduced virtual clone method for DcmObject and derived classes.
 *
 * Revision 1.1  2002/12/06 12:06:39  joergr
 * Added support for new value representation Other Float String (OF).
 *
 *
 */
