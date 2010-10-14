/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomDocument (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.22 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIDOCU_H
#define DIDOCU_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmimgle/diobjcou.h"
#include "dcmtk/ofstd/ofstring.h"

#define INCLUDE_CSTDDEF
#include "dcmtk/ofstd/ofstdinc.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmStack;
class DcmObject;
class DcmTagKey;
class DcmElement;
class DcmPixelData;
class DcmSequenceOfItems;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Interface class to DICOM data management (dcmdata).
 *  NB: This is an internal class of module dcmimgle/dcmimage.  Please do not
 *      use it for accessing DICOM data structures.  Instead use the classes
 *      and functions provided by dcmdata (e.g. class DcmFileFormat, DcmItem).
 */
class DiDocument
  : public DiObjectCounter
{

 public:

    /** constructor, open a DICOM file
     *
     ** @param  filename  the DICOM file
     *  @param  flags     configuration flags (only stored for later use)
     *  @param  fstart    first frame to be processed (only stored for later use)
     *  @param  fcount    number of frames (only stored for later use)
     */
    DiDocument(const char *filename,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);

    /** constructor, use a given DcmObject
     *
     ** @param  object  pointer to DICOM data structures
     *  @param  xfer    transfer syntax
     *  @param  flags   configuration flags (only stored for later use)
     *  @param  fstart  first frame to be processed (only stored for later use)
     *  @param  fcount  number of frames (only stored for later use)
     */
    DiDocument(DcmObject *object,
               const E_TransferSyntax xfer,
               const unsigned long flags = 0,
               const unsigned long fstart = 0,
               const unsigned long fcount = 0);

    /** destructor
     */
    virtual ~DiDocument();

    /** check whether current document is valid
     *
     ** @return status, true if successful, false otherwise
     */
    inline int good() const
    {
        return Object != NULL;
    }

    /** get current DICOM object (dataset)
     *
     ** @return pointer to DICOM object
     */
    inline DcmObject *getDicomObject() const
    {
        return Object;
    }

    /** get current DICOM dataset
     *
     ** @return pointer to DICOM dataset
     */
    inline DcmDataset *getDataset() const
    {
        return OFstatic_cast(DcmDataset *, Object);
    }

    /** get first frame to be processed
     *
     ** @return first frame to be processed
     */
    inline unsigned long getFrameStart() const
    {
        return FrameStart;
    }

    /** get number of frames to be processed
     *
     ** @return number of frames to be processed
     */
    inline unsigned long getFrameCount() const
    {
        return FrameCount;
    }

    /** get configuration flags
     *
     ** @return configuration flags
     */
    inline unsigned long getFlags() const
    {
        return Flags;
    }

    /** get transfer syntax of the DICOM dataset
     *
     ** @return transfer syntax
     */
    inline E_TransferSyntax getTransferSyntax() const
    {
        return Xfer;
    }

    /** get photometric interpretation (color model).
     *  Please note that this is the color model of the decompressed image which might
     *  deviate from the color model of the original compressed image.
     *
     ** @return photometric interpretation of the DICOM object
     */
    inline const char *getPhotometricInterpretation() const
    {
        return PhotometricInterpretation.c_str();
    }

    /** get pixel data object
     *
     ** @return reference to pixel data object (might be NULL)
     */
    inline DcmPixelData *getPixelData() const
    {
        return PixelData;
    }

    /** check whether pixel data only exist in compressed format
     *
     ** @return true if pixel data is compressed, false if an uncompressed version exists
     */
    inline OFBool isCompressed() const
    {
        return DcmXfer(Xfer).isEncapsulated();
    }

    /** search for given tag
     *
     ** @param  tag  tag to search for
     *  @param  obj  element in the dataset where the search should start (default: root)
     *
     ** @return pointer to element if successful, NULL otherwise
     */
    DcmElement *search(const DcmTagKey &tag,
                       DcmObject *obj = NULL) const;

    /** search for given tag and put result(s) on a stack
     *
     ** @param  tag    tag to search for
     *  @param  stack  stack where the result(s) should be stored
     *
     ** @return status, true if successful, false otherwise
     */
    int search(const DcmTagKey &tag,
               DcmStack &stack) const;


    /** get value multiplicity (VM) of given tag
     *
     ** @param  tag  tag to be searched
     *
     ** @return VM if successful, 0 otherwise
     */
    unsigned long getVM(const DcmTagKey &tag) const;

    /** get value of given tag (Uint16)
     *
     ** @param  tag          tag to search for
     *  @param  returnVal    reference to the storage area for the resulting value
     *  @param  pos          position in muti-valued elements (starting with 0)
     *  @param  item         element in the dataset where the search should start (default: root)
     *  @param  allowSigned  also allow signed value (Sint16) if true
     *
     ** @return VM if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           Uint16 &returnVal,
                           const unsigned long pos = 0,
                           DcmObject *item = NULL,
                           const OFBool allowSigned = OFFalse) const;

    /** get value of given tag (Sint16)
     *
     ** @param  tag        tag to search for
     *  @param  returnVal  reference to the storage area for the resulting value
     *  @param  pos        position in muti-valued elements (starting with 0)
     *
     ** @return VM if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           Sint16 &returnVal,
                           const unsigned long pos = 0) const;

    /** get value of given tag (Uint32)
     *
     ** @param  tag        tag to search for
     *  @param  returnVal  reference to the storage area for the resulting value
     *  @param  pos        position in muti-valued elements (starting with 0)
     *
     ** @return VM if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           Uint32 &returnVal,
                           const unsigned long pos = 0) const;

    /** get value of given tag (Sint32)
     *
     ** @param  tag        tag to search for
     *  @param  returnVal  reference to the storage area for the resulting value
     *  @param  pos        position in muti-valued elements (starting with 0)
     *
     ** @return VM if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           Sint32 &returnVal,
                           const unsigned long pos = 0) const;

    /** get value of given tag (double)
     *
     ** @param  tag        tag to search for
     *  @param  returnVal  reference to the storage area for the resulting value
     *  @param  pos        position in muti-valued elements (starting with 0)
     *
     ** @return VM if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           double &returnVal,
                           const unsigned long pos = 0) const;

    /** get value of given tag (Uint16 array)
     *
     ** @param  tag        tag to search for
     *  @param  returnVal  reference to the storage area for the resulting value
     *  @param  item       element in the dataset where the search should start (default: root)
     *
     ** @return VM / number of values if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           const Uint16 *&returnVal,
                           DcmObject *item = NULL) const;

    /** get value of given tag (const char *)
     *
     ** @param  tag        tag to search for
     *  @param  returnVal  reference to the storage area for the resulting value
     *  @param  item       element in the dataset where the search should start (default: root)
     *
     ** @return VM if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           const char *&returnVal,
                           DcmObject *item = NULL) const;

    /** get value of given tag (OFString)
     *
     ** @param  tag        tag to search for
     *  @param  returnVal  reference to the storage area for the resulting value
     *  @param  pos        position in muti-valued elements (starting with 0)
     *  @param  item       element in the dataset where the search should start (default: root)
     *
     ** @return VM if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           OFString &returnVal,
                           const unsigned long pos = 0,
                           DcmObject *item = NULL) const;

    /** get sequence of given tag
     *
     ** @param  tag  tag to search for
     *  @param  seq  reference to the storage area for the resulting value
     *
     ** @return cardinality if successful, 0 otherwise
     */
    unsigned long getSequence(const DcmTagKey &tag,
                              DcmSequenceOfItems *&seq) const;

  // --- static helper functions ---

    /** get value of given element (Uint16)
     *
     ** @param  elem         element where the value is stored
     *  @param  returnVal    reference to the storage area for the resulting value
     *  @param  pos          position in muti-valued elements (starting with 0)
     *  @param  allowSigned  also allow signed value (Sint16) if true
     *
     ** @return VM if successful, 0 otherwise
     */
    static unsigned long getElemValue(const DcmElement *elem,
                                      Uint16 &returnVal,
                                      const unsigned long pos = 0,
                                      const OFBool allowSigned = OFFalse);

    /** get value of given element (Uint16 array)
     *
     ** @param  elem       element where the value is stored
     *  @param  returnVal  reference to the storage area for the resulting value
     *
     ** @return VM / number of values if successful, 0 otherwise
     */
    static unsigned long getElemValue(const DcmElement *elem,
                                      const Uint16 *&returnVal);

    /** get value of given element (const char *)
     *
     ** @param  elem       element where the value is stored
     *  @param  returnVal  reference to the storage area for the resulting value
     *
     ** @return VM if successful, 0 otherwise
     */
    static unsigned long getElemValue(const DcmElement *elem,
                                      const char *&returnVal);

    /** get value of given element (OFString)
     *
     ** @param  elem       element where the value is stored
     *  @param  returnVal  reference to the storage area for the resulting value
     *  @param  pos        position in muti-valued elements (starting with 0)
     *
     ** @return VM if successful, 0 otherwise
     */
    static unsigned long getElemValue(const DcmElement *elem,
                                      OFString &returnVal,
                                      const unsigned long pos = 0);


 protected:

    /** convert pixel data to uncompressed representation (if required)
     */
    void convertPixelData();


 private:

    /// reference to DICOM dataset (in memory)
    DcmObject *Object;
    /// reference to DICOM fileformat (read from file, maybe NULL)
    DcmFileFormat *FileFormat;
    /// reference to pixel data object
    DcmPixelData *PixelData;
    /// transfer syntax used for reading the dataset
    E_TransferSyntax Xfer;

    /// first frame to be processed
    unsigned long FrameStart;
    /// number of frames to be processed
    unsigned long FrameCount;

    /// configuration flags
    unsigned long Flags;

    /// photometric interpretation (color model)
    OFString PhotometricInterpretation;

 // --- declarations to avoid compiler warnings

    DiDocument(const DiDocument &);
    DiDocument &operator=(const DiDocument &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: didocu.h,v $
 * Revision 1.22  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.21  2009-11-25 15:53:40  joergr
 * dapted code for new approach to access individual frames of a DICOM image.
 * Fixed issue with attributes that use a value representation of US or SS.
 *
 * Revision 1.20  2007/07/25 15:11:43  joergr
 * Enhanced misleading documentation.
 *
 * Revision 1.19  2005/12/08 16:47:38  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.18  2003/12/08 18:20:13  joergr
 * Added comment that class DiDocument is meant for internal purposes only.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated CVS header.
 *
 * Revision 1.17  2003/07/04 13:25:54  meichel
 * Replaced forward declarations for OFString with explicit includes,
 *   needed when compiling with HAVE_STD_STRING
 *
 * Revision 1.16  2003/06/12 15:08:34  joergr
 * Fixed inconsistent API documentation reported by Doxygen.
 *
 * Revision 1.15  2002/11/27 14:08:03  meichel
 * Adapted module dcmimgle to use of new header file ofstdinc.h
 *
 * Revision 1.14  2002/08/21 09:51:44  meichel
 * Removed DicomImage and DiDocument constructors that take a DcmStream
 *   parameter
 *
 * Revision 1.13  2002/06/26 16:01:07  joergr
 * Added new methods to get the explanation string of stored VOI windows and
 * LUTs (not only of the currently selected VOI transformation).
 *
 * Revision 1.12  2001/11/29 16:56:51  joergr
 * Fixed bug in dcmimgle that caused incorrect decoding of some JPEG compressed
 * images (certain DICOM attributes, e.g. photometric interpretation, might
 * change during decompression process).
 *
 * Revision 1.11  2001/11/19 12:55:29  joergr
 * Adapted code to support new dcmjpeg module and JPEG compressed images.
 *
 * Revision 1.10  2001/06/01 15:49:41  meichel
 * Updated copyright header
 *
 * Revision 1.9  2000/09/12 10:04:43  joergr
 * Corrected bug: wrong parameter for attribute search routine led to crashes
 * when multiple pixel data attributes were contained in the dataset (e.g.
 * IconImageSequence). Added new checking routines to avoid crashes when
 * processing corrupted image data.
 *
 * Revision 1.8  2000/03/08 16:24:15  meichel
 * Updated copyright header.
 *
 * Revision 1.7  2000/02/02 11:02:37  joergr
 * Removed space characters before preprocessor directives.
 *
 * Revision 1.6  1999/09/17 12:09:20  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.5  1999/03/24 17:19:59  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.4  1999/02/03 17:00:33  joergr
 * Added BEGIN_EXTERN_C and END_EXTERN_C to some C includes.
 *
 * Revision 1.3  1998/12/16 16:26:52  joergr
 * Added methods to use getOFString from class DcmElement (incl. multi value
 * fields).
 *
 * Revision 1.2  1998/12/14 17:16:32  joergr
 * Added (simplified) methods to return values of a given DcmElement object.
 *
 * Revision 1.1  1998/11/27 14:53:59  joergr
 * Added copyright message.
 * Added static methods to return the value of a given element.
 * Added support of frame start and count for future use.
 *
 * Revision 1.7  1998/07/01 08:39:20  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.6  1998/06/25 08:50:09  joergr
 * Added compatibility mode to support ACR-NEMA images and wrong
 * palette attribute tags.
 *
 * Revision 1.5  1998/05/11 14:53:15  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
