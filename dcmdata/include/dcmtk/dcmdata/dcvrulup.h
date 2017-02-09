/*
 *
 *  Copyright (C) 1994-2017, OFFIS e.V.
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
 */


#ifndef DCVRULUP_H
#define DCVRULUP_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrul.h"

class DcmDirectoryRecord;
class DcmItem;

/** a class used for DICOMDIR byte offsets
 */
class DCMTK_DCMDATA_EXPORT DcmUnsignedLongOffset
  : public DcmUnsignedLong
{

  public:

    // Make friend with DcmDirectoryRecord and DcmItem which require access
    // to protected constructor allowing construction using an explicit
    // value length.
    friend class DcmDirectoryRecord;
    friend class DcmItem;

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     */
    DcmUnsignedLongOffset(const DcmTag &tag);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmUnsignedLongOffset(const DcmUnsignedLongOffset &old);

    /** destructor
     */
    virtual ~DcmUnsignedLongOffset();

    /** assignment operator.
     *  @param obj the offset to be copied
     *  @return reference to this object
     */
    DcmUnsignedLongOffset &operator=(const DcmUnsignedLongOffset &obj);

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
    DcmUnsignedLongOffset(const DcmTag &tag,
                          const Uint32 len);

    /// pointer to the referenced object. NULL means that no object is referenced.
    DcmObject *nextRecord;
};


#endif // DCVRUSUP_H
