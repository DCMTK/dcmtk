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
 *  Purpose: Interface of class DcmFileFormat
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-20 16:34:08 $
 *  CVS/RCS Revision: $Revision: 1.38 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCFILEFO_H
#define DCFILEFO_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcdatset.h"


// forward declarations
class DcmMetaInfo;
class DcmInputStream;
class DcmOutputStream;
class DcmRepresentationParameter;


/** a class handling the DICOM file format (with meta header)
 */
class DcmFileFormat
  : public DcmSequenceOfItems
{

 public:

    /** default constructor
     */
    DcmFileFormat();

    /** constructor
     *  @param dataset to be copied (!) into the new DcmFileFormat object
     */
    DcmFileFormat(DcmDataset *dataset);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmFileFormat(const DcmFileFormat &old);

    /** destructor
     */
    virtual ~DcmFileFormat();

    /** assignment operator
     *  @param obj fileformat to be assigned/copied
     *  @return reference to this object
     */
    DcmFileFormat &operator=(const DcmFileFormat &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmFileFormat(*this);
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
     *  @return type identifier of this class (EVR_fileFormat)
     */
    virtual DcmEVR ident() const;

    /** clear file meta information header and dataset
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition clear();

    /** remove all elements with an invalid group number from the meta information header
     *  and dataset
     */
    virtual void removeInvalidGroups();

    /** print meta header and dataset to a stream
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName optional filename used to write the raw pixel data file
     *  @param pixelCounter optional counter used for automatic pixel data filename creation
     */
    virtual void print(STD_NAMESPACE ostream&out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    /** make sure that all data elements of the file meta information header are existent
     *  in metainfo and contain correct values.
     *  @param oxfer the transfer syntax which shall be used
     *  @param writeMode flag indicating whether to update the file meta information or not
     */
    virtual OFCondition validateMetaInfo(const E_TransferSyntax oxfer,
                                         const E_FileWriteMode writeMode = EWM_fileformat);

    /** get file meta information header part of the fileformat
     *  @return reference to internally stored file meta information header
     */
    DcmMetaInfo *getMetaInfo();

    /** get dataset part of the fileformat
     *  @return reference to internally stored dataset
     */
    DcmDataset *getDataset();

    /** get dataset part and remove it from the fileformat.
     *  Please note that a new, initially empty dataset is also added to the fileformat.
     *  @return reference to previously stored but now removed dataset
     */
    DcmDataset *getAndRemoveDataset();

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

    /** check if this DICOM object can be encoded in the given transfer syntax.
     *  @param newXfer transfer syntax in which the DICOM object is to be encoded
     *  @param oldXfer transfer syntax in which the DICOM object was read or created.
     *  @return true if object can be encoded in desired transfer syntax, false otherwise.
     */
    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer = EXS_Unknown);

    /** read object from a stream.
     *  @param inStream DICOM input stream
     *  @param xfer transfer syntax to use when parsing
     *  @param glenc handling of group length parameters
     *  @param maxReadLength attribute values larger than this value are skipped
     *    while parsing and read later upon first access if the stream type supports
     *    this.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmInputStream &inStream,
                             const E_TransferSyntax xfer = EXS_Unknown,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** write fileformat to a stream
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

    /** write object to a stream (abstract)
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @param wcache pointer to write cache object, may be NULL
     *  @param glenc group length encoding
     *  @param padenc dataset trailing padding encoding
     *  @param padlen padding structure size for complete file
     *  @param subPadlen padding structure set for sequence items
     *  @param instanceLength number of extra bytes added to the item/dataset
     *    length used when computing the padding. This parameter is for instance
     *    used to pass the length of the file meta information header from the
     *    DcmFileFormat to the DcmDataset object.
     *  @param writeMode write file with or without meta header. Also allows for
     *    updating the information in the file meta information header.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype,
                              DcmWriteCache *wcache,
                              const E_GrpLenEncoding glenc,
                              const E_PaddingEncoding padenc = EPD_noChange,
                              const Uint32 padlen = 0,
                              const Uint32 subPadlen = 0,
                              Uint32 instanceLength = 0,
                              const E_FileWriteMode writeMode = EWM_fileformat);

    /** write object in XML format.
     *  The XML declaration (e.g. <?xml version="1.0"?>) is not written by this function.
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream&out,
                                 const size_t flags = 0);

    /** load object from a DICOM file.
     *  This method supports DICOM objects stored as a file (with meta header) or as a
     *  dataset (without meta header).  By default, the presence of a meta header is
     *  detected automatically.
     *  @param fileName name of the file to load
     *  @param readXfer transfer syntax used to read the data (auto detection if EXS_Unknown)
     *  @param groupLength flag, specifying how to handle the group length tags
     *  @param maxReadLength maximum number of bytes to be read for an element value.
     *    Element values with a larger size are not loaded until their value is retrieved
     *    (with getXXX()) or loadAllDataElements() is called.
     *  @param readMode read file with or without meta header, i.e. as a fileformat or a
     *    dataset.  Use ERM_fileOnly in order to force the presence of a meta header.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition loadFile(const char *fileName,
                                 const E_TransferSyntax readXfer = EXS_Unknown,
                                 const E_GrpLenEncoding groupLength = EGL_noChange,
                                 const Uint32 maxReadLength = DCM_MaxReadLength,
                                 const E_FileReadMode readMode = ERM_autoDetect);

    /** save object to a DICOM file.
     *  @param fileName name of the file to save
     *  @param writeXfer transfer syntax used to write the data (EXS_Unknown means use current)
     *  @param encodingType flag, specifying the encoding with undefined or explicit length
     *  @param groupLength flag, specifying how to handle the group length tags
     *  @param padEncoding flag, specifying how to handle the padding tags
     *  @param padLength number of bytes used for the dataset padding (has to be an even number)
     *  @param subPadLength number of bytes used for the item padding (has to be an even number)
     *  @param writeMode write file with or without meta header. Also allows for updating the
     *    information in the file meta information header.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition saveFile(const char *fileName,
                                 const E_TransferSyntax writeXfer = EXS_Unknown,
                                 const E_EncodingType encodingType = EET_UndefinedLength,
                                 const E_GrpLenEncoding groupLength = EGL_recalcGL,
                                 const E_PaddingEncoding padEncoding = EPD_noChange,
                                 const Uint32 padLength = 0,
                                 const Uint32 subPadLength = 0,
                                 const E_FileWriteMode writeMode = EWM_fileformat);

    // methods for different pixel representations

    /** select a specific representation (compressed or uncompressed) of the dataset
     *  and create the representation if needed. This may cause compression or decompression
     *  to be applied to the pixel data in the dataset.
     *  @param repType desired transfer syntax
     *  @param repParam desired representation parameter (e.g. quality factor for lossy compression)
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition chooseRepresentation(const E_TransferSyntax repType,
                                     const DcmRepresentationParameter *repParam)
    {
        return getDataset()->chooseRepresentation(repType, repParam);
    }

    /** check if all PixelData elements in this dataset have a representation conforming
     *  to the given transfer syntax and representation parameters (see dcpixel.h for
     *  definition of "conforming").
     *  @param repType desired transfer syntax
     *  @param repParam desired representation parameter (e.g. quality factor for lossy compression)
     *  @return true if all pixel elements have the desired representation, false otherwise
     */
    OFBool hasRepresentation(const E_TransferSyntax repType,
                             const DcmRepresentationParameter *repParam)
    {
        return getDataset()->hasRepresentation(repType, repParam);
    }

    /** removes all but the original representation in all pixel data elements
     */
    void removeAllButOriginalRepresentations()
    {
        getDataset()->removeAllButOriginalRepresentations();
    }

    /** removes all but the current representation in all pixel data elements.
     *  Makes the current representation original.
     */
    void removeAllButCurrentRepresentations()
    {
        getDataset()->removeAllButCurrentRepresentations();
    }

    /** get current file read mode.  This mode specifies whether a file is read as a
     *  fileformat or dataset (without meta header).  In addition, the reading can be
     *  restricted to DICOM files only.
     *  @return file read mode
     */
    E_FileReadMode getReadMode() const
    {
        return FileReadMode;
    }

    /** set current file read mode.  This mode specifies whether a file is read as a
     *  fileformat or dataset (without meta header).  In addition, the reading can be
     *  restricted to DICOM files only.
     *  @param readMode file read mode to be set
     */
    void setReadMode(const E_FileReadMode readMode)
    {
        FileReadMode = readMode;
    }

    /** method inherited from base class that shall not be used for instances of this class.
     *  Method immediately returns with error code.
     *  @param item item
     *  @param where where
     *  @return always returns EC_IllegalCall.
     */
    virtual OFCondition insertItem(DcmItem *item, const unsigned long where = DCM_EndOfListIndex);

    /** method inherited from base class that shall not be used for instances of this class.
     *  Method immediately returns.
     *  @param num num
     *  @return always returns NULL.
     */
    virtual DcmItem *remove(const unsigned long num);

    /** method inherited from base class that shall not be used for instances of this class.
     *  Method immediately returns.
     *  @param item item
     *  @return always returns NULL.
     */
    virtual DcmItem *remove(DcmItem *item);

  private:

    /** This function checks if a particular data element of the file meta information header is
     *  existent.  If the element is not existent, it will be inserted.  Additionally, this function
     *  makes sure that the corresponding data element will contain a correct value.
     *  @param metainfo  the meta header information
     *  @param dataset   the data set information
     *  @param atagkey   tag of the data element which shall be checked
     *  @param obj       data object from metainfo which represents the data element that shall be checked.
     *                   Equals NULL if this data element is not existent in the meta header information.
     *  @param oxfer     The transfer syntax which shall be used.
     *  @param writeMode flag indicating whether to update the file meta information or not
     *  @return EC_Normal if successful, an error code otherwise
     */
    static OFCondition checkMetaHeaderValue(DcmMetaInfo *metainfo,
                                            DcmDataset *dataset,
                                            const DcmTagKey &atagkey,
                                            DcmObject *obj,
                                            const E_TransferSyntax oxfer,
                                            const E_FileWriteMode writeMode);

    /** read DCM_TransferSyntaxUID from meta header dataset and return as E_TransferSyntax value
     *  @param metainfo meta-header dataset
     *  @return E_TransferSyntax value for DCM_TransferSyntaxUID, EXS_Unknown if not found or unknown
     */
    static E_TransferSyntax lookForXfer(DcmMetaInfo *metainfo);

    /// file read mode, specifies whether to read the meta header or not
    E_FileReadMode FileReadMode;
};


#endif // DCFILEFO_H


/*
** CVS/RCS Log:
** $Log: dcfilefo.h,v $
** Revision 1.38  2010-10-20 16:34:08  joergr
** Renamed method to avoid warnings reported by gcc with additional flags.
**
** Revision 1.37  2010-10-20 16:31:25  joergr
** Added missing API documentation.
**
** Revision 1.36  2010-10-14 13:15:41  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.35  2010-10-08 08:37:01  joergr
** Enhanced documentation on pixel-related parameters.
**
** Revision 1.34  2010-03-01 09:08:44  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.33  2010-02-22 11:39:53  uli
** Remove some unneeded includes.
**
** Revision 1.32  2009-08-25 13:00:23  joergr
** Added new methods which remove all data elements with an invalid group number
** from the meta information header, dataset and/or fileformat.
** Fixed wrong comment on clear() method.
**
** Revision 1.31  2009-08-21 09:18:07  joergr
** Added parameter 'writeMode' to save/write methods which allows for specifying
** whether to write a dataset or fileformat as well as whether to update the
** file meta information or to create a new file meta information header.
**
** Revision 1.30  2008-07-17 11:19:48  onken
** Updated copyFrom() documentation.
**
** Revision 1.29  2008-07-17 10:30:23  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.28  2008-06-23 12:09:13  joergr
** Fixed inconsistencies in Doxygen API documentation.
**
** Revision 1.27  2007/11/29 14:30:19  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.26  2006/08/15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.25  2005/12/08 16:28:13  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.24  2005/12/02 08:48:17  joergr
** Added new file read mode that makes it possible to distinguish between DICOM
** files, datasets and other non-DICOM files.  For this reason, the last
** parameter of method loadFile() changed from OFBool to E_FileReadMode.
**
** Revision 1.23  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.22  2002/12/06 12:49:10  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.21  2002/08/27 16:55:32  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.20  2002/08/20 12:18:35  meichel
** Changed parameter list of loadFile and saveFile methods in class
**   DcmFileFormat. Removed loadFile and saveFile from class DcmObject.
**
** Revision 1.19  2002/04/25 09:39:47  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.18  2002/04/11 12:22:51  joergr
** Added new methods for loading and saving DICOM files.
**
** Revision 1.17  2001/09/25 17:19:26  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.16  2001/06/01 15:48:40  meichel
** Updated copyright header
**
** Revision 1.15  2000/04/14 15:31:32  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.14  2000/03/08 16:26:14  meichel
** Updated copyright header.
**
** Revision 1.13  2000/03/03 14:05:23  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.12  2000/02/10 10:50:51  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.11  1999/03/31 09:24:38  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.10  1998/11/12 16:47:39  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.9  1997/07/21 08:14:39  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.8  1997/05/27 13:48:27  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.7  1997/05/16 08:23:47  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
** - Added a new method calcElementLength to calculate the length of an
**   element, item or sequence. For elements it returns the length of
**   tag, length field, vr field, and value length, for item and
**   sequences it returns the length of the whole item. sequence including
**   the Delimitation tag (if appropriate).  It can never return
**   UndefinedLength.
**
** Revision 1.6  1996/08/05 08:45:21  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.5  1996/07/29 15:46:23  andreas
** Add method getAndRemoveDataset to remove connection of dataset from fileformat
**
** Revision 1.4  1996/01/09 11:06:15  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.3  1996/01/05 13:22:55  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
