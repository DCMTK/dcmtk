/*
 *
 *  Copyright (C) 1996-2014, OFFIS e.V.
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
class DCMTK_DCMIMGLE_EXPORT DiDocument
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
     ** @param  object  pointer to DICOM data structures (fileformat, dataset or item)
     *  @param  xfer    transfer syntax of the 'object'.
     *                  (could also be EXS_Unknown in case of fileformat or dataset)
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
     *  @param  pos          position in multi-valued elements (starting with 0)
     *  @param  item         pointer to item in dataset where to start (default: main dataset)
     *  @param  allowSigned  also allow signed value (Sint16) if true
     *
     ** @return VM if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           Uint16 &returnVal,
                           const unsigned long pos = 0,
                           DcmItem *item = NULL,
                           const OFBool allowSigned = OFFalse) const;

    /** get value of given tag (Sint16)
     *
     ** @param  tag        tag to search for
     *  @param  returnVal  reference to the storage area for the resulting value
     *  @param  pos        position in multi-valued elements (starting with 0)
     *  @param  item       pointer to item in dataset where to start (default: main dataset)
     *
     ** @return VM if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           Sint16 &returnVal,
                           const unsigned long pos = 0,
                           DcmItem *item = NULL) const;

    /** get value of given tag (Uint32)
     *
     ** @param  tag        tag to search for
     *  @param  returnVal  reference to the storage area for the resulting value
     *  @param  pos        position in multi-valued elements (starting with 0)
     *  @param  item       pointer to item in dataset where to start (default: main dataset)
     *
     ** @return VM if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           Uint32 &returnVal,
                           const unsigned long pos = 0,
                           DcmItem *item = NULL) const;

    /** get value of given tag (Sint32)
     *
     ** @param  tag        tag to search for
     *  @param  returnVal  reference to the storage area for the resulting value
     *  @param  pos        position in multi-valued elements (starting with 0)
     *  @param  item       pointer to item in dataset where to start (default: main dataset)
     *
     ** @return VM if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           Sint32 &returnVal,
                           const unsigned long pos = 0,
                           DcmItem *item = NULL) const;

    /** get value of given tag (double)
     *
     ** @param  tag        tag to search for
     *  @param  returnVal  reference to the storage area for the resulting value
     *  @param  pos        position in multi-valued elements (starting with 0)
     *  @param  item       pointer to item in dataset where to start (default: main dataset)
     *
     ** @return VM if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           double &returnVal,
                           const unsigned long pos = 0,
                           DcmItem *item = NULL) const;

    /** get value of given tag (Uint16 array)
     *
     ** @param  tag        tag to search for
     *  @param  returnVal  reference to the storage area for the resulting value
     *  @param  item       pointer to item in dataset where to start (default: main dataset)
     *
     ** @return VM / number of values if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           const Uint16 *&returnVal,
                           DcmItem *item = NULL) const;

    /** get value of given tag (const char *)
     *
     ** @param  tag        tag to search for
     *  @param  returnVal  reference to the storage area for the resulting value
     *  @param  item       pointer to item in dataset where to start (default: main dataset)
     *
     ** @return VM if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           const char *&returnVal,
                           DcmItem *item = NULL) const;

    /** get value of given tag (OFString)
     *
     ** @param  tag        tag to search for
     *  @param  returnVal  reference to the storage area for the resulting value
     *  @param  pos        position in multi-valued elements (starting with 0)
     *  @param  item       pointer to item in dataset where to start (default: main dataset)
     *
     ** @return VM if successful, 0 otherwise
     */
    unsigned long getValue(const DcmTagKey &tag,
                           OFString &returnVal,
                           const unsigned long pos = 0,
                           DcmItem *item = NULL) const;

    /** get sequence of given tag
     *
     ** @param  tag   tag to search for
     *  @param  seq   reference to the storage area for the resulting value
     *  @param  item  pointer to item in dataset where to start (default: main dataset)
     *
     ** @return cardinality if successful, 0 otherwise
     */
    unsigned long getSequence(const DcmTagKey &tag,
                              DcmSequenceOfItems *&seq,
                              DcmItem *item = NULL) const;

  // --- static helper functions ---

    /** get value of given element (Uint16)
     *
     ** @param  elem         element where the value is stored
     *  @param  returnVal    reference to the storage area for the resulting value
     *  @param  pos          position in multi-valued elements (starting with 0)
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
     *  @param  pos        position in multi-valued elements (starting with 0)
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
