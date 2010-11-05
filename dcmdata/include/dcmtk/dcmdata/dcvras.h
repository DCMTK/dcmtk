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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmAgeString
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-11-05 09:34:11 $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCVRAS_H
#define DCVRAS_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcbytstr.h"


/** a class representing the DICOM value representation 'Age String' (AS)
 */
class DcmAgeString
  : public DcmByteString
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmAgeString(const DcmTag &tag,
                 const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmAgeString(const DcmAgeString &old);

    /** destructor
     */
    virtual ~DcmAgeString(void);

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmAgeString &operator=(const DcmAgeString &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmAgeString(*this);
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
     *  @return type identifier of this class (EVR_AS)
     */
    virtual DcmEVR ident(void) const;

    /** check whether stored value conforms to the VR and to the specified VM
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (valid values: "1", "1-2", "1-3", "1-8", "1-99", "1-n", "2", "2-n", "2-2n",
     *                   "3", "3-n", "3-3n", "4", "6", "9", "16", "32")
     *  @param oldFormat parameter not used for this VR (only for DA, TM, PN)
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    virtual OFCondition checkValue(const OFString &vm = "1-n",
                                   const OFBool oldFormat = OFFalse);

    /* --- static helper functions --- */

    /** check whether given string value conforms to the VR "AS" (Age String)
     *  and to the specified VM.
     *  @param value string value to be checked (possibly multi-valued)
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (valid values: "1", "1-2", "1-3", "1-8", "1-99", "1-n", "2", "2-n", "2-2n",
     *                   "3", "3-n", "3-3n", "4", "6", "9", "16", "32")
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    static OFCondition checkStringValue(const OFString &value,
                                        const OFString &vm = "1-n");
};


#endif // DCVRAS_H


/*
** CVS/RCS Log:
** $Log: dcvras.h,v $
** Revision 1.19  2010-11-05 09:34:11  joergr
** Added support for checking the value multiplicity "9" (see Supplement 131).
**
** Revision 1.18  2010-10-14 13:15:42  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.17  2010-04-23 15:26:12  joergr
** Specify an appropriate default value for the "vm" parameter of checkValue().
**
** Revision 1.16  2010-04-23 14:25:27  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.15  2010-04-22 09:31:30  joergr
** Revised misleading parameter documentation for the checkValue() method.
**
** Revision 1.14  2010-04-22 08:59:10  joergr
** Added support for further VM values ("1-8", "1-99", "16", "32") to be checked.
**
** Revision 1.13  2009-08-03 09:05:30  joergr
** Added methods that check whether a given string value conforms to the VR and
** VM definitions of the DICOM standards.
**
** Revision 1.12  2008-07-17 11:19:49  onken
** Updated copyFrom() documentation.
**
** Revision 1.11  2008-07-17 10:30:23  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.10  2005-12-08 16:28:52  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.9  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.8  2002/12/06 12:49:13  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.7  2001/06/01 15:48:48  meichel
** Updated copyright header
**
** Revision 1.6  2000/03/08 16:26:21  meichel
** Updated copyright header.
**
** Revision 1.5  1999/03/31 09:24:55  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.4  1998/11/12 16:47:45  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.3  1996/01/05 13:23:02  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
