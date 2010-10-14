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
 *  Author:  Andrew Hewett
 *
 *  Purpose: Interface of class DcmUnlimitedText
 *           Value Representation UT is defined in Correction Proposal 101
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:43 $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCVRUT_H
#define DCVRUT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcchrstr.h"


/** a class representing the DICOM value representation 'Unlimited Text' (UT)
 */
class DcmUnlimitedText
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

    /// destructor
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

    /** return identifier for this class. Every class derived from this class
     *  returns a unique value of type enum DcmEVR for this call. This is used
     *  as a "poor man's RTTI" to correctly identify instances derived from
     *  this class even on compilers not supporting RTTI.
     *  @return type identifier of this class
     */
    virtual DcmEVR ident() const;

    /** check whether stored value conforms to the VR and to the specified VM
     *  @param vm parameter not used for this VR
     *  @param oldFormat parameter not used for this VR (only for DA, TM, PN)
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

    /** get a particular components of the string value
     *  @param stringVal string variable in which the result value is stored
     *  @param pos not used since value multiplicity is always 1
     *  @param normalize remove trailing spaces if OFTrue
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** get the string value (all compenents)
     *  @param stringVal string variable in which the result value is stored
     *  @param normalize remove trailing spaces if OFTrue
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFStringArray(OFString &stringVal,
                                         OFBool normalize = OFTrue);

    /* --- static helper functions --- */

    /** check whether given string value conforms to the VR "UT" (Unlimited Text)
     *  @param value string value to be checked (possibly multi-valued)
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    static OFCondition checkStringValue(const OFString &value);
};


#endif // DCVRUT_H


/*
** CVS/RCS Log:
** $Log: dcvrut.h,v $
** Revision 1.16  2010-10-14 13:15:43  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.15  2010-04-23 15:26:13  joergr
** Specify an appropriate default value for the "vm" parameter of checkValue().
**
** Revision 1.14  2010-04-23 14:25:27  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.13  2009-08-03 09:05:30  joergr
** Added methods that check whether a given string value conforms to the VR and
** VM definitions of the DICOM standards.
**
** Revision 1.12  2008-07-17 11:19:49  onken
** Updated copyFrom() documentation.
**
** Revision 1.11  2008-07-17 10:30:24  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.10  2007-11-29 14:30:35  meichel
** Updated doxygen API documentation
**
** Revision 1.9  2005/12/08 16:29:16  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.8  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.7  2002/12/06 12:49:21  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.6  2001/09/25 17:19:36  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.5  2001/06/01 15:48:55  meichel
** Updated copyright header
**
** Revision 1.4  2000/03/08 16:26:28  meichel
** Updated copyright header.
**
** Revision 1.3  1999/03/31 09:25:12  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.2  1998/11/12 16:47:58  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.1  1998/01/19 13:19:40  hewett
** Initial version.
**
*/
