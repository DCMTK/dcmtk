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
 *  Purpose: Interface of class DcmPixelSequence
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:41 $
 *  CVS/RCS Revision: $Revision: 1.37 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCPIXSEQ_H
#define DCPIXSEQ_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcofsetl.h"   /* for class DcmOffsetList */

class DcmPixelItem;

/** this class implements a sequence of pixel items, i.e. the data structure DICOM is using
 *  to store compressed pixel data. The object behaves very much like a sequence, but uses
 *  the pixel data tag (7FE0,0010) and OB value representation with undefined length,
 *  and the "items" contained within the sequence are in fact pixel items (class DcmPixelItem)
 *  that contain no list of DICOM elements but raw compressed pixel data.
 */
class DcmPixelSequence : public DcmSequenceOfItems
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


/*
** CVS/RCS Log:
** $Log: dcpixseq.h,v $
** Revision 1.37  2010-10-14 13:15:41  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.36  2010-03-01 09:08:44  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.35  2010-02-22 11:39:54  uli
** Remove some unneeded includes.
**
** Revision 1.34  2009-02-04 17:54:30  joergr
** Fixed various layout and formatting issues.
**
** Revision 1.33  2009-02-04 10:18:19  joergr
** Fixed issue with compressed frames of odd length (possibly wrong values in
** basic offset table).
**
** Revision 1.32  2008-07-17 11:19:49  onken
** Updated copyFrom() documentation.
**
** Revision 1.31  2008-07-17 10:30:23  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.30  2008-06-23 12:09:13  joergr
** Fixed inconsistencies in Doxygen API documentation.
**
** Revision 1.29  2007/11/29 14:30:19  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.28  2006/08/15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.27  2005/12/08 16:28:31  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.26  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.25  2002/12/06 12:49:12  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.24  2002/08/27 16:55:38  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.23  2002/05/24 14:51:42  meichel
** Moved helper methods that are useful for different compression techniques
**   from module dcmjpeg to module dcmdata
**
** Revision 1.22  2001/09/25 17:19:28  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.21  2001/06/01 15:48:42  meichel
** Updated copyright header
**
** Revision 1.20  2000/11/07 16:56:08  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.19  2000/04/14 16:02:23  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.18  2000/03/08 16:26:17  meichel
** Updated copyright header.
**
** Revision 1.17  2000/03/06 18:11:30  joergr
** Local variable hided member variable (reported by Sun CC 4.2).
**
** Revision 1.16  2000/03/03 14:41:56  joergr
** Corrected bug related to padding of file and item size.
**
** Revision 1.15  2000/03/03 14:05:25  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.14  2000/02/10 10:50:53  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.13  1999/03/31 09:24:44  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.12  1998/11/12 16:47:42  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.11  1998/07/15 15:48:51  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.10  1997/07/21 08:25:10  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.9  1997/05/27 13:48:29  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.8  1997/05/22 16:57:10  andreas
** - Corrected errors for writing of pixel sequences for encapsulated
**   transfer syntaxes.
**
** Revision 1.7  1997/05/06 09:22:37  hewett
** Added a "before" flag to the insertion of items for compatibility with
** insertion in normal Sequences.
**
** Revision 1.6  1996/08/05 08:45:27  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.5  1996/01/29 13:38:13  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.4  1996/01/24 09:34:55  andreas
** Support for 64 bit long
**
** Revision 1.3  1996/01/05 13:22:58  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
