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
 *  Purpose: Interface of class DcmPixelItem
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:41 $
 *  CVS/RCS Revision: $Revision: 1.30 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCPXITEM_H
#define DCPXITEM_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcvrobow.h"
#include "dcmtk/dcmdata/dcofsetl.h"   /* for class DcmOffsetList */


/** this class implements a container for a fragment of compressed pixel data.
 *  Instances of this class use the same attribute tags as sequence items,
 *  but are maintained within a pixel data element (class DcmPixelSequence)
 *  with undefined length and contain no DICOM structure, but raw data.
 *  Therefore, this class is derived from DcmOtherByteOtherWord, the class
 *  that is used for OB raw data which is handled very similar.
 */
class DcmPixelItem : public DcmOtherByteOtherWord
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

/*
** CVS/RCS Log:
** $Log: dcpxitem.h,v $
** Revision 1.30  2010-10-14 13:15:41  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.29  2010-03-01 09:08:44  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.28  2010-02-22 11:39:54  uli
** Remove some unneeded includes.
**
** Revision 1.27  2009-02-04 17:54:31  joergr
** Fixed various layout and formatting issues.
**
** Revision 1.26  2009-02-04 10:18:19  joergr
** Fixed issue with compressed frames of odd length (possibly wrong values in
** basic offset table).
**
** Revision 1.25  2008-07-17 11:19:49  onken
** Updated copyFrom() documentation.
**
** Revision 1.24  2008-07-17 10:30:23  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.23  2007-11-29 14:30:19  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.22  2006/08/15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.21  2005/12/08 16:28:32  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.20  2005/11/24 12:50:57  meichel
** Fixed bug in code that prepares a byte stream that is fed into the MAC
**   algorithm when creating or verifying a digital signature. The previous
**   implementation was non-conformant when signatures included compressed
**   (encapsulated) pixel data because the item length was included in the byte
**   stream, while it should not. The global variable dcmEnableOldSignatureFormat
**   and a corresponding command line option in dcmsign allow to re-enable the old
**   implementation.
**
** Revision 1.19  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.18  2002/12/06 12:49:12  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.17  2002/08/27 16:55:38  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.16  2002/05/24 14:51:42  meichel
** Moved helper methods that are useful for different compression techniques
**   from module dcmjpeg to module dcmdata
**
** Revision 1.15  2002/04/25 09:41:46  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.14  2001/11/16 15:54:39  meichel
** Adapted digital signature code to final text of supplement 41.
**
** Revision 1.13  2001/09/25 17:19:28  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.12  2001/06/01 15:48:43  meichel
** Updated copyright header
**
** Revision 1.11  2000/04/14 15:31:33  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.10  2000/03/08 16:26:17  meichel
** Updated copyright header.
**
** Revision 1.9  2000/03/03 14:05:25  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.8  2000/02/10 10:50:53  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.7  1999/03/31 09:24:45  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.6  1998/11/12 16:47:43  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.5  1997/05/22 16:57:10  andreas
** - Corrected errors for writing of pixel sequences for encapsulated
**   transfer syntaxes.
**
** Revision 1.4  1996/01/29 13:38:13  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:22:58  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
