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
 *  Purpose: Interface of class DcmPixelSequence
 *
 */


#ifndef DCPIXSEQ_H
#define DCPIXSEQ_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcofsetl.h"   /* for class DcmOffsetList */


/*
** Forward declarations
*/

class DcmPixelItem;

/*
** Defines
*/

/// macro for the "pixel sequence" tag
#define DCM_PixelSequenceTag (DcmTag(DCM_PixelData, EVR_OB))


/** this class implements a sequence of pixel items, i.e. the data structure DICOM is using
 *  to store compressed pixel data. The object behaves very much like a sequence, but uses
 *  the pixel data tag (7FE0,0010) and OB value representation with undefined length,
 *  and the "items" contained within the sequence are in fact pixel items (class DcmPixelItem)
 *  that contain no list of DICOM elements but raw compressed pixel data.
 */
class DCMTK_DCMDATA_EXPORT DcmPixelSequence : public DcmSequenceOfItems
{
public:

    /** constructor
     *  @param tag attribute tag
     *  @param len length of the attribute value
     */
    DcmPixelSequence(const DcmTag &tag, const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmPixelSequence(const DcmPixelSequence &old);

    /// destructor
    virtual ~DcmPixelSequence();

    /** copy assignment operator
     *  @param obj element to be copied
     */
    DcmPixelSequence &operator=(const DcmPixelSequence &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmPixelSequence(*this);
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

    /** get type identifier
     *  @return type identifier of this class (EVR_item)
     */
    virtual DcmEVR ident(void) const { return EVR_pixelSQ; }

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

    /** calculate the length of this DICOM element when encoded with the
     *  given transfer syntax and the given encoding type for sequences.
     *  For elements, the length includes the length of the tag, length field,
     *  VR field and the value itself, for items and sequences it returns
     *  the length of the complete item or sequence including delimitation tags
     *  if applicable. Never returns undefined length.
     *  @param xfer transfer syntax for length calculation
     *  @param enctype sequence encoding type for length calculation
     *  @return length of DICOM element
     */
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    /** insert the given pixel item at the given position within the list maintained by this object.
     *  Ownership of the pixel item, which must be allocated on the heap, is transferred to the pixel sequence.
     *  @param item pointer to DcmPixelItem instance allocated on the heap, must not be NULL.
     *  @param where position at which the new item is to be inserted.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition insert(DcmPixelItem *item,
                               unsigned long where = DCM_EndOfListIndex);

    /** access a pixel item from the pixel sequence. This method returns a pointer to one
     *  of the pixel items in the list, and not a copy.
     *  @param item upon success, a pointer to the selected pixel item is returned in this parameter
     *  @param num index number of pixel item, must be < card()
     *  @return pointer to item if found, NULL if num >= card()
     */
    virtual OFCondition getItem(DcmPixelItem * &item,
                                const unsigned long num);

    /** remove pixel item from list. If found, the pixel item is not deleted but
     *  returned to the caller who is responsible for further management of the
     *  DcmPixelItem object.
     *  @param item upon success, a pointer to the removed pixel item is returned in this parameter
     *  @param num index number of item, must be < card()
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition remove(DcmPixelItem * &item,
                               const unsigned long num);

    /** remove pixel item from list. Tthe pixel item is not deleted;
     *  the caller is responsible for further management of the DcmPixelItem object.
     *  @param item pointer to element to be removed from list
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition remove(DcmPixelItem* item);

    /** changes the transfer syntax of this object to the given one.
     *  This only works if no transfer syntax was defined so far, or if the new and the old one
     *  are identical.
     *  @param newXfer
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition changeXfer(const E_TransferSyntax newXfer);

    /** check if this DICOM object can be encoded in the given transfer syntax.
     *  @param newXfer transfer syntax in which the DICOM object is to be encoded
     *  @param oldXfer transfer syntax in which the DICOM object was read or created.
     *  @return true if object can be encoded in desired transfer syntax, false otherwise.
     */
    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer);

    /** This function reads the information of all attributes which
     *  are captured in the input stream and captures this information
     *  in elementList. Each attribute is represented as an element
     *  in this list. If not all information for an attribute could be
     *  read from the stream, the function returns EC_StreamNotifyClient.
     *  @param inStream      The stream which contains the information.
     *  @param ixfer         The transfer syntax which was used to encode
     *                       the information in inStream.
     *  @param glenc         Encoding type for group length; specifies
     *                       what will be done with group length tags.
     *  @param maxReadLength Maximum read length for reading an attribute value.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmInputStream & inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** write object to a stream
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @param wcache pointer to write cache object, may be NULL
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype,
                              DcmWriteCache *wcache);

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

    /** appends a single compressed frame to this DICOM pixel sequence
     *  @param offsetList list containing offset table entries.
     *    Upon success, an entry is appended to the list. The offset values are always even,
     *    so it is expected that odd length pixel items are padded later during writing.
     *  @param compressedData pointer to compressed image data, must not be NULL
     *  @param compressedLen number of bytes of compressed image data
     *  @param fragmentSize maximum fragment size (in kbytes) for compression, 0 for unlimited.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition storeCompressedFrame(DcmOffsetList &offsetList,
                                             Uint8 *compressedData,
                                             Uint32 compressedLen,
                                             Uint32 fragmentSize);

protected:

    /** helper function for read(). Create sub-object (pixel item) of the
     *  appropriate type depending on the tag.
     *  @param newObject upon success, a pointer to the newly created object is returned in this parameter
     *  @param newTag tag of the sub-object to be created
     *  @param newLength length of the sub-object to be created
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition makeSubObject(DcmObject * &newObject,   // out
                                      const DcmTag &newTag,
                                      const Uint32 newLength);  // in

private:
    /** the transfer syntax in which the compressed pixel data maintained by this object
     *  is encoded. This may very well differ from the transfer syntax of the main dataset
     *  if this object was created by a compression codec in memory.
     */
    E_TransferSyntax Xfer;

    /// method inherited from base class that is useless in this class
    virtual OFCondition insert(DcmItem* /*item*/,
                               unsigned long /*where*/ = DCM_EndOfListIndex,
                               OFBool /*before*/ = OFFalse)
    {
        return EC_IllegalCall;
    }

    /// method inherited from base class that is useless in this class
    virtual DcmItem* getItem(const unsigned long /*num*/)
    {
        return NULL;
    }

    /// method inherited from base class that is useless in this class
    virtual DcmItem* remove(const unsigned long /*num*/)
    {
        return NULL;
    }

    /// method inherited from base class that is useless in this class
    virtual DcmItem* remove(DcmItem* /*item*/)
    {
        return NULL;
    }

};

#endif // DCPIXSEQ_H
