/*
 *
 *  Copyright (C) 1994-2015, OFFIS e.V.
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
 *  Purpose: Interface of class DcmPixelItem
 *
 */


#ifndef DCPXITEM_H
#define DCPXITEM_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcvrobow.h"
#include "dcmtk/dcmdata/dcofsetl.h"   /* for class DcmOffsetList */


/*
** Defines
*/

/// macro for the "pixel item" tag
#define DCM_PixelItemTag (DcmTag(DCM_Item, EVR_OB))


/** this class implements a container for a fragment of compressed pixel data.
 *  Instances of this class use the same attribute tags as sequence items,
 *  but are maintained within a pixel data element (class DcmPixelSequence)
 *  with undefined length and contain no DICOM structure, but raw data.
 *  Therefore, this class is derived from DcmOtherByteOtherWord, the class
 *  that is used for OB raw data which is handled very similar.
 */
class DCMTK_DCMDATA_EXPORT DcmPixelItem : public DcmOtherByteOtherWord
{
  public:

    /** constructor
     *  @param tag attribute tag
     *  @param len length of the attribute value
     */
    DcmPixelItem(const DcmTag &tag, const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmPixelItem(const DcmPixelItem &old);

    /// destructor
    virtual ~DcmPixelItem();

    /** copy assignment operator
     *  @param obj element to be copied
     */
    DcmPixelItem &operator=(const DcmPixelItem &obj) { DcmOtherByteOtherWord::operator=(obj); return *this; }

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmPixelItem(*this);
    }

    /** get type identifier
     *  @return type identifier of this class (EVR_item)
     */
    virtual DcmEVR ident(void) const { return EVR_pixelItem; }

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
    virtual OFCondition copyFrom(const DcmObject &rhs);

    /** check if this pixel item is nested in a pixel sequence, i.e.\ not a
     *  top-level or stand-alone item/dataset
     *  @return true if this item is nested, false otherwise
     */
    virtual OFBool isNested() const;

    /** get parent item of this object, i.e.\ the item/dataset in which the
     *  surrounding pixel sequence is stored.
     *  @return pointer to the parent item of this object (might be NULL)
     */
    virtual DcmItem *getParentItem();

    /** print all elements of the item to a stream
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName optional filename used to write the raw pixel data file
     *  @param pixelCounter optional counter used for automatic pixel data filename creation
     */
    virtual void print(STD_NAMESPACE ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    /** creates in this object an offset table for a compressed pixel sequence.
     *  @param offsetList list of size entries for each individual encoded frame
     *    provided by the compression codec. All entries are expected to have
     *    an even value (i.e. the pixel items are padded).
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createOffsetTable(const DcmOffsetList &offsetList);

    /** write object in XML format
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &out,
                                 const size_t flags = 0);

    /** special write method for creation of digital signatures
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @param wcache pointer to write cache object, may be NULL
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeSignatureFormat(DcmOutputStream &outStream,
                                             const E_TransferSyntax oxfer,
                                             const E_EncodingType enctype,
                                             DcmWriteCache *wcache);

  protected:

    /** write tag, VR and length field to the given output stream
     *  @param outStream output stream
     *  @param oxfer transfer syntax for writing
     *  @param writtenBytes number of bytes written to stream returned in this parameter
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeTagAndLength(DcmOutputStream &outStream,
                                          const E_TransferSyntax oxfer,
                                          Uint32 &writtenBytes) const;

};


#endif // DCPXITEM_H
