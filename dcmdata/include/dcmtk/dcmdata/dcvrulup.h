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
 *  Purpose: Interface of class DcmUnsignedLongOffset
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:43 $
 *  CVS/RCS Revision: $Revision: 1.21 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCVRULUP_H
#define DCVRULUP_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrul.h"


/** a class used for DICOMDIR byte offsets
 */
class DcmUnsignedLongOffset
  : public DcmUnsignedLong
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmUnsignedLongOffset(const DcmTag &tag,
                          const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmUnsignedLongOffset(const DcmUnsignedLongOffset &old);

    /** destructor
     */
    virtual ~DcmUnsignedLongOffset();

    /** assignment operator. 
     *  @param the offset to be copied
     */
    DcmUnsignedLongOffset &operator=(const DcmUnsignedLongOffset &);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmUnsignedLongOffset(*this);
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
     *  @return type identifier of this class (internal type: EVR_up)
     */
    virtual DcmEVR ident() const;

    /** clear the currently stored value
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition clear();

    /** get stored object reference
     *  @return pointer to the currently referenced object (might be NULL)
     */
    virtual DcmObject *getNextRecord();

    /** set stored object reference
     *  @param record pointer to the object to be referenced
     *  @return pointer to the newly referenced object (might be NULL)
     */
    virtual DcmObject *setNextRecord(DcmObject *record);

    /** check the currently stored element value
     *  @param autocorrect correct value length if OFTrue
     *  @return status, EC_Normal if value length is correct, an error code otherwise
     */
    virtual OFCondition verify(const OFBool autocorrect = OFFalse);

  private:

    /// pointer to the referenced object. NULL means that no object is referenced.
    DcmObject *nextRecord;
};


#endif // DCVRUSUP_H


/*
** CVS/RCS Log:
** $Log: dcvrulup.h,v $
** Revision 1.21  2010-10-14 13:15:43  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.20  2009-11-04 09:58:08  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.19  2008-07-17 11:19:49  onken
** Updated copyFrom() documentation.
**
** Revision 1.18  2008-07-17 10:30:23  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.17  2007-06-29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.16  2005/12/08 16:29:14  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.15  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.14  2002/12/06 12:49:20  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.13  2001/09/25 17:19:35  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.12  2001/06/01 15:48:54  meichel
** Updated copyright header
**
** Revision 1.11  2000/04/14 15:31:36  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.10  2000/03/08 16:26:27  meichel
** Updated copyright header.
**
** Revision 1.9  2000/03/03 14:05:28  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.8  2000/02/10 10:50:56  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.7  1999/03/31 09:25:10  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.6  1998/07/15 15:48:56  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.5  1997/07/21 08:25:17  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.4  1996/08/05 08:45:40  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.3  1996/01/05 13:23:11  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
