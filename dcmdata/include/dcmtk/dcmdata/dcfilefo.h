/*
 *
 *  Copyright (C) 1994-2014, OFFIS e.V.
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
class DCMTK_DCMDATA_EXPORT DcmFileFormat
  : public DcmSequenceOfItems
{

 public:

    /** default constructor
     */
    DcmFileFormat();

    /** constructor taking an existing dataset for initialization. Depending on the
     *  parameters, a deep or shallow copy is performed.
     *  @param dataset the dataset to be inserted into the new DcmFileFormat object. If NULL,
     *                 an empty dataset is created and inserted instead.
     *  @param deepCopy If OFTrue (default), a deep copy of the dataset is performed.
     *                  Otherwise only the pointer is copied and the ownership
     *                  of the dataset is taken over by the DcmFileFormat object.
     */
    DcmFileFormat(DcmDataset *dataset,
                  OFBool deepCopy = OFTrue);

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
     *  @param oxfer output transfer syntax (EXS_Unknown means use original)
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
    virtual OFCondition writeXML(STD_NAMESPACE ostream &out,
                                 const size_t flags = 0);

    /** load object from a DICOM file.
     *  This method supports DICOM objects stored as a file (with meta header) or as a
     *  dataset (without meta header).  By default, the presence of a meta header is
     *  detected automatically.
     *  @param fileName name of the file to load (may contain wide chars if support enabled).
     *    Since there are various constructors for the OFFilename class, a "char *", "OFString"
     *    or "wchar_t *" can also be passed directly to this parameter.
     *  @param readXfer transfer syntax used to read the data (auto detection if EXS_Unknown)
     *  @param groupLength flag, specifying how to handle the group length tags
     *  @param maxReadLength maximum number of bytes to be read for an element value.
     *    Element values with a larger size are not loaded until their value is retrieved
     *    (with getXXX()) or loadAllDataIntoMemory() is called.
     *  @param readMode read file with or without meta header, i.e. as a fileformat or a
     *    dataset.  Use ERM_fileOnly in order to force the presence of a meta header.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition loadFile(const OFFilename &fileName,
                                 const E_TransferSyntax readXfer = EXS_Unknown,
                                 const E_GrpLenEncoding groupLength = EGL_noChange,
                                 const Uint32 maxReadLength = DCM_MaxReadLength,
                                 const E_FileReadMode readMode = ERM_autoDetect);

    /** save object to a DICOM file.
     *  @param fileName name of the file to save (may contain wide chars if support enabled).
     *    Since there are various constructors for the OFFilename class, a "char *", "OFString"
     *    or "wchar_t *" can also be passed directly to this parameter.
     *  @param writeXfer transfer syntax used to write the data (EXS_Unknown means use original)
     *  @param encodingType flag, specifying the encoding with undefined or explicit length
     *  @param groupLength flag, specifying how to handle the group length tags
     *  @param padEncoding flag, specifying how to handle the padding tags
     *  @param padLength number of bytes used for the dataset padding (has to be an even number)
     *  @param subPadLength number of bytes used for the item padding (has to be an even number)
     *  @param writeMode write file with or without meta header. Also allows for updating the
     *    information in the file meta information header.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition saveFile(const OFFilename &fileName,
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

    /** convert all element values that are contained in the dataset and that are affected
     *  by SpecificCharacterSet from the given source character set to the given
     *  destination character set. The defined terms for a particular character set can
     *  be found in the DICOM standard, e.g. "ISO_IR 100" for ISO 8859-1 (Latin 1) or
     *  "ISO_IR 192" for Unicode in UTF-8. An empty string denotes the default character
     *  repertoire, which is ASCII (7-bit). If multiple values are given for 'fromCharset'
     *  (separated by a backslash) code extension techniques are used and escape sequences
     *  may be encountered in the source string to switch between the specified character
     *  sets.
     *  @param fromCharset name of the source character set(s) used for the conversion
     *  @param toCharset name of the destination character set used for the conversion.
     *    Only a single value is permitted (i.e. no code extensions).
     *  @param transliterate mode specifying whether a character that cannot be
     *    represented in the destination character encoding is approximated through one
     *    or more characters that look similar to the original one
     *  @param discardIllegal mode specifying whether characters that cannot be represented
     *    in the destination character encoding will be silently discarded
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition convertCharacterSet(const OFString &fromCharset,
                                            const OFString &toCharset,
                                            const OFBool transliterate = OFFalse,
                                            const OFBool discardIllegal = OFFalse);

    /** convert all element values that are contained in the dataset and that are affected
     *  by SpecificCharacterSet to the given destination character set. The source
     *  character set is determined automatically from the value of the
     *  SpecificCharacterSet (0008,0005) element. The defined terms for the destination
     *  character set can be found in the DICOM standard, e.g. "ISO_IR 100" for ISO 8859-1
     *  (Latin 1) or "ISO_IR 192" for Unicode in UTF-8. An empty string denotes the
     *  default character repertoire, which is ASCII (7-bit).
     *  NB: In case of a DICOMDIR, the SpecificCharacterSet in the main dataset is neither
     *      checked nor updated, since the Basic Directory IOD has no SOP Common Module.
     *  @param toCharset name of the destination character set used for the conversion.
     *    Only a single value is permitted (i.e. no code extensions).
     *  @param transliterate mode specifying whether a character that cannot be
     *    represented in the destination character encoding is approximated through one
     *    or more characters that look similar to the original one
     *  @param discardIllegal mode specifying whether characters that cannot be represented
     *    in the destination character encoding will be silently discarded
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition convertCharacterSet(const OFString &toCharset,
                                            const OFBool transliterate = OFFalse,
                                            const OFBool discardIllegal = OFFalse);

    /** convert all element values that are contained in the dataset and that are affected
     *  by SpecificCharacterSet from the currently selected source character set to the
     *  currently selected destination character set
     *  @param converter character set converter to be used to convert the element values
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition convertCharacterSet(DcmSpecificCharacterSet &converter);

    /** convert all element values that are contained in the dataset and that are
     *  affected by SpecificCharacterSet to UTF-8 (Unicode). The value of the
     *  SpecificCharacterSet (0008,0005) element is updated, set or deleted automatically
     *  if needed. The transliteration mode is disabled - see convertCharacterSet().
     *  NB: In case of a DICOMDIR, the SpecificCharacterSet in the main dataset is neither
     *      checked nor updated, since the Basic Directory IOD has no SOP Common Module.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition convertToUTF8();

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
